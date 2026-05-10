#pragma once

#include <Arduino.h>

class DhtSensor {
public:
  struct Config {
    uint8_t dataPin;
  };

  DhtSensor() = default;

  void setup(const Config &cfg);

  // Returns true when a valid temperature reading is obtained.
  bool readTemperature(float &outDegC);

private:
  uint8_t m_dataPin = 255;
};
