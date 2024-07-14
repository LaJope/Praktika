// Для создания web-приложения.
#include <WiFi.h>
#include <WebServer.h>
// Пользовательский файл, содержащий строку, содержащую html сайта.
#include <web-html.h>

// Для подключения датчика силы тока и напряжения INA219.
#include <Wire.h>
#include <Adafruit_INA219.h>

// Для реализации таймера
#include <ctime>
#include <chrono>

#include <string>

// Пины для управления мотором спомощью L298N.
// Пин 3 отвечает за управление скоростью мотора.
#define PIN_MOTOR_SPEED 25 // 3
// Пины 4-5 отвечают за направление вращения мотора.
#define PIN_MOTOR_1 32 // 4
#define PIN_MOTOR_2 33 // 5

// Используется для дебага.
// Чтобы перейти к собственной точке доступа ESP закомментируйте #define строку
// #define ESP_DEBUG
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

enum class Rotation : byte
{
  CLOCKWISE = 0,
  ANTI_CLOCKWISE = 1
};
Rotation Motor_Rotation = Rotation::CLOCKWISE;

std::chrono::hours Time_Hour = std::chrono::hours(0);
std::chrono::minutes Time_Min = std::chrono::minutes(0);
std::chrono::seconds Time_Sec = std::chrono::seconds(0);
std::chrono::steady_clock::time_point StartTime = std::chrono::steady_clock::now();
bool Timer_On = false;

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

void setup()
{
  Serial.begin(115200);

  disableCore1WDT();

  // Установка всех управляющих пинов в режим выхода
  // pinMode(PIN_MOTOR_SPEED, OUTPUT);
  // pinMode(PIN_MOTOR_1, OUTPUT);
  // pinMode(PIN_MOTOR_2, OUTPUT);
  // Команда остановки
  // analogWrite(PIN_MOTOR_SPEED, 0);
  // digitalWrite(PIN_MOTOR_1, LOW);
  // digitalWrite(PIN_MOTOR_2, LOW);

  // Проверка на наличие INA219
  // if (! Sensor.begin()) {
  //  while (1) { delay(10); }
  //}

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
  server.on("/xml", HTTP_GET, SendXML);
  server.on("/UPDATE_RPM", HTTP_PUT, UpdateRPM);
  server.on("/SET_TIMER", HTTP_POST, SetTimer);
  server.on("/SET_TIMER_HOUR", HTTP_PUT, SetTimerHour);
  server.on("/SET_TIMER_MIN", HTTP_PUT, SetTimerMin);
  server.on("/SET_TIMER_SEC", HTTP_PUT, SetTimerSec);
  server.on("/DISABLE_TIMER", HTTP_POST, DisableTimer);
  server.on("/STATE_BUTTON", HTTP_POST, ProcessState);
  server.on("/REVERSE_BUTTON", HTTP_POST, ProcessReverse);

  server.begin();

  Serial.println("Server Started!");
}

void temp()
{
  server.send(200, "text/plain", "1");
}

void loop()
{
  if ((millis() - SensorUpdate) >= SensorUpdateTime)
  {
    SensorUpdate = millis();
    Voltage_V = Sensor.getBusVoltage_V();
    Current_mA = Sensor.getCurrent_mA();
    CheckTime();
  }
  server.handleClient();
}

void SendWebsite()
{
  server.send(200, "text/html", PAGE_MAIN);
  Serial.println("Sending HTML!");
}

void SendXML()
{
  strcpy(XML, "<?xml version='1.0'?>\n<Data>\n");
  sprintf(buf, "<RPM>%d</RPM>\n", Motor_Speed);
  strcat(XML, buf);
  sprintf(buf, "<VOLT>%f</VOLT>\n", Voltage_V);
  strcat(XML, buf);
  sprintf(buf, "<CURR>%f</CURR>\n", Current_mA);
  strcat(XML, buf);
  sprintf(buf, "<STATE>%s</STATE>\n", (Motor_On ? "ON" : "OFF"));
  strcat(XML, buf);

  std::chrono::seconds temp_all_time = std::chrono::seconds(Time_Hour) + std::chrono::seconds(Time_Min) + std::chrono::seconds(Time_Sec);
  std::chrono::seconds temp_elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime);
  uint32_t time_left = (temp_all_time >= temp_elapsed_time) ? (temp_all_time - temp_elapsed_time).count() : 0;

  sprintf(buf, "<TIME-LEFT>%02d:", time_left / 3600);
  strcat(XML, buf);
  sprintf(buf, "%02d:", time_left % 3600 / 60);
  strcat(XML, buf);
  sprintf(buf, "%02d</TIME-LEFT>\n", time_left % 60);
  strcat(XML, buf);
  sprintf(buf, "</Data>");
  strcat(XML, buf);
  server.send(200, "text/xml", XML);
}

