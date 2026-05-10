#pragma once

#include <Arduino.h>

#include "task_sensor.h"

namespace TaskMonitor
{
  struct Config
  {
    uint16_t periodMs;
  };

  void setup(const Config &cfg);
  void tick(const TaskSensor::State &s, bool fanOn);
}
