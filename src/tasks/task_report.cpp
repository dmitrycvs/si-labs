#include "task_report.h"
#include "config.h"
#include "task_measure.h"
#include "task_stats.h"
#include "LCDController.h"

// Static handle
static SemaphoreHandle_t s_xDataMutex = nullptr;

// LCD instance (I2C address 0x27, 16x2)
static LCDController *lcd = nullptr;

// Task function
static void prvTaskReport(void *pvParameters)
{
  lcd = new LCDController(0x27, 16, 2);
  lcd->setup();

  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_REPORT_PERIOD_MS));

    // Snapshot all shared data atomically
    float   fTemp      = 0.0f;
    bool    xValid     = false;
    bool    xAlert     = false;
    bool    xRawCond   = false;
    uint8_t ucDebounce = 0;

    if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
      fTemp      = SensorData.fRawTemperature;
      xValid     = SensorData.xSensorValid;
      xAlert     = Alert.xAlertActive;
      xRawCond   = Alert.xRawCondition;
      ucDebounce = Alert.ucDebounceCount;
      xSemaphoreGive(s_xDataMutex);
    }

    // --- Serial / STDIO report ---
    printf("=== Temperature Report ===\n");
    if (xValid)
    {
      printf("Raw temp  : %.2f C\n", fTemp);
      printf("Threshold : H=%.1f C  L=%.1f C\n", THRESHOLD_HIGH_C, THRESHOLD_LOW_C);
      printf("Raw cond  : %s threshold (pre-debounce)\n", xRawCond ? "ABOVE" : "BELOW");
      printf("Debounce  : %u / %u samples\n", ucDebounce, DEBOUNCE_COUNT);
      printf("State     : %s\n", xAlert ? "** ALERT **" : "NORMAL");
    }
    else
    {
      printf("Sensor    : DISCONNECTED\n");
    }
    printf("==========================\n\n");

    // --- LCD report (16x2) ---
    lcd->clear();
    if (xValid)
    {
      // Line 0: "T:XX.XXC [AL]" or "T:XX.XXC [OK]"
      char line0[17];
      snprintf(line0, sizeof(line0), "T:%.2fC %s", fTemp, xAlert ? "[AL]" : "[OK]");
      lcd->printAt(0, 0, line0);

      // Line 1: "H:26.0  L:24.0"
      char line1[17];
      snprintf(line1, sizeof(line1), "H:%.1f  L:%.1f", THRESHOLD_HIGH_C, THRESHOLD_LOW_C);
      lcd->printAt(0, 1, line1);
    }
    else
    {
      lcd->printAt(0, 0, "Sensor error");
      lcd->printAt(0, 1, "Reconnect...");
    }
  }
}

// Create function
void vTaskReportCreate(SemaphoreHandle_t xDataMutex)
{
  s_xDataMutex = xDataMutex;

  xTaskCreate(
      prvTaskReport,
      "TaskReport",
      TASK_REPORT_STACK_SIZE,
      nullptr,
      TASK_REPORT_PRIORITY,
      nullptr);
}
