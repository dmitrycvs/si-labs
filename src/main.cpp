#include <Arduino.h>
#include "Console.h"
#include "config.h"
#include "tasks/task_acquisition.h"
#include "tasks/task_output.h"
#include "tasks/task_logger.h"

void setup()
{
  Console::init();

  TaskAcquisition::setup(Config::ACQUISITION_CFG);
  TaskOutput::setup(Config::OUTPUT_CFG);
  TaskLogger::setup(Config::LOGGER_CFG);
}

void loop()
{
  TaskAcquisition::tick();
  const TaskAcquisition::State sensorState = TaskAcquisition::getState();

  TaskOutput::commandState(sensorState.relayRequestOn);
  TaskOutput::tick();

  TaskLogger::tick(sensorState, TaskOutput::isOn());
}
