#include <WiFi.h>
#include <WebServer.h>
#include <web-html.h>

// С этими данными будет создана точка доступа.
#define APP_SSID "ESP32 TEST"
#define APP_PASS "1234567890"

#define PIN_A0 34 // Для теста. Пин 34
#define PIN_A1 35 // Для теста. Пин 35

int BitsA0 = 0, BitsA1 = 0;
uint32_t SensorUpdate = 0;

int Motor_speed = 0;

bool Motor_is_on = false;
bool Motor_Rotation = false; // false -- по часовой стрелке. true - против.

char XML[2048];
char buf[32];

// IP, используемый для дебага. Будет распределен сторонним роутером.
IPAddress Actual_IP;

// Информация для собственной точки доступа ESP32.
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(80);

void setup() {
  Serial.begin(115200);

  disableCore1WDT();

  Serial.println("Starting Server!");

  WiFi.softAP(APP_SSID, APP_PASS);
  delay(500);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("\nIP address: ");
  Serial.println(Actual_IP);

  // При попытке доступа к странице IP/, будет вызываться функция SendWebsite.
  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/UPDATE_RPM", UpdateRPM);
  server.on("/UPDATE_RPM_BUTTON", UpdateRPMButton);
  server.on("/STATE_BUTTON", ProcessState);
  server.on("/REVERSE_BUTTON", ProcessReverse);

  server.begin();
}

void loop() {
  if ((millis() - SensorUpdate) >= 50) {
    SensorUpdate = millis();
    BitsA0 = analogRead(PIN_A0);
    BitsA1 = analogRead(PIN_A1);
  }
  server.handleClient();
}

void SendWebsite() {
  Serial.println("Sending WebPage!");
  server.send(200, "text/html", PAGE_MAIN);  // Tickle Tickle Tickle
}

void SendXML() {
  Serial.println("Sending XML!");
  strcpy(XML, "<?xml version='1.0'?>\n<Data>\n");
  sprintf(buf, "<RMP>%d</RMP>\n", BitsA0);
  strcat(XML, buf);
  sprintf(buf, "<CURR>%d</CURR>\n", BitsA0);
  strcat(XML, buf);
  sprintf(buf, "<DIFF>%d</DIFF>\n", BitsA1);
  strcat(XML, buf);
  sprintf(buf, "</Data>");
  strcat(XML, buf);
  Serial.println(XML);
  server.send(200, "text/xml", XML);
}

void UpdateRPM() {
  String t_state = server.arg("VALUE");

  Motor_speed = t_state.toInt();
  Serial.print("UpdateSlider"); Serial.println(Motor_speed);

  ledcWrite(0, Motor_speed);
  // server.send(200, "text/plain", "");

  sprintf(buf, "%d", Motor_speed);
  server.send(200, "text/plain", buf);
}

void UpdateRPMButton() {
  String t_state = server.arg("VALUE");

  Motor_speed = t_state.toInt();
  Serial.print("UpdateSlider"); Serial.println(Motor_speed);

  ledcWrite(0, Motor_speed);

  sprintf(buf, "%d", Motor_speed);
  server.send(200, "text/plain", buf);
}

void ProcessState() {
  Motor_is_on ^= 1;
  sprintf(buf, "%s", (Motor_is_on ? "ON" : "OFF"));
  server.send(200, "text/plain", "");
}

void ProcessReverse() {
  int temp = Motor_speed;
  Serial.println("Start button Pressed!");
  while (Motor_speed > 10) {
    Motor_speed -= 10
  }
  // Stop motor. Reverse diff.
  while (Motor_speed < temp - 10) {
    Motor_speed += 10;
  }
  server.send(200, "text/plain", "");
}
