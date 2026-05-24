#include <Arduino.h>
#include "config.h"
#include "tasks/task_sensor.h"
#include "tasks/task_driver.h"
#include "tasks/task_monitor.h"

void setup()
{
  Serial.begin(9600);

  TaskSensor::setup(Config::SENSOR_CFG);
  TaskDriver::setup(Config::DRIVER_CFG);
  TaskMonitor::setup(Config::MONITOR_CFG);
}

void loop()
{
  TaskSensor::tick();
  const TaskSensor::State s = TaskSensor::getState();

  TaskDriver::commandDutyPct(s.pwmDuty);
  TaskDriver::tick();

  TaskMonitor::tick(s, TaskDriver::isActive());
}
