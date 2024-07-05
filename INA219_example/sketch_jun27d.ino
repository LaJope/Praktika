#include <Wire.h>               // Подключаем библиотеку Wire 
#include <Adafruit_INA219.h>    // Подключаем библиотеку Adafruit_INA219 
 
Adafruit_INA219 ina219;         // Создаем объект ina219
 
void setup(void) 
{
  Serial.begin(115200);         // Инициализируем последовательную связь на скорости 115200
  uint32_t currentFrequency;    // Создаем переменную
    
// По умолчанию при инициализации будет использоваться самый большой диапазон (32 В, 2 А)
// Проверка инициализации модуля INA219
 
  if (! ina219.begin()) {       
    Serial.println("Failed to find INA219 chip");  
    while (1) { delay(10); }
  }
 
// Чтобы использовать немного более низкий диапазон 32 В, 1 А (более высокая точность на усилителях):
// ina219.setCalibration_32V_1A ();
// Или использовать более низкий диапазон 16 В, 400 мА (более высокая точность на вольт и ампер):
// ina219.setCalibration_16V_400mA ();
   
  Serial.println("Measuring voltage and current with INA219 ..."); // Отправка сообщение
}
 
void loop(void) 
{
  float shuntvoltage = 0;     // Создаем переменную shuntvoltage 
  float busvoltage = 0;       // Создаем переменную busvoltage 
  float current_mA = 0;       // Создаем переменную current_mA 
  float loadvoltage = 0;      // Создаем переменную loadvoltage
  float power_mW = 0;         // Создаем переменную power_mW 
 
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
 
  delay(2000);   // Пауза
}
