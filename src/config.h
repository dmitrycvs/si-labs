#pragma once
#include <Arduino.h>
#include "tasks/task_acquisition.h"
#include "tasks/task_output.h"
#include "tasks/task_logger.h"

namespace Config
{
  constexpr uint8_t DHT_PIN      = 3;
  constexpr uint8_t SERVO_PIN    = 7;
  constexpr uint8_t BTN_UP_PIN   = 9;
  constexpr uint8_t BTN_DOWN_PIN = 8;

  static constexpr TaskAcquisition::Config ACQUISITION_CFG{
      .dhtPin           = DHT_PIN,
      .upButtonPin      = BTN_UP_PIN,
      .downButtonPin    = BTN_DOWN_PIN,
      .buttonsActiveLow = true,
      .buttonDebounceMs = 50,
      .setpointInitPct  = 55,
      .setpointMinPct   = 30,
      .setpointMaxPct   = 90,
      .setpointStepPct  = 1,
      .hysteresisPct    = 3.0f,
      .buttonPeriodMs   = 50,
      .samplePeriodMs   = 2000,
      .controlPeriodMs  = 1000,
  };

  static constexpr TaskOutput::Config OUTPUT_CFG{
      .servoPin = SERVO_PIN,
      .periodMs = 50,
  };

  static constexpr TaskLogger::Config LOGGER_CFG{
      .periodMs = 2000,
  };
}
