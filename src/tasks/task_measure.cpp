#include "task_measure.h"
#include "config.h"
#include <math.h>

// Shared sensor data (protected by xDataMutex)
SensorReading_t SensorData = {0, 0.0f, false};

// Static handles
static SemaphoreHandle_t s_xNewDataSemaphore = nullptr;
static SemaphoreHandle_t s_xDataMutex        = nullptr;

// Convert raw ADC value to temperature (°C) using the Beta equation
static float prvAdcToTemperature(uint16_t usAdc)
{
  // Voltage divider: Vout = Vcc * R_ntc / (R_series + R_ntc)
  // => R_ntc = R_series * ADC / (ADC_MAX - ADC)
  float fRntc = NTC_R_SERIES * (float)usAdc / (ADC_MAX - (float)usAdc);

  // Beta equation: T = 1 / ( 1/T0 + (1/B)*ln(R/R0) )
  float fTkelvin = 1.0f / (1.0f / NTC_T0_K + (1.0f / NTC_BETA) * logf(fRntc / NTC_R0));

  return fTkelvin - 273.15f;
}

// Thread-safe getter
float sensor_read()
{
  float fTemp = 0.0f;
  if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    fTemp = SensorData.fRawTemperature;
    xSemaphoreGive(s_xDataMutex);
  }
  return fTemp;
}

// Task function
static void prvTaskMeasure(void *pvParameters)
{
  // ADC is configured in setup() before task creation

  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    uint16_t usAdc = (uint16_t)analogRead(NTC_SENSOR_PIN);

    // ADC railed at 0 or 4095 means the sensor is likely disconnected / shorted
    bool xValid = (usAdc > 0) && (usAdc < (uint16_t)ADC_MAX);

    float fTemp = 0.0f;
    if (xValid)
    {
      fTemp = prvAdcToTemperature(usAdc);
    }

    // Update shared data
    if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      SensorData.usRawAdc        = usAdc;
      SensorData.fRawTemperature = fTemp;
      SensorData.xSensorValid    = xValid;
      xSemaphoreGive(s_xDataMutex);
    }

    // Signal the conditioning task that new data is ready
    xSemaphoreGive(s_xNewDataSemaphore);

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_MEASURE_PERIOD_MS));
  }
}

// Create function
void vTaskMeasureCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex)
{
  s_xNewDataSemaphore = xNewDataSemaphore;
  s_xDataMutex        = xDataMutex;

  xTaskCreate(
      prvTaskMeasure,
      "TaskMeasure",
      TASK_MEASURE_STACK_SIZE,
      nullptr,
      TASK_MEASURE_PRIORITY,
      nullptr);
}
