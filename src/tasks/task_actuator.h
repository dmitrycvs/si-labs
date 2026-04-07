#ifndef TASK_ACTUATOR_H
#define TASK_ACTUATOR_H

#include <Arduino.h>

// Returns current servo position in degrees (0-180)
uint8_t actuator_get_position();

// Call once in setup()
void task_actuator_init();

// Call every loop iteration — drives servo at TASK_SIGNAL_PERIOD_MS
void task_actuator_run();

#endif // TASK_ACTUATOR_H
