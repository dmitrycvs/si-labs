#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Raw sensor data shared between tasks (protected by xDataMutex)
typedef struct
{
  uint16_t usRawAdc;          // raw 12-bit ADC value (0-4095)
  float    fRawTemperature;   // temperature converted via Beta equation (°C)
  bool     xSensorValid;      // false when ADC is railed (0 or 4095)
} SensorReading_t;

extern SensorReading_t SensorData;

// Thread-safe getter — returns latest raw temperature in °C
float sensor_read();

// Create the sensor acquisition task
void vTaskMeasureCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex);

#endif // TASK_MEASURE_H
