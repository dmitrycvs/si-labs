#include <Arduino.h>
#include "tasks/task_measure.h"
#include "tasks/task_control.h"
#include "tasks/task_report.h"

void setup()
{
  Serial.begin(115200);
  Serial.println(F("ON-OFF Hysteresis Temperature Control (DHT22)"));
  Serial.println(F("Buttons: UP/DOWN adjust setpoint by 1 C"));
  Serial.println(F("Commands: SET:<degC>  HYS:<degC>"));

  // Task 1 - Measurement: reads DS18B20 and parses serial commands (1 000 ms)
  task_measure_init();

  // Task 2 - Control: evaluates hysteresis and drives relay (1 000 ms)
  task_control_init();

  // Task 3 - Reporting: Serial Plotter every 500 ms, status block every 2 000 ms
  task_report_init();
}

void loop()
{
  task_measure_run();
  task_control_run();
  task_report_run();
}
