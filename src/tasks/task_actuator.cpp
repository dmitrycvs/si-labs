#include "task_actuator.h"
#include "task_signal.h"
#include "config.h"
#include <Servo.h>

static Servo s_servo;

uint8_t actuator_get_position()
{
  return ServoData.actual_position;
}

void task_actuator_init()
{
  s_servo.attach(SERVO_PIN);
  s_servo.write(90);  // start at centre
  ServoData.actual_position = 90;
}

void task_actuator_run()
{
  static uint32_t s_last_run = 0;
  if ((uint32_t)(millis() - s_last_run) < TASK_SIGNAL_PERIOD_MS) return;
  s_last_run = millis();

  uint8_t pos = ServoData.conditioned_target;

  // Drive servo to conditioned position
  s_servo.write(pos);
  ServoData.actual_position = pos;

  // Alert: position at or near physical limits
  ServoData.alert_at_min = (pos <= ALERT_NEAR_MIN);
  ServoData.alert_at_max = (pos >= ALERT_NEAR_MAX);
}
