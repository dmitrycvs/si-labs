#include "task_output.h"

#include "ServoActuator.h"

namespace
{
  TaskOutput::Config g_cfg{7, 50};
  ServoActuator      g_servo;

  bool g_commanded = false;
  bool g_applied   = false;

  uint32_t g_lastMs = 0;
}

namespace TaskOutput
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_servo.setup(ServoActuator::Config{.pin = g_cfg.servoPin});

    g_commanded = false;
    g_applied   = false;
    g_servo.disable();

    g_lastMs = millis();
  }

  void commandState(bool on)
  {
    g_commanded = on;
  }

  void tick()
  {
    g_servo.update();

    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs) return;
    g_lastMs = now;

    if (g_applied != g_commanded)
    {
      g_applied = g_commanded;
      if (g_applied) g_servo.enable();
      else           g_servo.disable();
    }
  }

  bool isOn()
  {
    return g_applied;
  }
}
