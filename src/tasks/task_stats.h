#ifndef TASK_STATS_H
#define TASK_STATS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Alert state shared between tasks (protected by xDataMutex)
typedef struct
{
  bool    xAlertActive;    // confirmed alert state (after debounce)
  bool    xRawCondition;   // raw threshold condition before debounce
  uint8_t ucDebounceCount; // current consecutive-sample counter
} AlertState_t;

extern AlertState_t Alert;

// Create the threshold alerting task
void vTaskStatsCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex);

#endif // TASK_STATS_H
