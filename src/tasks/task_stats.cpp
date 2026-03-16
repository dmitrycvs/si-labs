#include "task_stats.h"
#include "task_measure.h"
#include "config.h"
#include "LedController.h"

// Shared alert state (protected by xDataMutex)
AlertState_t Alert = {false, false, 0};

// Static handles
static SemaphoreHandle_t s_xNewDataSemaphore = nullptr;
static SemaphoreHandle_t s_xDataMutex        = nullptr;

// LED instances
static LedController *GreenLed = nullptr;
static LedController *RedLed   = nullptr;

// Task function
static void prvTaskStats(void *pvParameters)
{
  GreenLed = new LedController(GREEN_LED_PIN);
  RedLed   = new LedController(RED_LED_PIN);
  GreenLed->setup();
  RedLed->setup();

  // Initial state: normal (green on)
  GreenLed->turnOn();
  RedLed->turnOff();

  for (;;)
  {
    // Block until task_measure signals that new data is ready
    if (xSemaphoreTake(s_xNewDataSemaphore, portMAX_DELAY) == pdTRUE)
    {
      if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
      {
        float fTemp  = SensorData.fRawTemperature;
        bool  xValid = SensorData.xSensorValid;

        if (xValid)
        {
          // --- Hysteresis threshold ---
          // Use THRESHOLD_HIGH_C to trigger an alert from normal state,
          // use THRESHOLD_LOW_C to clear it (prevents chattering near the boundary).
          bool xCondition;
          if (!Alert.xAlertActive)
          {
            xCondition = (fTemp >= THRESHOLD_HIGH_C); // trigger when crossing high
          }
          else
          {
            xCondition = (fTemp >= THRESHOLD_LOW_C);  // clear only when crossing low
          }

          Alert.xRawCondition = xCondition;

          // --- Debounce / anti-bounce ---
          // The new state must persist for DEBOUNCE_COUNT consecutive samples
          // before the confirmed alert state is updated.
          if (xCondition != Alert.xAlertActive)
          {
            Alert.ucDebounceCount++;
            if (Alert.ucDebounceCount >= DEBOUNCE_COUNT)
            {
              Alert.xAlertActive    = xCondition;
              Alert.ucDebounceCount = 0;

              // Update LEDs to reflect confirmed state change
              if (Alert.xAlertActive)
              {
                GreenLed->turnOff();
                RedLed->turnOn();
              }
              else
              {
                RedLed->turnOff();
                GreenLed->turnOn();
              }
            }
          }
          else
          {
            // Condition matches current state — reset pending counter
            Alert.ucDebounceCount = 0;
          }
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
