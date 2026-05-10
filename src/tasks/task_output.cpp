#include "task_output.h"

#include "RelayOutput.h"

namespace
{
  TaskOutput::Config g_cfg{12, 100};
  RelayOutput        g_relay;

  bool g_commanded = false;
  bool g_applied   = false;

  uint32_t g_lastMs = 0;
}

namespace TaskOutput
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_relay.setup(RelayOutput::Config{.pin = g_cfg.relayPin, .activeHigh = true});

    g_commanded = false;
    g_applied   = false;
    g_relay.set(false);

    g_lastMs = millis();
  }

  void commandState(bool on)
  {
    g_commanded = on;
  }

  void tick()
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs) return;
    g_lastMs = now;

    if (g_applied != g_commanded)
    {
      g_applied = g_commanded;
      g_relay.set(g_applied);
    }
  }

  bool isOn()
  {
    return g_applied;
  }
}
