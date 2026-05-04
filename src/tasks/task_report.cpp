#include "task_report.h"
#include "task_measure.h"
#include "config.h"

void task_report_init() {}

void task_report_run()
{
  static uint32_t s_last_plotter = 0;
  static uint32_t s_last_human   = 0;
  uint32_t now = millis();

  // ── Arduino Serial Plotter line ───────────────────────────────────────────
  // Relay is scaled ×10 so it is visible alongside degree values.
  if ((uint32_t)(now - s_last_plotter) >= TASK_REPORT_PERIOD_MS)
  {
    s_last_plotter = now;
    float t = ControlData.sensor_ok ? ControlData.temperature : ControlData.setpoint;
    Serial.print(F("SetPoint:"));     Serial.print(ControlData.setpoint, 1);
    Serial.print(F(",Temperature:")); Serial.print(t, 1);
    Serial.print(F(",Humidity:"));    Serial.print(ControlData.sensor_ok ? ControlData.humidity : 0.0f, 1);
    Serial.print(F(",Relay:"));       Serial.println(ControlData.relay_state ? 10 : 0);
  }

  // ── Human-readable status block ───────────────────────────────────────────
  if ((uint32_t)(now - s_last_human) >= TASK_REPORT_HUMAN_MS)
  {
    s_last_human = now;

    Serial.println(F("=== Hysteresis Control ==="));
    Serial.print(F("SetPoint    : ")); Serial.print(ControlData.setpoint, 1);    Serial.println(F(" C"));
    Serial.print(F("Hysteresis  : ±")); Serial.print(ControlData.hysteresis, 1); Serial.println(F(" C"));

    if (ControlData.sensor_ok)
    {
      Serial.print(F("Temperature : ")); Serial.print(ControlData.temperature, 1); Serial.println(F(" C"));
      Serial.print(F("Humidity    : ")); Serial.print(ControlData.humidity, 1);    Serial.println(F(" %"));
    }
    else
    {
      Serial.println(F("Temperature : SENSOR ERROR"));
      Serial.println(F("Humidity    : SENSOR ERROR"));
    }

    Serial.print(F("Relay       : ")); Serial.println(ControlData.relay_state ? F("ON") : F("OFF"));
    Serial.println(F("==========================\n"));
  }
}
