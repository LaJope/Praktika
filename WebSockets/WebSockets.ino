// Для создания web-приложения.
#include <WiFi.h>
#include <WebServer.h>

// WebSockets by Markus Sattler
#include <WebSocketsServer.h>

// ArduinoJson by Benoit Blanchon
#include <ArduinoJson.h>

// Пользовательский файл, содержащий строку, содержащую html сайта.
#include <web-html.h>

// Для подключения датчика силы тока и напряжения INA219.
// Adagruin INA219 by Adafruit
#include <Wire.h>
#include <Adafruit_INA219.h>

// Для реализации таймера
#include <ctime>
#include <chrono>

#include <string>

// Пины для управления мотором спомощью L298N.
// Пин 3 отвечает за управление скоростью мотора.
// #define PIN_MOTOR_SPEED 25 // 3
// Пины 4-5 отвечают за направление вращения мотора.
// #define PIN_MOTOR_1 32 // 4
// #define PIN_MOTOR_2 33 // 5

// Используется для дебага.
// Чтобы перейти к собственной точке доступа ESP закомментируйте #define строку
#define ESP_DEBUG
const char *LOCAL_SSID = "DancingCow";
const char *LOCAL_PASS = "perfectwe";

// С этими данными будет создана точка доступа.
const char *APP_SSID = "ESP32 TEST";
const char *APP_PASS = "1234567890";

// Задаёт время, через которое будет обнавляться информация с датчика тока.
const uint8_t SensorUpdateTime = 1000;
uint32_t SensorUpdate = 0;

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

void setup()
{
  Serial.begin(115200);

  disableCore1WDT();

  Serial.println("Starting Server!");

#ifdef ESP_DEBUG
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
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
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  server.handleClient();
  webSocket.loop();
  if ((millis() - SensorUpdate) >= SensorUpdateTime)
  {
    SensorUpdate = millis();
    String jsonString = "";
    JsonObject object = doc_tx.to<JsonObject>();
    object["RPM"] = random(100);
    object["CURR"] = random(100);
    serializeJson(doc_tx, jsonString);
    webSocket.broadcastTXT(jsonString);
  }
}

void SendWebsite()
{
  server.send(200, "text/html", PAGE_MAIN);
  Serial.println("Sending HTML!");
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length) {
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
      const uint8_t speed = doc_rx["SPEED"];
      Serial.print("Recieved speed ");
      Serial.println(speed);

      break;
  }
}