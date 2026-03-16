#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Raw sensor data shared between tasks (protected by xDataMutex)
typedef struct
{
  float fRawTemperature; // latest raw reading from DS18B20 in °C
  bool  xSensorValid;    // true if sensor is connected and reading is valid
} SensorReading_t;

extern SensorReading_t SensorData;

// Returns the latest raw temperature value (thread-safe getter)
float sensor_read();

// Create the sensor acquisition task
void vTaskMeasureCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex);

#endif // TASK_MEASURE_H
