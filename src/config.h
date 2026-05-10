#pragma once
#include <stdint.h>

namespace Config {
  // East-West traffic light pins
  static constexpr uint8_t PIN_EW_GREEN  = 2;
  static constexpr uint8_t PIN_EW_YELLOW = 3;
  static constexpr uint8_t PIN_EW_RED    = 4;

  // North-South traffic light pins
  static constexpr uint8_t PIN_NS_GREEN  = 5;
  static constexpr uint8_t PIN_NS_YELLOW = 6;
  static constexpr uint8_t PIN_NS_RED    = 7;

  // NS priority request button
  static constexpr uint8_t PIN_BUTTON = 8;

  // Timing (milliseconds)
  static constexpr uint16_t GREEN_MS      = 5000;
  static constexpr uint16_t YELLOW_MS     = 2000;
  static constexpr uint16_t ALL_RED_MS    = 1000;
  static constexpr uint16_t DEBOUNCE_MS   = 50;
  static constexpr uint16_t BTN_PERIOD_MS = 50;
}
