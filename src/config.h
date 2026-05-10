#pragma once
#include <Arduino.h>
#include "tasks/task_sensor.h"
#include "tasks/task_driver.h"
#include "tasks/task_monitor.h"

namespace Config
{
  constexpr uint8_t DHT_PIN      = 3;
  constexpr uint8_t FAN_PIN      = 5;   // PWM-capable pin
  constexpr uint8_t BTN_UP_PIN   = 9;
  constexpr uint8_t BTN_DOWN_PIN = 8;

  static constexpr TaskSensor::Config SENSOR_CFG{
      .dhtPin        = DHT_PIN,
      .upPin         = BTN_UP_PIN,
      .downPin       = BTN_DOWN_PIN,
      .activeLow     = true,
      .debounceMs    = 50,
      .targetInit    = 50,
      .targetMin     = 20,
      .targetMax     = 80,
      .targetStep    = 1,
      .kp            = 3.0f,
      .ki            = 0.05f,
      .kd            = 0.5f,
      .outMin        = 0.0f,
      .outMax        = 100.0f,
      .btnPeriodMs   = 50,
      .measurePeriodMs = 2000,
      .pidPeriodMs   = 2000,
  };

  static constexpr TaskDriver::Config DRIVER_CFG{
      .fanPin    = FAN_PIN,
      .fanMinPwm = 30,
      .periodMs  = 50,
  };

  static constexpr TaskMonitor::Config MONITOR_CFG{
      .periodMs = 2000,
  };
}
