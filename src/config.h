#pragma once
#include <Arduino.h>
#include "tasks/task_acquisition.h"
#include "tasks/task_output.h"
#include "tasks/task_logger.h"

namespace Config
{
  constexpr uint8_t DHT_PIN      = 3;
  constexpr uint8_t RELAY_PIN    = 12;
  constexpr uint8_t BTN_UP_PIN   = 9;
  constexpr uint8_t BTN_DOWN_PIN = 8;

  static constexpr TaskAcquisition::Config ACQUISITION_CFG{
      .dhtPin           = DHT_PIN,
      .upButtonPin      = BTN_UP_PIN,
      .downButtonPin    = BTN_DOWN_PIN,
      .buttonsActiveLow = true,
      .buttonDebounceMs = 50,
      .setpointInitDeg  = 25,
      .setpointMinDeg   = 10,
      .setpointMaxDeg   = 40,
      .setpointStepDeg  = 1,
      .hysteresisDeg    = 2.0f,
      .buttonPeriodMs   = 50,
      .samplePeriodMs   = 2000,
      .controlPeriodMs  = 1000,
  };

  static constexpr TaskOutput::Config OUTPUT_CFG{
      .relayPin = RELAY_PIN,
      .periodMs = 100,
  };

  static constexpr TaskLogger::Config LOGGER_CFG{
      .periodMs = 2000,
  };
}
