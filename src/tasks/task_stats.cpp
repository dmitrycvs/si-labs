#include "task_stats.h"
#include "task_measure.h"
#include "config.h"
#include "LedController.h"

// Shared processed data (protected by xDataMutex)
ProcessedData_t Processed = {0.0f, 0.0f, 0.0f, false, false};

// Static handles
static SemaphoreHandle_t s_xNewDataSemaphore = nullptr;
static SemaphoreHandle_t s_xDataMutex        = nullptr;

// LED instance
static LedController *RedLed = nullptr;

// ---- Median filter state ----
static float   s_afMedianBuf[MEDIAN_FILTER_SIZE];
static uint8_t s_ucMedianIdx   = 0;
static uint8_t s_ucMedianCount = 0;

// ---- EMA state ----
static float s_fEmaValue       = 0.0f;
static bool  s_xEmaInitialized = false;

// Saturate (clamp) temperature to the configured valid range
static float prvSaturate(float fTemp)
{
  if (fTemp < TEMP_SAT_MIN_C) return TEMP_SAT_MIN_C;
  if (fTemp > TEMP_SAT_MAX_C) return TEMP_SAT_MAX_C;
  return fTemp;
}

// Insert a new sample into the circular buffer and return the median
static float prvMedianFilter(float fNewValue)
{
  s_afMedianBuf[s_ucMedianIdx] = fNewValue;
  s_ucMedianIdx = (s_ucMedianIdx + 1) % MEDIAN_FILTER_SIZE;
  if (s_ucMedianCount < MEDIAN_FILTER_SIZE)
    s_ucMedianCount++;

  // Copy the valid portion and sort (bubble sort — tiny array)
  float afSorted[MEDIAN_FILTER_SIZE];
  for (uint8_t i = 0; i < s_ucMedianCount; i++)
    afSorted[i] = s_afMedianBuf[i];

  for (uint8_t i = 0; i < s_ucMedianCount - 1; i++)
  {
    for (uint8_t j = 0; j < s_ucMedianCount - i - 1; j++)
    {
      if (afSorted[j] > afSorted[j + 1])
      {
        float fTmp      = afSorted[j];
        afSorted[j]     = afSorted[j + 1];
        afSorted[j + 1] = fTmp;
      }
    }
  }

  return afSorted[s_ucMedianCount / 2];
}

// Exponential moving average
static float prvEma(float fNewValue)
{
  if (!s_xEmaInitialized)
  {
    s_fEmaValue       = fNewValue;
    s_xEmaInitialized = true;
  }
  else
  {
    s_fEmaValue = EMA_ALPHA * fNewValue + (1.0f - EMA_ALPHA) * s_fEmaValue;
  }
  return s_fEmaValue;
}

// Task function
static void prvTaskStats(void *pvParameters)
{
  RedLed = new LedController(RED_LED_PIN);
  RedLed->setup();
  RedLed->turnOff();

  for (;;)
  {
    // Block until task_measure signals that a new sample is ready
    if (xSemaphoreTake(s_xNewDataSemaphore, portMAX_DELAY) == pdTRUE)
    {
      if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
      {
        float fRaw   = SensorData.fRawTemperature;
        bool  xValid = SensorData.xSensorValid;

        if (xValid)
        {
          // 1. Saturation — clamp to physical range
          float fSaturated = prvSaturate(fRaw);

          // 2. Median filter — remove salt-and-pepper noise (spikes)
          float fMedian = prvMedianFilter(fSaturated);

          // 3. Exponential moving average — smooth the signal
          float fFiltered = prvEma(fMedian);

          // 4. Alert thresholds
          bool xHigh = (fFiltered >= ALERT_HIGH_C);
          bool xLow  = (fFiltered <= ALERT_LOW_C);

          // Update shared processed data
          Processed.fSaturatedTemp = fSaturated;
          Processed.fMedianTemp    = fMedian;
          Processed.fFilteredTemp  = fFiltered;
          Processed.xAlertHigh     = xHigh;
          Processed.xAlertLow      = xLow;

          // Red LED on if any alert, off otherwise
          if (xHigh || xLow)
            RedLed->turnOn();
          else
            RedLed->turnOff();
        }

        xSemaphoreGive(s_xDataMutex);
      }
    }
  }
}

// Create function
void vTaskStatsCreate(SemaphoreHandle_t xNewDataSemaphore, SemaphoreHandle_t xDataMutex)
{
  s_xNewDataSemaphore = xNewDataSemaphore;
  s_xDataMutex        = xDataMutex;

  xTaskCreate(
      prvTaskStats,
      "TaskStats",
      TASK_STATS_STACK_SIZE,
      nullptr,
      TASK_STATS_PRIORITY,
      nullptr);
}
