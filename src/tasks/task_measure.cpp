#include "task_measure.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Shared sensor data (protected by xDataMutex)
SensorReading_t SensorData = {0.0f, false};

// Static handles
static SemaphoreHandle_t s_xNewDataSemaphore = nullptr;
static SemaphoreHandle_t s_xDataMutex        = nullptr;

// Sensor driver objects
static OneWire*          s_pOneWire  = nullptr;
static DallasTemperature* s_pSensors = nullptr;

// Returns the latest raw temperature (thread-safe)
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
  s_pOneWire = new OneWire(TEMP_SENSOR_PIN);
  s_pSensors = new DallasTemperature(s_pOneWire);
  s_pSensors->begin();
  s_pSensors->setResolution(9);          // 9-bit: ~94 ms conversion time
  s_pSensors->setWaitForConversion(true); // synchronous: block until conversion done

  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Request conversion and block until it completes (~94 ms), then read
    s_pSensors->requestTemperatures();
    float fTemp  = s_pSensors->getTempCByIndex(0);
    bool  xValid = (fTemp != DEVICE_DISCONNECTED_C);

    // Update shared sensor data
    if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      if (xValid)
      {
        SensorData.fRawTemperature = fTemp;
      }
      SensorData.xSensorValid = xValid;
      xSemaphoreGive(s_xDataMutex);
    }

    // Signal that new data is available for the threshold alerting task
    xSemaphoreGive(s_xNewDataSemaphore);
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
