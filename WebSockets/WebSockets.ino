// Для создания web-приложения.
#include <WiFi.h>
#include <WebServer.h>

// WebSockets by Markus Sattler
#include <WebSocketsServer.h>

// ArduinoJson by Benoit Blanchon
#include <ArduinoJson.h>

// Пользовательский файл, содержащий строку, содержащую html сайта.
#include <web-html.h>
// Пользовательский файл, содержащий классы для управления мотора, сенсора и таймера.
#include <Utils.h>

// Для реализации таймера
#include <chrono>
#include <string>

// Используется для дебага.
// Чтобы перейти к собственной точке доступа ESP закомментируйте #define строку
#define ESP_DEBUG
const char* LOCAL_SSID = "DancingCow";
const char* LOCAL_PASS = "perfectwe";

// С этими данными будет создана точка доступа.
const char* APP_SSID = "ESP32 TEST";
const char* APP_PASS = "1234567890";

// Задаёт время, через которое будет обнавляться информация с датчика тока.
const uint8_t SensorUpdateTime = 1000;
uint32_t SensorUpdate = 0;

C_L298N_Motor Motor;
C_INA219_Sensor Sensor;
C_Timer Timer;

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;

// IP адрес для дебага
IPAddress Actual_IP;

// Информация для точки доступа ESP32.
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);

  disableCore1WDT();

  Motor.SetupPins();
  // Sensor.SetupINA219();

#ifdef ESP_DEBUG
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif

#ifndef ESP_DEBUG
  WiFi.softAP(APP_SSID, APP_PASS);
  delay(500);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
#endif

  server.on("/", HTTP_GET, SendWebsite);

  server.begin();
  webSocket.begin();
  webSocket.onEvent(WebSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  if ((millis() - SensorUpdate) >= SensorUpdateTime) {
    SensorUpdate = millis();
    SendJson();
    Timer.CheckTime(Motor);
    Sensor.UpdateSensor();
  }
}

void SendWebsite() {
  server.send(200, "text/html", PAGE_MAIN);
  Serial.println("Sending HTML!");
}

void SendJson() {
  String jsonString = "";
  JsonObject jsonMessage = doc_tx.to<JsonObject>();
  jsonMessage["RPM"] = Motor.GetSpeed();
  // Serial.println(Motor.GetSpeed());
  std::pair<float, float> sensorData = Sensor.GetSensorData();
  jsonMessage["VOLT"] = sensorData.first;
  // Serial.println(sensorData.first);
  jsonMessage["CURR"] = sensorData.second;
  // Serial.println(sensorData.second);
  jsonMessage["MOTOR_STATE"] = (Motor.GetState() ? "ON" : "OFF");
  // Serial.println(int(Motor.GetState()));
  jsonMessage["TIMER_STATE"] = (Timer.GetState() ? "ON" : "OFF");
  // Serial.println(int(Timer.GetState()));
  jsonMessage["TIME_LEFT"] = Timer.GetTimeRemaining();
  // Serial.println(Timer.GetTimeRemaining());
  serializeJson(doc_tx, jsonString);
  webSocket.broadcastTXT(jsonString);
}

void WebSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.println("Client Disconnected");
    break;
  case WStype_CONNECTED:
    Serial.println("Client Connected");
    break;
  case WStype_TEXT:
    DeserializationError error = deserializeJson(doc_rx, payload);
    if (error) {
      Serial.println("deserializeJson() failed!");
      return;
    }
    String messageType = String(doc_rx["type"]);
    if (messageType == "CHANGE-STATE") {
      Motor.SwitchState();
      if (Motor.GetState()) {
        Timer.RefreshStartTime();
      } else {
        Timer.SetState(false);
        Timer.SetTime(0, 0, 0);
      }
    }
    if (messageType == "CHANGE-ROTATION") {
      Motor.ProcessReverse();
    }
    if (messageType == "CHANGE-SPEED") {
      uint8_t newSpeed = int(doc_rx["speed"]);
      Motor.SetSpeed(newSpeed);
    }
    if (messageType == "UPDATE-TIMER") {
      Timer.SetTime(doc_rx["hours"], doc_rx["minutes"], doc_rx["seconds"]);
      Timer.RefreshStartTime();
      Timer.SetState(true);
    }
    if (messageType == "DISABLE-TIMER") {
      Timer.SetState(false);
      Timer.SetTime(0, 0, 0);
    }
  }
}