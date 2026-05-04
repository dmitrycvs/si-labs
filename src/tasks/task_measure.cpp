#include "task_measure.h"
#include "config.h"
#include <DHT.h>

static DHT s_dht(DHT_PIN, DHT22);

ControlData_t ControlData = {0.0f, 0.0f, DEFAULT_SETPOINT, DEFAULT_HYSTERESIS, false, false};

// ── Button handler ────────────────────────────────────────────────────────────
// Polled every loop(); debounced by BTN_DEBOUNCE_MS.
// Detects falling edges so a held button fires only once per press.
static void prvReadButtons()
{
  static bool     s_up_prev  = false;
  static bool     s_dn_prev  = false;
  static uint32_t s_last     = 0;

  if ((uint32_t)(millis() - s_last) < BTN_DEBOUNCE_MS) return;
  s_last = millis();

  bool up   = (digitalRead(BTN_UP_PIN)   == LOW);
  bool down = (digitalRead(BTN_DOWN_PIN) == LOW);

  if (up && !s_up_prev)
  {
    ControlData.setpoint += SETPOINT_STEP;
    Serial.print(F("[BTN] SetPoint = ")); Serial.print(ControlData.setpoint, 1); Serial.println(F(" C"));
  }
  if (down && !s_dn_prev)
  {
    ControlData.setpoint -= SETPOINT_STEP;
    Serial.print(F("[BTN] SetPoint = ")); Serial.print(ControlData.setpoint, 1); Serial.println(F(" C"));
  }

  s_up_prev = up;
  s_dn_prev = down;
}

// ── Serial command parser ─────────────────────────────────────────────────────
// Accepted formats (case-sensitive, newline-terminated):
//   SET:<value>   e.g. "SET:26.5"  → update setpoint
//   HYS:<value>   e.g. "HYS:1.5"  → update hysteresis band
static void prvParseSerialCommands()
{
  while (Serial.available())
  {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    if (line.startsWith(F("SET:")))
    {
      float v = line.substring(4).toFloat();
      ControlData.setpoint = v;
      Serial.print(F("[INFO] SetPoint = ")); Serial.print(v, 1); Serial.println(F(" C"));
    }
    else if (line.startsWith(F("HYS:")))
    {
      float v = line.substring(4).toFloat();
      if (v > 0.0f)
      {
        ControlData.hysteresis = v;
        Serial.print(F("[INFO] Hysteresis = ±")); Serial.print(v, 1); Serial.println(F(" C"));
      }
      else
      {
        Serial.println(F("[WARN] Hysteresis must be > 0"));
      }
    }
    else
    {
      Serial.println(F("[WARN] Unknown command. Use SET:<val> or HYS:<val>"));
    }
  }
}

void task_measure_init()
{
  pinMode(BTN_UP_PIN,   INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  s_dht.begin();
}

void task_measure_run()
{
  prvReadButtons();  // fast poll every loop(), debounced internally

  static uint32_t s_last_run = 0;
  if ((uint32_t)(millis() - s_last_run) < TASK_MEASURE_PERIOD_MS) return;
  s_last_run = millis();

  float t = s_dht.readTemperature();
  float h = s_dht.readHumidity();

  if (isnan(t) || isnan(h))
  {
    ControlData.sensor_ok   = false;
    ControlData.temperature = 0.0f;
    ControlData.humidity    = 0.0f;
    Serial.println(F("[ERROR] DHT22 read failed"));
  }
  else
  {
    ControlData.sensor_ok   = true;
    ControlData.temperature = t;
    ControlData.humidity    = h;
  }

  prvParseSerialCommands();
}
