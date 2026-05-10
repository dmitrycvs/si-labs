#pragma once

#include <Arduino.h>

#include "task_acquisition.h"

namespace TaskLogger
{
  struct Config
  {
    uint16_t periodMs;
  };

  void setup(const Config &cfg);
  void tick(const TaskAcquisition::State &sensor, bool relayOn);
}
