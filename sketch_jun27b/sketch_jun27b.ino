// Для создания web-приложения.
#include <WiFi.h>
#include <WebServer.h>
// Пользовательский файл, содержащий строку, содержащую html сайта.
#include <web-html.h>

// Для подключения датчика силы тока и напряжения INA219.
#include <Wire.h>
#include <Adafruit_INA219.h>

// Пины для управления мотором спомощью L298N.
// Пин 3 отвечает за управление скоростью мотора.
#define PIN_MOTOR_SPEED 3
// Пины 4-5 отвечают за направление вращения мотора.
#define PIN_MOTOR_1 4
#define PIN_MOTOR_2 5

// Используется для дебага.
// Чтобы перейти к собственной точке доступа ESP закомментируйте #define строку 
#define ESP_DEBUG
const char *LOCAL_SSID = "DancingCow";
const char *LOCAL_PASS = "perfectwe";

// С этими данными будет создана точка доступа.
const char *APP_SSID = "ESP32 TEST"; 
const char *APP_PASS = "1234567890";

const uint16_t SpeedChangeTime = 200;

// Объект ina219, который используется для получения данных
// с физического сенсора посредством методов класса.
Adafruit_INA219 Sensor;

float Voltage_V = 0;
float Current_mA = 0;

// Задаёт время, через которое будет обнавляться информация с датчика тока.
const uint8_t SensorUpdateTime = 50; 
uint32_t SensorUpdate = 0;

uint8_t Motor_Speed = 0;
bool Motor_On = false;

enum class Rotation : byte { CLOCKWISE = 0, ANTI_CLOCKWISE = 1 };
Rotation Motor_Rotation = Rotation::CLOCKWISE;

char XML[2048];
char buf[32];

// IP адрес для дебага
IPAddress Actual_IP;

// Информация для точки доступа ESP32.
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(80);

void setup() {
  Serial.begin(115200);

  disableCore1WDT();

  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_MOTOR_SPEED, OUTPUT);
  pinMode(PIN_MOTOR_1, OUTPUT);
  pinMode(PIN_MOTOR_2, OUTPUT);
  // Команда остановки
  analogWrite(PIN_MOTOR_SPEED, 0);
  digitalWrite(PIN_MOTOR_1, LOW);
  digitalWrite(PIN_MOTOR_2, LOW);

  // Проверка на наличие INA219
  //if (! Sensor.begin()) {
  //  while (1) { delay(10); }
  //}

#ifdef ESP_DEBUG
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
  }
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
#endif

#ifndef ESP_DEBUG
  WiFi.softAP(APP_SSID, APP_PASS);
  delay(500);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
#endif

  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/UPDATE_RPM", UpdateRPM);
  server.on("/STATE_BUTTON", ProcessState);
  server.on("/REVERSE_BUTTON", ProcessReverse);

  server.begin();
}

void loop() {
  if ((millis() - SensorUpdate) >= SensorUpdateTime) {
    SensorUpdate = millis();
    Voltage_V = Sensor.getBusVoltage_V();
    Current_mA = Sensor.getCurrent_mA();
  }
  server.handleClient();
}

void SendWebsite() {
  server.send(200, "text/html", PAGE_MAIN);
}

void SendXML() {
  strcpy(XML, "<?xml version='1.0'?>\n<Data>\n");
  sprintf(buf, "<RPM>%d</RPM>\n", Motor_Speed);
  strcat(XML, buf);
  sprintf(buf, "<VOLT>%d</VOLT>\n", Voltage_V);
  strcat(XML, buf);
  sprintf(buf, "<CURR>%d</CURR>\n", Current_mA);
  strcat(XML, buf);
  sprintf(buf, "</Data>");
  strcat(XML, buf);
  server.send(200, "text/xml", XML);
}

void UpdateRPM() {
  String new_speed = server.arg("VALUE");
  Motor_Speed = new_speed.toInt();
  analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
  sprintf(buf, "%d", Motor_Speed);
  server.send(200, "text/plain", buf);
}

void ProcessState() {
  Motor_On ^= 1;
  if (Motor_On == false) {
    Motor_Speed = 0;
    analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
    digitalWrite(PIN_MOTOR_1, LOW); 
    digitalWrite(PIN_MOTOR_2, LOW);
  }
  else {
    MotorRotation();   
  }
  sprintf(buf, "%s", (Motor_On ? "ON" : "OFF"));
  server.send(200, "text/plain", buf);
}

void ProcessReverse() {
  Serial.println("Reverse START");
  int temp = Motor_Speed;
  SlowDown(0);
  Motor_Rotation = static_cast<Rotation>((int)Motor_Rotation ^ 1);
  MotorRotation();
  delay(SpeedChangeTime);
  SpeedUp(temp);
  server.send(200, "text/plain", "DONE");
  Serial.println("Reverse STOP");
}

void SpeedUp(uint8_t aim_speed) {
  aim_speed = (aim_speed > 255) ? 255 : aim_speed;
  while (Motor_Speed < aim_speed - 10) {
    Motor_Speed += 10;
    analogWrite(PIN_MOTOR_SPEED, 0);
    delay(SpeedChangeTime);
  }
  Motor_Speed = aim_speed;
}

void SlowDown(uint8_t aim_speed) {
  aim_speed = (aim_speed < 0) ? 0 : aim_speed;
  while (Motor_Speed > aim_speed + 10) {
    Motor_Speed -= 10;
    analogWrite(PIN_MOTOR_SPEED, 0);
    delay(SpeedChangeTime);
  }
  Motor_Speed = aim_speed;
}

void MotorRotation() {
  switch (Motor_Rotation) {
    case (Rotation::CLOCKWISE):
      digitalWrite(PIN_MOTOR_1, LOW);
      digitalWrite(PIN_MOTOR_2, HIGH);
      break;
    case (Rotation::ANTI_CLOCKWISE):
      digitalWrite(PIN_MOTOR_1, HIGH);
      digitalWrite(PIN_MOTOR_2, LOW);
      break;
    }
}