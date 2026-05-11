#pragma once

#include <Arduino.h>

namespace TaskOutput
{
  struct Config
  {
    uint8_t  servoPin;
    uint16_t periodMs;
  };

  void setup(const Config &cfg);
  void commandState(bool on);
  void tick();
  bool isOn();
}
