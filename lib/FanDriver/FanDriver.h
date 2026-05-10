#pragma once

#include <Arduino.h>

class FanDriver {
public:
  struct Config {
    uint8_t pin;
    uint8_t minPwm; // minimum PWM value to guarantee spin-up (0 = no minimum)
  };

  FanDriver() = default;

  void setup(const Config &cfg);
  void setDutyPct(float pct);
  bool isActive() const;

private:
  Config  m_cfg{255, 30};
  bool    m_active = false;
};
