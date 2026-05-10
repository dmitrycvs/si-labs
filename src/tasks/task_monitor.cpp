#include "task_monitor.h"
#include <stdio.h>

namespace
{
  TaskMonitor::Config g_cfg{2000};
  uint32_t g_lastMs = 0;

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

namespace TaskMonitor
{
  void setup(const Config &cfg)
  {
    g_cfg    = cfg;
    g_lastMs = millis();

    printf("\n--- System Ready ---\n");
  }

  void tick(const TaskSensor::State &s, bool fanOn)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs) return;
    g_lastMs = now;

    if (s.readingValid)
    {
      const int h10   = toTenths(s.humidPct);
      const int pid10 = toTenths(s.pidOut);
      const int pwm10 = toTenths(s.pwmDuty);

      // Serial Plotter line (> prefix)
      printf(">SetPoint:%d,Humidity:%d.%d,Output:%d.%d\n",
             s.targetPct,
             h10 / 10, absVal(h10 % 10),
             pid10 / 10, absVal(pid10 % 10));

      // Human-readable line (# prefix)
      printf("# Hum=%d.%d %% | SP=%d %% | PID=%d.%d %% | Fan=%d.%d %%\n",
             h10 / 10, absVal(h10 % 10),
             s.targetPct,
             pid10 / 10, absVal(pid10 % 10),
             pwm10 / 10, absVal(pwm10 % 10));
    }
    else
    {
      printf(">SetPoint:%d,Humidity:0.0,Output:0.0\n", s.targetPct);
      printf("# Hum=n/a | SP=%d %% | PID=0.0 %% | Fan=0.0 %%\n", s.targetPct);
    }
  }
}
