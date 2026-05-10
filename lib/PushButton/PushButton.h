#pragma once

#include <Arduino.h>

class PushButton {
public:
  struct Config {
    uint8_t  pin;
    bool     activeLow;
    uint16_t debounceMs;
  };

  PushButton() = default;

  void setup(const Config &cfg);

  // Returns true once per stable press (rising edge after debounce).
  bool pollRisingEdge();

private:
  Config   m_cfg{255, true, 25};
  bool     m_prevRaw     = false;
  bool     m_stableState = false;
  uint32_t m_changeMs    = 0;
};
