#include <WiFi.h>
#include <WebServer.h>
#include <web-html.h>

#include <Wire.h>               // Подключаем библиотеку Wire 
#include <Adafruit_INA219.h>    // Подключаем библиотеку Adafruit_INA219 
 
Adafruit_INA219 ina219;         // Создаем объект ina219
 

// Эти строчки нужны для дебага. Закоментируйте верхнюю, чтобы контроллер стал сервером.
//#define USE_INTRANET
#define LOCAL_SSID "Galaxy A52O49D"
#define LOCAL_PASS "perfectwe"

// С этими данными будет создана точка доступа.
#define APP_SSID "ESP32 TEST"
#define APP_PASS "1234567890"

#define PIN_OUTPUT 26
#define PIN_A0 34
#define PIN_A1 35

int BitsA0 = 0, BitsA1 = 0;
double VoltsA0 = 0, VoltsA1 = 0;
int FanSpeed = 0, FanRPM = 0;
uint32_t SensorUpdate = 0;

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

#ifdef USE_INTRANET
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

#ifndef USE_INTRANET
  WiFi.softAP(APP_SSID, APP_PASS);
  delay(500);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("\nIP address: ");
  Serial.println(Actual_IP);
#endif

  printWifiStatus();

  // При попытке доступа к странице IP/, будет вызываться функция SendWebsite.
  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/UPDATE_SLIDER", UpdateSlider);
  server.on("/BUTTON_0", ProcessButton_0);
  server.on("/BUTTON_1", ProcessButton_1);

  server.begin();
}

float shuntvoltage = 0;     // Создаем переменную shuntvoltage 
float busvoltage = 0;       // Создаем переменную busvoltage 
float current_mA = 0;       // Создаем переменную current_mA 
float loadvoltage = 0;      // Создаем переменную loadvoltage
float power_mW = 0;         // Создаем переменную power_mW 

void loop() {
  if ((millis() - SensorUpdate) >= 50) {
    // Serial.println("Reading Sensors");
 
    shuntvoltage = ina219.getShuntVoltage_mV();       // Получение напряжение на шунте 
    busvoltage = ina219.getBusVoltage_V();            // Получение значение напряжения V
    current_mA = ina219.getCurrent_mA();              // Получение значение тока в мА
    power_mW = ina219.getPower_mW();                  // Получение значение мощности
    loadvoltage = busvoltage + (shuntvoltage / 1000); // Расчет напряжение на нагрузки

    SensorUpdate = millis();
    BitsA0 = analogRead(PIN_A0);
    BitsA1 = analogRead(PIN_A1);

    VoltsA0 = BitsA0 * 3.3 / 4096;
    VoltsA1 = BitsA1 * 3.3 / 4096;
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

  sprintf(buf, "<B0>%d</B0>\n", BitsA0);
  strcat(XML, buf);

  sprintf(buf, "<V0>%d.%d</V0>\n", (int)(VoltsA0), abs((int)(VoltsA0 * 10) - ((int)(VoltsA0)*10)));

  sprintf(buf, "<B1>%d</B1>\n", BitsA1);
  strcat(XML, buf);

  sprintf(buf, "<V1>%d.%d</V1>\n", (int)(VoltsA1), abs((int)(VoltsA1 * 10) - ((int)(VoltsA1)*10)));
  strcat(XML, buf);

  strcat(XML, "</Data>\n");

  Serial.println(XML);
  server.send(200, "text/xml", XML);
}

void UpdateSlider() {
  String t_state = server.arg("VALUE");

  FanSpeed = t_state.toInt();
  Serial.print("UpdateSlider");
  Serial.println(FanSpeed);

  // ledcWrite(0, FanSpeed);

  FanRPM = map(FanSpeed, 0, 255, 0, 2400);
  strcpy(buf, "");
  sprintf(buf, "%d", FanRPM);
  sprintf(buf, buf);
  server.send(200, "text/plain", buf);
}

void ProcessButton_0() {
  Serial.println("Button 0 Pressed!");
  server.send(200, "text/plain", "");
}

void ProcessButton_1() {
  Serial.println("Button 1 Pressed!");
  server.send(200, "text/plain", "");
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("Open http://");
  Serial.println(ip);
}
