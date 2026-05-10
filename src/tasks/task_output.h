#pragma once

#include <Arduino.h>

namespace TaskOutput
{
  struct Config
  {
    uint8_t  relayPin;
    uint16_t periodMs;
  };

  void setup(const Config &cfg);
  void commandState(bool on);
  void tick();
  bool isOn();
}
