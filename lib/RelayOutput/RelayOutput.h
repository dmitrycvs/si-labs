#pragma once

#include <Arduino.h>

class RelayOutput {
public:
  struct Config {
    uint8_t pin;
    bool    activeHigh;
  };

  RelayOutput() = default;

  void setup(const Config &cfg);
  void set(bool on);
  bool isOn() const;

private:
  Config m_cfg{255, true};
  bool   m_on = false;
};
