#pragma once

#include <Arduino.h>

namespace TaskSensor
{
  struct Config
  {
    uint8_t  dhtPin;
    uint8_t  upPin;
    uint8_t  downPin;
    bool     activeLow;
    uint16_t debounceMs;

    int   targetInit;
    int   targetMin;
    int   targetMax;
    int   targetStep;

    float kp;
    float ki;
    float kd;
    float outMin;
    float outMax;

    uint16_t btnPeriodMs;
    uint16_t measurePeriodMs;
    uint16_t pidPeriodMs;
  };

  struct State
  {
    float humidPct;
    bool  readingValid;
    int   targetPct;
    float pidOut;
    float pwmDuty;
    bool  fanActive;
  };

  void  setup(const Config &cfg);
  void  tick();
  State getState();
}
