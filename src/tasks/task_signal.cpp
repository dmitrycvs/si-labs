#include "task_signal.h"
#include "config.h"

ServoData_t ServoData = {90, 90, 90, false, false};

// ── Median filter state ───────────────────────────────────────────────────────
static uint8_t s_median_buf[MEDIAN_WINDOW];
static uint8_t s_median_idx   = 0;
static uint8_t s_median_count = 0;

// ── EMA state ────────────────────────────────────────────────────────────────
static float s_ema = 90.0f;

// ── Ramp state ───────────────────────────────────────────────────────────────
static float s_ramp_pos = 90.0f;  // current ramped position (fractional)

// Insert new value into circular buffer and return the median.
static uint8_t prvMedianFilter(uint8_t new_val)
{
  s_median_buf[s_median_idx] = new_val;
  s_median_idx = (s_median_idx + 1) % MEDIAN_WINDOW;
  if (s_median_count < MEDIAN_WINDOW) s_median_count++;

  // Copy valid portion and sort (bubble sort — tiny array)
  uint8_t sorted[MEDIAN_WINDOW];
  for (uint8_t i = 0; i < s_median_count; i++)
    sorted[i] = s_median_buf[i];

  for (uint8_t i = 0; i < s_median_count - 1; i++)
    for (uint8_t j = 0; j < s_median_count - i - 1; j++)
      if (sorted[j] > sorted[j + 1]) {
        uint8_t tmp  = sorted[j];
        sorted[j]    = sorted[j + 1];
        sorted[j + 1] = tmp;
      }

  return sorted[s_median_count / 2];
}

// Parse one serial line.
// Accepts a plain integer (0-180).
// Returns the parsed value in [0,180], or -1 if nothing / invalid.
static int16_t prvReadSerialCommand()
{
  if (!Serial.available()) return -1;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return -1;

  // Check all characters are digits (optional leading sign not accepted)
  for (uint8_t i = 0; i < line.length(); i++) {
    if (!isDigit(line[i])) {
      Serial.println(F("[WARN] Invalid command - send a number 0-180"));
      return -1;
    }
  }

  int16_t val = (int16_t)line.toInt();

  // Saturation: clamp to physical limits [SERVO_MIN, SERVO_MAX]
  if (val < SERVO_MIN) { val = SERVO_MIN; Serial.println(F("[INFO] Saturated to 0")); }
  if (val > SERVO_MAX) { val = SERVO_MAX; Serial.println(F("[INFO] Saturated to 180")); }

  return val;
}

void task_signal_init()
{
  // Pre-fill median buffer with the initial position (90 deg = centre)
  for (uint8_t i = 0; i < MEDIAN_WINDOW; i++)
    s_median_buf[i] = 90;
  s_median_count = MEDIAN_WINDOW;
}

void task_signal_run()
{
  static uint32_t s_last_run    = 0;
  static uint8_t  s_last_target = 90;  // last accepted target (held between cycles)

  if ((uint32_t)(millis() - s_last_run) < TASK_SIGNAL_PERIOD_MS) return;
  s_last_run = millis();

  // ── 1. Read and saturate input ────────────────────────────────────────────
  int16_t cmd = prvReadSerialCommand();
  if (cmd >= 0)
    s_last_target = (uint8_t)cmd;

  ServoData.raw_target = s_last_target;

  // ── 2. Median filter (removes impulse noise / typos) ─────────────────────
  uint8_t median_val = prvMedianFilter(s_last_target);

  // ── 3. Exponential Moving Average (smooths residual fluctuations) ─────────
  s_ema = EMA_ALPHA * (float)median_val + (1.0f - EMA_ALPHA) * s_ema;

  // ── 4. Ramp (soft start/stop — protects servo from abrupt changes) ────────
  float target_f = s_ema;
  if (s_ramp_pos < target_f)
    s_ramp_pos = fminf(s_ramp_pos + RAMP_STEP, target_f);
  else if (s_ramp_pos > target_f)
    s_ramp_pos = fmaxf(s_ramp_pos - RAMP_STEP, target_f);

  ServoData.conditioned_target = (uint8_t)(s_ramp_pos + 0.5f);  // round
}
