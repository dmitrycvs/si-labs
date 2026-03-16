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
    uint16_t usAdc       = 0;
    float    fRaw        = 0.0f;
    bool     xValid      = false;
    float    fSaturated  = 0.0f;
    float    fMedian     = 0.0f;
    float    fFiltered   = 0.0f;
    bool     xAlertHigh  = false;
    bool     xAlertLow   = false;

    if (xSemaphoreTake(s_xDataMutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
      usAdc      = SensorData.usRawAdc;
      fRaw       = SensorData.fRawTemperature;
      xValid     = SensorData.xSensorValid;
      fSaturated = Processed.fSaturatedTemp;
      fMedian    = Processed.fMedianTemp;
      fFiltered  = Processed.fFilteredTemp;
      xAlertHigh = Processed.xAlertHigh;
      xAlertLow  = Processed.xAlertLow;
      xSemaphoreGive(s_xDataMutex);
    }

    // --- Serial / STDIO structured report ---
    printf("=== Temperature Report ===\n");
    if (xValid)
    {
      printf("ADC raw   : %u\n", usAdc);
      printf("Raw temp  : %.2f C\n", fRaw);
      printf("Saturated : %.2f C  [%.0f .. %.0f]\n", fSaturated, TEMP_SAT_MIN_C, TEMP_SAT_MAX_C);
      printf("Median    : %.2f C  (window %d)\n", fMedian, MEDIAN_FILTER_SIZE);
      printf("Filtered  : %.2f C  (EMA a=%.2f)\n", fFiltered, EMA_ALPHA);

      if (xAlertHigh)
        printf("ALERT     : HIGH (>= %.1f C)\n", ALERT_HIGH_C);
      else if (xAlertLow)
        printf("ALERT     : LOW  (<= %.1f C)\n", ALERT_LOW_C);
      else
        printf("State     : NORMAL\n");
    }
    else
    {
      printf("Sensor    : DISCONNECTED (ADC=%u)\n", usAdc);
    }
    printf("==========================\n\n");

    // --- LCD report (16x2) ---
    lcd->clear();
    if (xValid)
    {
      // Line 0: "R:25.3 F:25.1 C"
      char acLine0[17];
      snprintf(acLine0, sizeof(acLine0), "R:%.1f F:%.1f C", fRaw, fFiltered);
      lcd->printAt(0, 0, acLine0);

      // Line 1: "M:25.2  [OK]" or "[HIGH]" / "[LOW]"
      char acLine1[17];
      const char *pcState = (xAlertHigh) ? "[HIGH]" : (xAlertLow) ? "[LOW]" : "[OK]";
      snprintf(acLine1, sizeof(acLine1), "M:%.1f  %s", fMedian, pcState);
      lcd->printAt(0, 1, acLine1);
    }
    else
    {
      lcd->printAt(0, 0, "Sensor error");
      lcd->printAt(0, 1, "ADC disconnected");
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
