#pragma once

#include <Arduino.h>

namespace TaskAcquisition
{
  struct Config
  {
    uint8_t  dhtPin;
    uint8_t  upButtonPin;
    uint8_t  downButtonPin;
    bool     buttonsActiveLow;
    uint16_t buttonDebounceMs;

    int      setpointInitPct;
    int      setpointMinPct;
    int      setpointMaxPct;
    int      setpointStepPct;
    float    hysteresisPct;

    uint16_t buttonPeriodMs;
    uint16_t samplePeriodMs;
    uint16_t controlPeriodMs;
  };

  struct State
  {
    float humidityPct;
    bool  sensorValid;
    int   setpointPct;
    float lowerBoundPct;
    float upperBoundPct;
    bool  relayRequestOn;
  };

  void  setup(const Config &cfg);
  void  tick();
  State getState();
}
