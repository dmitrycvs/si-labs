#include "task_report.h"
#include "task_signal.h"
#include "config.h"

void task_report_init()
{
  // Nothing to initialise — report goes to Serial, opened in main setup()
}

void task_report_run()
{
  static uint32_t s_last_run = 0;
  if ((uint32_t)(millis() - s_last_run) < TASK_REPORT_PERIOD_MS) return;
  s_last_run = millis();

  uint8_t  state    = ActuatorData.actuator_state;
  uint32_t duration = (uint32_t)millis() - ActuatorData.state_changed_at;
  bool     alert    = ActuatorData.alert;

  // ── Serial structured report ──────────────────────────────────────────
  Serial.println(F("=== Actuator Report ==="));
  Serial.print(F("State    : ")); Serial.println(state ? F("ON") : F("OFF"));
  Serial.print(F("Duration : ")); Serial.print(duration); Serial.println(F(" ms"));
  if (alert)
  {
    Serial.print(F("ALERT    : ON for > "));
    Serial.print(ALERT_ON_DURATION_MS);
    Serial.println(F(" ms !"));
  }
  else
  {
    Serial.println(F("Status   : OK"));
  }
  Serial.println(F("=======================\n"));
}
