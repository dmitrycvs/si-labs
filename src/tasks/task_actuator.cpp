#include "task_actuator.h"
#include "task_signal.h"
#include "config.h"

uint8_t actuator_get_state()
{
  return ActuatorData.actuator_state;
}

void task_actuator_init()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void task_actuator_run()
{
  static uint32_t s_last_run = 0;
  if ((uint32_t)(millis() - s_last_run) < TASK_SIGNAL_PERIOD_MS) return;
  s_last_run = millis();

  uint8_t cmd = ActuatorData.conditioned_cmd;

  // ── Drive relay ───────────────────────────────────────────────────────
  digitalWrite(RELAY_PIN, cmd ? HIGH : LOW);

  // ── Update shared state ───────────────────────────────────────────────
  uint32_t now       = (uint32_t)millis();
  uint8_t  new_state = cmd;

  if (new_state != ActuatorData.actuator_state)
    ActuatorData.state_changed_at = now;

  ActuatorData.actuator_state = new_state;

  // Alert: relay ON continuously beyond the configured limit
  ActuatorData.alert = new_state &&
                       (now - ActuatorData.state_changed_at >= ALERT_ON_DURATION_MS);
}
