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

    int      setpointInitDeg;
    int      setpointMinDeg;
    int      setpointMaxDeg;
    int      setpointStepDeg;
    float    hysteresisDeg;

    uint16_t buttonPeriodMs;
    uint16_t samplePeriodMs;
    uint16_t controlPeriodMs;
  };

  struct State
  {
    float temperatureCelsius;
    bool  sensorValid;
    int   setpointDeg;
    float lowerBoundDeg;
    float upperBoundDeg;
    bool  relayRequestOn;
  };

  void  setup(const Config &cfg);
  void  tick();
  State getState();
}
