#ifndef TASK_SIGNAL_H
#define TASK_SIGNAL_H

#include <Arduino.h>

// Shared actuator data — written by task_signal / task_actuator, read by task_report.
typedef struct
{
  uint8_t  conditioned_cmd;    // 0=OFF, 1=ON  (output of signal conditioning)
  uint8_t  actuator_state;     // 0=OFF, 1=ON  (current physical output)
  uint32_t state_changed_at;   // millis() when actuator state last changed
  bool     alert;              // true when relay has been ON > ALERT_ON_DURATION_MS
} ActuatorData_t;

extern ActuatorData_t ActuatorData;

// Call once in setup()
void task_signal_init();

// Call every loop iteration — internally rate-limits to TASK_SIGNAL_PERIOD_MS
void task_signal_run();

#endif // TASK_SIGNAL_H
