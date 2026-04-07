#ifndef TASK_SIGNAL_H
#define TASK_SIGNAL_H

#include <Arduino.h>

// Shared servo data — written by task_signal / task_actuator, read by task_report.
typedef struct
{
  uint8_t raw_target;          // raw user input after saturation (0-180 deg)
  uint8_t conditioned_target;  // after median filter + EMA + ramping (0-180 deg)
  uint8_t actual_position;     // position last written to servo (0-180 deg)
  bool    alert_at_min;        // position <= ALERT_NEAR_MIN
  bool    alert_at_max;        // position >= ALERT_NEAR_MAX
} ServoData_t;

extern ServoData_t ServoData;

// Call once in setup()
void task_signal_init();

// Call every loop iteration — internally rate-limits to TASK_SIGNAL_PERIOD_MS
void task_signal_run();

#endif // TASK_SIGNAL_H
