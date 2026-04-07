#ifndef TASK_ACTUATOR_H
#define TASK_ACTUATOR_H

#include <Arduino.h>

// Returns current relay state: 0=OFF, 1=ON
uint8_t actuator_get_state();

// Call once in setup()
void task_actuator_init();

// Call every loop iteration — runs at TASK_SIGNAL_PERIOD_MS (derived recurrence)
void task_actuator_run();

#endif // TASK_ACTUATOR_H
