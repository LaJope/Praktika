#include <Wire.h>               // Подключаем библиотеку Wire 
#include <Adafruit_INA219.h>    // Подключаем библиотеку Adafruit_INA219 
 
Adafruit_INA219 ina219;         // Создаем объект ina219

#define PIN_ENB 3 // Вывод управления скоростью вращения мотора №2
#define PIN_IN3 5 // Вывод управления направлением вращения мотора №2
#define PIN_IN4 4 // Вывод управления направлением вращения мотора №2

uint8_t power = 105; // Значение ШИМ (или скорости вращения)

void setup() {
  Serial.begin(115200);
  
  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  // Команда остановки двум моторам
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);

  uint32_t currentFrequency;    // Создаем переменную
    
  // По умолчанию при инициализации будет использоваться самый большой диапазон (32 В, 2 А)
  // Проверка инициализации модуля INA219
 
  if (! ina219.begin()) {       
    Serial.println("Failed to find INA219 chip");  
    while (1) { delay(10); }
  }
  
}

void loop() {
  // Вращаем моторы в одну сторону с разной скоростью
  while(power < 255) {
    analogWrite(PIN_ENB, power); // Устанавливаем скорость 2-го мотора
    // Задаём направление для 2-го мотора
    digitalWrite(PIN_IN3, HIGH);
    digitalWrite(PIN_IN4, LOW);
    delay(3000); // Пауза 3 секунды
    power += 30; // Увеличиваем скорость
    getInfo();
  }
  power = 225;
  // Вращаем моторы в другую сторону с разной скоростью
  while(power > 105) {
    analogWrite(PIN_ENB, power); // Устанавливаем скорость 2-го мотора
    // Задаём направление для 2-го мотора
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);
    delay(3000); // Пауза 3 секунды
    power -= 30; // Уменьшаем скорость
    getInfo();
  }
  power = 105;
}


float shuntvoltage = 0;     // Создаем переменную shuntvoltage 
float busvoltage = 0;       // Создаем переменную busvoltage 
float current_mA = 0;       // Создаем переменную current_mA 
float loadvoltage = 0;      // Создаем переменную loadvoltage
float power_mW = 0;         // Создаем переменную power_mW 

void getInfo() { 
  shuntvoltage = ina219.getShuntVoltage_mV();       // Получение напряжение на шунте 
  busvoltage = ina219.getBusVoltage_V();            // Получение значение напряжения V
  current_mA = ina219.getCurrent_mA();              // Получение значение тока в мА
  power_mW = ina219.getPower_mW();                  // Получение значение мощности
  loadvoltage = busvoltage + (shuntvoltage / 1000); // Расчет напряжение на нагрузки
  
  // Поочередно отправляем полученные значение в последовательный порт.
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
}
