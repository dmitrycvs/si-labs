#include "task_logger.h"
#include <stdio.h>

namespace
{
  TaskLogger::Config g_cfg{2000};
  uint32_t g_lastMs = 0;
  uint32_t g_seq    = 0;

  int toTenths(float v)
  {
    const float s = v * 10.0f;
    return (s >= 0.0f) ? (int)(s + 0.5f) : (int)(s - 0.5f);
  }

  int absVal(int v)
  {
    return (v < 0) ? -v : v;
  }
}

namespace TaskLogger
{
  void setup(const Config &cfg)
  {
    g_cfg    = cfg;
    g_lastMs = millis();
    g_seq    = 0;

    printf("\n--- System Ready ---\n");
  }

  void tick(const TaskAcquisition::State &sensor, bool relayOn)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs) return;
    g_lastMs = now;

    if (sensor.sensorValid)
    {
      const int t10  = toTenths(sensor.temperatureCelsius);
      const int lo10 = toTenths(sensor.lowerBoundDeg);
      const int hi10 = toTenths(sensor.upperBoundDeg);

      printf(
          "[%lu] T=%d.%d C | SP=%d C | band=[%d.%d, %d.%d] C | relay=%s | sensor=OK\n",
          (unsigned long)g_seq++,
          t10 / 10, absVal(t10 % 10),
          sensor.setpointDeg,
          lo10 / 10, absVal(lo10 % 10),
          hi10 / 10, absVal(hi10 % 10),
          relayOn ? "ON" : "OFF");
    }
    else
    {
      const int lo10 = toTenths(sensor.lowerBoundDeg);
      const int hi10 = toTenths(sensor.upperBoundDeg);

      printf(
          "[%lu] T=n/a | SP=%d C | band=[%d.%d, %d.%d] C | relay=%s | sensor=ERR\n",
          (unsigned long)g_seq++,
          sensor.setpointDeg,
          lo10 / 10, absVal(lo10 % 10),
          hi10 / 10, absVal(hi10 % 10),
          relayOn ? "ON" : "OFF");
    }
  }
}
