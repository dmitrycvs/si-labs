#include "task_signal.h"
#include "config.h"

ActuatorData_t ActuatorData = {0, 0, 0, false};

// Read one line from Serial.
//   Returns 1 (ON), 0 (OFF), 2 (TOGGLE), or -1 (none / invalid).
// Saturation: any command outside the known set is rejected with a warning.
static int8_t prvReadSerialCommand()
{
  if (!Serial.available()) return -1;

  String line = Serial.readStringUntil('\n');
  line.trim();

  if (line.equalsIgnoreCase("ON"))     return 1;
  if (line.equalsIgnoreCase("OFF"))    return 0;
  if (line.equalsIgnoreCase("TOGGLE")) return 2;

  Serial.println(F("[WARN] Unknown command - use ON, OFF or TOGGLE"));
  return -1;
}

void task_signal_init()
{
  // No hardware pins to configure — input is Serial only
}

void task_signal_run()
{
  static uint32_t s_last_run = 0;

  // Software debounce / persistent validation counters.
  // A command must arrive consistently for DEBOUNCE_SAMPLES consecutive
  // cycles before it is accepted, eliminating spurious or noisy inputs.
  static int8_t  s_pending_cmd   = -1;  // command currently being validated
  static uint8_t s_stable_count  =  0;  // consecutive matching samples

  static uint8_t s_desired = 0;

  if ((uint32_t)(millis() - s_last_run) < TASK_SIGNAL_PERIOD_MS) return;
  s_last_run = millis();

  int8_t cmd = prvReadSerialCommand();

  if (cmd == 2)
  {
    // TOGGLE is applied immediately — no debounce needed for an explicit one-shot command
    s_desired      ^= 1u;
    s_pending_cmd   = -1;
    s_stable_count  =  0;
  }
  else if (cmd == -1)
  {
    // No input this cycle — reset validation window
    s_pending_cmd  = -1;
    s_stable_count =  0;
  }
  else
  {
    // Saturation: clamp to valid range [INPUT_SAT_MIN, INPUT_SAT_MAX]
    if (cmd < INPUT_SAT_MIN) cmd = INPUT_SAT_MIN;
    if (cmd > INPUT_SAT_MAX) cmd = INPUT_SAT_MAX;

    if (cmd == s_pending_cmd)
    {
      s_stable_count++;
    }
    else
    {
      // New candidate — restart debounce counter
      s_pending_cmd  = cmd;
      s_stable_count = 1;
    }

    // Persistent state validation: accept only after DEBOUNCE_SAMPLES stable cycles
    if (s_stable_count >= DEBOUNCE_SAMPLES)
    {
      s_desired      = (uint8_t)s_pending_cmd;
      s_stable_count = DEBOUNCE_SAMPLES;  // cap to prevent overflow
    }
  }

  ActuatorData.conditioned_cmd = s_desired;
}
