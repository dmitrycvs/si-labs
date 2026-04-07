#include <Arduino.h>
#include "tasks/task_signal.h"
#include "tasks/task_actuator.h"
#include "tasks/task_report.h"

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Analog Actuator Control - Servo"));
  Serial.println(F("Send a position in degrees (0-180)"));

  // Task 1 - Signal Conditioning: saturation, median filter, EMA, ramping (50 ms)
  task_signal_init();

  // Task 2 - Actuator Control: drives servo from conditioned position (50 ms)
  task_actuator_init();

  // Task 3 - Display & Reporting: serial report every 500 ms
  task_report_init();
}

void loop()
{
  task_signal_run();
  task_actuator_run();
  task_report_run();
}
