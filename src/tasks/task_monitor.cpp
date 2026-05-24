#include "task_monitor.h"
#include <Arduino.h>

namespace
{
  TaskMonitor::Config g_cfg{2000};
  uint32_t g_lastMs = 0;

  void printFixed(float v)
  {
    if (v < 0.0f)
    {
      Serial.print('-');
      v = -v;
    }
    int whole = (int)v;
    int frac = (int)((v - (float)whole) * 10.0f + 0.5f);
    if (frac >= 10)
    {
      whole++;
      frac = 0;
    }
    Serial.print(whole);
    Serial.print('.');
    Serial.print(frac);
  }
}

namespace TaskMonitor
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_lastMs = millis();

    Serial.println(F("\n--- System Ready ---"));
  }

  void tick(const TaskSensor::State &s, bool fanOn)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastMs) < g_cfg.periodMs)
      return;
    g_lastMs = now;

    // Serial Plotter line
    Serial.print(F(">SetPoint:"));
    Serial.print(s.targetPct);
    Serial.print(F(",Humidity:"));
    printFixed(s.readingValid ? s.humidPct : 0.0f);
    Serial.print(F(",Output:"));
    printFixed(s.readingValid ? s.pidOut : 0.0f);
    Serial.println();

    // Human-readable line
    if (s.readingValid)
    {
      Serial.print(F("Hum="));
      printFixed(s.humidPct);
      Serial.print(F(" % | SP="));
      Serial.print(s.targetPct);
      Serial.print(F(" % | PID="));
      printFixed(s.pidOut);
      Serial.print(F(" % | Fan="));
      printFixed(s.pwmDuty);
      Serial.println(F(" %"));
    }
    else
    {
      Serial.print(F("Hum=n/a | SP="));
      Serial.print(s.targetPct);
      Serial.println(F(" % | PID=0.0 % | Fan=0.0 %"));
    }
  }
}
