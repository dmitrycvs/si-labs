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

  uint8_t raw       = ServoData.raw_target;
  uint8_t cond      = ServoData.conditioned_target;
  uint8_t actual    = ServoData.actual_position;
  bool    at_min    = ServoData.alert_at_min;
  bool    at_max    = ServoData.alert_at_max;

  // ── Structured serial report ──────────────────────────────────────────────
  Serial.println(F("=== Servo Report ==="));
  Serial.print(F("Raw target  : ")); Serial.print(raw);    Serial.println(F(" deg"));
  Serial.print(F("Conditioned : ")); Serial.print(cond);   Serial.println(F(" deg"));
  Serial.print(F("Position    : ")); Serial.print(actual); Serial.println(F(" deg"));

  if (at_min)
    Serial.println(F("ALERT       : Limit reached - MIN position"));
  else if (at_max)
    Serial.println(F("ALERT       : Limit reached - MAX position"));
  else
    Serial.println(F("Status      : OK"));

  Serial.println(F("====================\n"));
}
