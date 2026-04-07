#include <Arduino.h>
#include "tasks/task_signal.h"
#include "tasks/task_actuator.h"
#include "tasks/task_report.h"

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Binary Actuator Control"));
  Serial.println(F("Commands: ON | OFF | TOGGLE"));

  // Task 1 — Signal Conditioning: reads Serial/button, debouncing (50 ms)
  task_signal_init();

  // Task 2 — Actuator Control: drives LEDs from conditioned command (50 ms)
  task_actuator_init();

  // Task 3 — Display & Reporting: serial report every 500 ms
  task_report_init();
}

void loop()
{
  task_signal_run();
  task_actuator_run();
  task_report_run();
}
