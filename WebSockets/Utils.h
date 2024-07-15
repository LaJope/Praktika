// Для подключения датчика силы тока и напряжения INA219.
// Adagruin INA219 by Adafruit
#include <Wire.h>
#include <Adafruit_INA219.h>

#include <chrono>

// Пины для управления мотором спомощью L298N.
// Пин 14 отвечает за управление скоростью мотора.
#define PIN_MOTOR_SPEED 14
// Пины 26-27 отвечают за направление вращения мотора.
#define PIN_MOTOR_1 26
#define PIN_MOTOR_2 27

enum class Rotation : byte {
  CLOCKWISE = 0,
  ANTI_CLOCKWISE = 1
};
struct C_L298N_Motor {
private:

  const uint16_t _speedChangeTime = 200;
  uint8_t _speed = 0;
  bool _isOn = false;
  Rotation _motorRotation = Rotation::CLOCKWISE;

  void ProcessState() {
    if (_isOn) {
      Rotate();
      return;
    }
    SlowDown(0);
    ledcWrite(PIN_MOTOR_SPEED, _speed);
    digitalWrite(PIN_MOTOR_1, LOW);
    digitalWrite(PIN_MOTOR_2, LOW);
  }

  void Rotate() {
    switch (_motorRotation) {
    case (Rotation::CLOCKWISE):
      digitalWrite(PIN_MOTOR_1, LOW);
      digitalWrite(PIN_MOTOR_2, HIGH);
      break;
    case (Rotation::ANTI_CLOCKWISE):
      digitalWrite(PIN_MOTOR_1, HIGH);
      digitalWrite(PIN_MOTOR_2, LOW);
      break;
    default:
      digitalWrite(PIN_MOTOR_1, LOW);
      digitalWrite(PIN_MOTOR_2, LOW);
      break;
    }
  }

  void SpeedUp(uint8_t aimSpeed) {
    aimSpeed = (aimSpeed > 255) ? 255 : aimSpeed;
    while (_speed < aimSpeed - 20) {
      _speed += 20;
      ledcWrite(PIN_MOTOR_SPEED, _speed);
      delay(_speedChangeTime);
    }
    _speed = aimSpeed;
    ledcWrite(PIN_MOTOR_SPEED, _speed);
  }

  void SlowDown(uint8_t aimSpeed) {
    aimSpeed = (aimSpeed < 0) ? 0 : aimSpeed;
    while (_speed > aimSpeed + 20) {
      _speed -= 20;
      ledcWrite(PIN_MOTOR_SPEED, _speed);
      delay(_speedChangeTime);
    }
    _speed = aimSpeed;
    ledcWrite(PIN_MOTOR_SPEED, _speed);
  }

public:

  void SetupPins() {
    const int freq = 30000;
    const int pwmChannel = 0;
    const int resolution = 8;

    pinMode(PIN_MOTOR_SPEED, OUTPUT);
    pinMode(PIN_MOTOR_1, OUTPUT);
    pinMode(PIN_MOTOR_2, OUTPUT);

    ledcAttachChannel(PIN_MOTOR_SPEED, freq, resolution, pwmChannel);
    digitalWrite(PIN_MOTOR_1, LOW);
    digitalWrite(PIN_MOTOR_2, LOW);
  }

  void SetSpeed(uint8_t speed) {
    _speed = speed;
    ledcWrite(PIN_MOTOR_SPEED, _speed);
  }

  uint8_t GetSpeed() { return _speed; }

  void SwitchState() {
    _isOn ^= 1;
    ProcessState();
  }

  bool GetState() { return _isOn; }

  void ProcessReverse() {
    uint8_t tempSpeed = _speed;
    SlowDown(0);
    _motorRotation = static_cast<Rotation>((int)_motorRotation ^ 1);
    Rotate();
    delay(_speedChangeTime);
    SpeedUp(tempSpeed);
  }
};

struct C_Timer {
private:

  uint32_t _time = 0;
  std::chrono::steady_clock::time_point _startTime = std::chrono::steady_clock::now();
  bool _isOn = false;

public:

  void SetTime(uint8_t hour, uint8_t min, uint8_t sec) {
    hour = ((hour > 23) ? 23 : hour);
    min = ((min > 59) ? 59 : min);
    sec = ((sec > 59) ? 59 : sec);
    _time = hour * 3600 + min * 60 + sec;
  }

  const String& GetTimeRemaining() {
    std::chrono::seconds temp_all_time = std::chrono::seconds(_time);
    std::chrono::seconds temp_elapsed_time =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _startTime);
    uint32_t time_left = (temp_all_time >= temp_elapsed_time) ? (temp_all_time - temp_elapsed_time).count() : 0;
    return String(time_left / 3600) + ":" + String(time_left % 3600 / 60) + ":" + String(time_left % 60);
  }

  void RefreshStartTime() { _startTime = std::chrono::steady_clock::now(); }

  void SetState(bool state) { _isOn = state; }

  bool GetState() { return _isOn; }

  void CheckTime(C_L298N_Motor& motor) {
    if (motor.GetState() &&
      _isOn &&
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _startTime) >=
      std::chrono::seconds(_time)) {
      motor.SwitchState();
      SetState(false);
      SetTime(0, 0, 0);
    }
  }
};

struct C_INA219_Sensor {
private:

  Adafruit_INA219 _ina219;
  float _voltage_V = 0.0f;
  float _current_mA = 0.0f;

public:

  void SetupINA219() {
    if (!_ina219.begin()) {
      while (1) { delay(10); }
    }
  }

  void UpdateSensor() {
    _voltage_V = _ina219.getBusVoltage_V();
    _current_mA = _ina219.getCurrent_mA();
  }

  std::pair<float, float> GetSensorData() {
    return { _voltage_V, _current_mA };
  }
};