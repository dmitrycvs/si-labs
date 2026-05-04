#include "task_control.h"
#include "task_measure.h"
#include "config.h"

void task_control_init()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);   // start with relay de-energised
  ControlData.relay_state = false;
}

void task_control_run()
{
  static uint32_t s_last_run = 0;
  if ((uint32_t)(millis() - s_last_run) < TASK_CONTROL_PERIOD_MS) return;
  s_last_run = millis();

  if (!ControlData.sensor_ok) return;  // never act on a bad reading

  float t   = ControlData.temperature;
  float sp  = ControlData.setpoint;
  float hys = ControlData.hysteresis;

  // Hysteresis ON-OFF logic (heating scenario)
  //   Turn ON  when temperature drops below  (setpoint - hysteresis)
  //   Turn OFF when temperature rises above  (setpoint + hysteresis)
  //   Inside the band: keep current state → prevents rapid chattering
  if (t < sp - hys)
    ControlData.relay_state = true;
  else if (t > sp + hys)
    ControlData.relay_state = false;
  // else: inside hysteresis band — no change

  digitalWrite(RELAY_PIN, ControlData.relay_state ? HIGH : LOW);
}
