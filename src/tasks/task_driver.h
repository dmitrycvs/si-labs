#pragma once

#include <Arduino.h>

namespace TaskDriver
{
  struct Config
  {
    uint8_t  fanPin;
    uint8_t  fanMinPwm;
    uint16_t periodMs;
  };

  void setup(const Config &cfg);
  void commandDutyPct(float pct);
  void tick();
  bool isActive();
}
