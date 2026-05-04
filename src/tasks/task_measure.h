#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#include <Arduino.h>

// Shared data exchanged between all tasks (single-threaded, no mutex needed)
typedef struct
{
  float temperature;  // last measured temperature (°C); 0 if sensor_ok = false
  float humidity;     // last measured relative humidity (%); 0 if sensor_ok = false
  float setpoint;     // configurable setpoint (°C), adjusted by buttons or serial
  float hysteresis;   // hysteresis half-band (°C)
  bool  relay_state;  // true = relay energised
  bool  sensor_ok;    // true = last DHT22 read was valid
} ControlData_t;

extern ControlData_t ControlData;

void task_measure_init();
void task_measure_run();

#endif // TASK_MEASURE_H
