#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "config.h"
#include "tasks/task_measure.h"
#include "tasks/task_stats.h"
#include "tasks/task_report.h"

// Shared synchronisation primitives
static SemaphoreHandle_t xNewDataSemaphore = nullptr; // signals task_stats when new reading is ready
static SemaphoreHandle_t xDataMutex        = nullptr; // protects SensorData and Processed structs

void setup()
{
  Serial.begin(115200);

  // Configure ADC before any task uses it
  analogReadResolution(12);
  analogSetPinAttenuation(NTC_SENSOR_PIN, ADC_11db);

  xNewDataSemaphore = xSemaphoreCreateBinary();
  xDataMutex        = xSemaphoreCreateMutex();

  // Task 1 – Sensor Acquisition: reads NTC via ADC every TASK_MEASURE_PERIOD_MS
  vTaskMeasureCreate(xNewDataSemaphore, xDataMutex);

  // Task 2 – Signal Conditioning: saturation, median filter, EMA, alerts
  vTaskStatsCreate(xNewDataSemaphore, xDataMutex);

  // Task 3 – Display & Reporting: prints structured report every TASK_REPORT_PERIOD_MS
  vTaskReportCreate(xDataMutex);
}

void loop()
{
}
