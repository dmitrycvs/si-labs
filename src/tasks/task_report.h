#ifndef TASK_REPORT_H
#define TASK_REPORT_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Create the display & reporting task
void vTaskReportCreate(SemaphoreHandle_t xDataMutex);

#endif // TASK_REPORT_H