void UpdateRPM()
{
  String new_speed = server.arg("SPEED");
  Motor_Speed = new_speed.toInt();
  // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
  sprintf(buf, "%d", Motor_Speed);
  server.send(200, "text/plain", buf);
  Serial.print("Updating RPM!");
  Serial.println(Motor_Speed);
}

void SetTimer()
{
  Timer_On = true;
  server.send(200, "text/plain", "");
}
void SetTimerHour()
{
  String new_hour = server.arg("HOUR");
  Time_Hour = std::chrono::hours(new_hour.toInt());
  server.send(200, "text/plain", "");
}
void SetTimerMin()
{
  String new_min = server.arg("MIN");
  Time_Min = std::chrono::minutes(new_min.toInt());
  server.send(200, "text/plain", "");
}
void SetTimerSec()
{
  String new_sec = server.arg("SEC");
  Time_Sec = std::chrono::seconds(new_sec.toInt());
  server.send(200, "text/plain", "");
}
void DisableTimer()
{
  Timer_On = false;
  Time_Hour = std::chrono::hours(0);
  Time_Min = std::chrono::minutes(0);
  Time_Sec = std::chrono::seconds(0);
  server.send(200, "text/plain", "");
}

void ProcessState()
{
  Serial.println("STATE BUTTON!");
  Motor_On ^= 1;
  if (Motor_On == false)
  {
    SlowDown(0);
    // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
    // digitalWrite(PIN_MOTOR_1, LOW);
    // digitalWrite(PIN_MOTOR_2, LOW);u
  }
  else
  {
    StartTime = std::chrono::steady_clock::now();
    MotorRotation();
  }
  server.send(200, "text/plain", "");
}

void ProcessReverse()
{
  Serial.println("Reverse START");
  uint8_t temp_speed = Motor_Speed;
  SlowDown(0);
  MotorRotation();
  Motor_Rotation = static_cast<Rotation>((int)Motor_Rotation ^ 1);
  delay(SpeedChangeTime);
  SpeedUp(temp_speed);
  server.send(200, "text/plain", "DONE");
  Serial.println("Reverse STOP");
}

void SpeedUp(uint8_t aim_speed)
{
  aim_speed = (aim_speed > 255) ? 255 : aim_speed;
  while (Motor_Speed < aim_speed - 20)
  {
    Motor_Speed += 20;
    // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
    delay(SpeedChangeTime);
    server.handleClient();
  }
  Motor_Speed = aim_speed;
  // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
}

void SlowDown(uint8_t aim_speed)
{
  aim_speed = (aim_speed < 0) ? 0 : aim_speed;
  while (Motor_Speed > aim_speed + 20)
  {
    Motor_Speed -= 20;
    // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
    delay(SpeedChangeTime);
    server.handleClient();
  }
  Motor_Speed = aim_speed;
  // analogWrite(PIN_MOTOR_SPEED, Motor_Speed);
}

void MotorRotation()
{
  switch (Motor_Rotation)
  {
  case (Rotation::CLOCKWISE):
    // digitalWrite(PIN_MOTOR_1, LOW);
    // digitalWrite(PIN_MOTOR_2, HIGH);
    break;
  case (Rotation::ANTI_CLOCKWISE):
    // digitalWrite(PIN_MOTOR_1, HIGH);
    // digitalWrite(PIN_MOTOR_2, LOW);
    break;
  }
}

void CheckTime()
{
  if (Motor_On && Timer_On && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime) >= (std::chrono::seconds(Time_Hour) + std::chrono::seconds(Time_Min) + std::chrono::seconds(Time_Sec)))
  {
    Motor_On = false;
    SlowDown(0);
    Timer_On = false;
    Time_Hour = std::chrono::hours(0);
    Time_Min = std::chrono::minutes(0);
    Time_Sec = std::chrono::seconds(0);
  }
}