#pragma once

#include <Arduino.h>

class ServoActuator {
public:
  struct Config {
    uint8_t pin;
  };

  ServoActuator() = default;

  void setup(const Config &cfg);
  void writeDeg(int deg);

  void enable();
  void disable();
  void update();

  int getPos() const;

private:
  uint8_t  m_pin     = 255;
  int      m_pos     = 0;
  bool     m_running = false;
  int      m_dir     = 1;
  uint32_t m_lastStepMs = 0;

  static constexpr int     POS_MIN = 0;
  static constexpr int     POS_MAX = 90;
  static constexpr int     STEP_DEG = 5;
  static constexpr uint16_t STEP_MS  = 10;
};
