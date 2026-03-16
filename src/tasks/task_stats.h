#ifndef TASK_STATS_H
#define TASK_STATS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Processed data shared between tasks (protected by xDataMutex)
typedef struct
{
  float fSaturatedTemp;   // after clamping to [TEMP_SAT_MIN_C, TEMP_SAT_MAX_C]
  float fMedianTemp;      // after salt-and-pepper (median) filter
  float fFilteredTemp;    // after exponential moving average (final output)
  bool  xAlertHigh;       // true when filtered temp >= ALERT_HIGH_C
  bool  xAlertLow;        // true when filtered temp <= ALERT_LOW_C
} ProcessedData_t;

extern ProcessedData_t Processed;

// Create the signal conditioning task
void vTaskStatsCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex);

#endif // TASK_STATS_H
