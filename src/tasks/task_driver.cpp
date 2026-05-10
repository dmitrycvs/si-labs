#include "task_driver.h"

#include "FanDriver.h"

namespace
{
  TaskDriver::Config g_cfg{5, 30, 50};
  FanDriver          g_fan;

  float    g_commanded = 0.0f;
  uint32_t g_lastMs    = 0;
}

namespace TaskDriver
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_fan.setup(FanDriver::Config{.pin = g_cfg.fanPin, .minPwm = g_cfg.fanMinPwm});

    g_commanded = 0.0f;
    g_fan.setDutyPct(0.0f);

    g_lastMs = millis();
  }

  void commandDutyPct(float pct)
  {
    g_commanded = pct;
  }

  void tick()
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs) return;
    g_lastMs = now;

    g_fan.setDutyPct(g_commanded);
  }

  bool isActive()
  {
    return g_fan.isActive();
  }
}
