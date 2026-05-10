#include "task_sensor.h"

#include "PushButton.h"
#include "DhtSensor.h"

namespace
{
  TaskSensor::Config g_cfg{3, 9, 8, true, 50, 50, 20, 80, 1, 3.0f, 0.05f, 0.5f, 0.0f, 100.0f, 50, 2000, 2000};
  TaskSensor::State  g_state{};

  PushButton g_btnUp;
  PushButton g_btnDown;
  DhtSensor  g_dht;

  uint32_t g_lastBtnMs     = 0;
  uint32_t g_lastMeasureMs = 0;
  uint32_t g_lastPidMs     = 0;

  float g_iAccum   = 0.0f;
  float g_prevErr  = 0.0f;
  bool  g_hasPrev  = false;

  int clampI(int v, int lo, int hi)
  {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }

  float clampF(float v, float lo, float hi)
  {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }
}

namespace TaskSensor
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;

    g_btnUp.setup(PushButton::Config{
        .pin        = g_cfg.upPin,
        .activeLow  = g_cfg.activeLow,
        .debounceMs = g_cfg.debounceMs,
    });
    g_btnDown.setup(PushButton::Config{
        .pin        = g_cfg.downPin,
        .activeLow  = g_cfg.activeLow,
        .debounceMs = g_cfg.debounceMs,
    });
    g_dht.setup(DhtSensor::Config{.dataPin = g_cfg.dhtPin});

    g_state              = {};
    g_state.humidPct     = 0.0f;
    g_state.readingValid = false;
    g_state.targetPct    = clampI(g_cfg.targetInit, g_cfg.targetMin, g_cfg.targetMax);
    g_state.pidOut       = 0.0f;
    g_state.pwmDuty      = 0.0f;
    g_state.fanActive    = false;

    g_iAccum  = 0.0f;
    g_prevErr = 0.0f;
    g_hasPrev = false;

    const uint32_t now = millis();
    g_lastBtnMs     = now - g_cfg.btnPeriodMs;
    g_lastMeasureMs = now - g_cfg.measurePeriodMs;
    g_lastPidMs     = now - g_cfg.pidPeriodMs;
  }

  void tick()
  {
    const uint32_t now = millis();

    if ((uint32_t)(now - g_lastBtnMs) >= g_cfg.btnPeriodMs)
    {
      g_lastBtnMs = now;

      bool changed = false;
      if (g_btnUp.pollRisingEdge())
      {
        g_state.targetPct += g_cfg.targetStep;
        changed = true;
      }
      if (g_btnDown.pollRisingEdge())
      {
        g_state.targetPct -= g_cfg.targetStep;
        changed = true;
      }
      if (changed)
      {
        g_state.targetPct = clampI(g_state.targetPct, g_cfg.targetMin, g_cfg.targetMax);
        g_iAccum  = 0.0f;
        g_prevErr = 0.0f;
        g_hasPrev = false;
        g_state.pidOut  = 0.0f;
        g_state.pwmDuty = 0.0f;
      }
    }

    if ((uint32_t)(now - g_lastMeasureMs) >= g_cfg.measurePeriodMs)
    {
      g_lastMeasureMs = now;

      float h = 0.0f;
      if (g_dht.readHumidity(h))
      {
        g_state.humidPct     = h;
        g_state.readingValid = true;
      }
      else
      {
        g_state.readingValid = false;
      }
    }

    if ((uint32_t)(now - g_lastPidMs) >= g_cfg.pidPeriodMs)
    {
      const uint32_t dtMs = (uint32_t)(now - g_lastPidMs);
      g_lastPidMs = now;

      if (!g_state.readingValid)
      {
        g_state.pidOut  = 0.0f;
        g_state.pwmDuty = 0.0f;
        g_iAccum  = 0.0f;
        g_prevErr = 0.0f;
        g_hasPrev = false;
      }
      else
      {
        const float dtSec = (dtMs > 0) ? (dtMs / 1000.0f) : 0.001f;

        // error > 0 means too humid → fan should run (direct action)
        const float err = g_state.humidPct - (float)g_state.targetPct;

        g_iAccum += g_cfg.ki * err * dtSec;
        g_iAccum = clampF(g_iAccum, g_cfg.outMin, g_cfg.outMax);

        float deriv = 0.0f;
        if (g_hasPrev)
          deriv = (err - g_prevErr) / dtSec;

        float output = (g_cfg.kp * err) + g_iAccum + (g_cfg.kd * deriv);
        output = clampF(output, g_cfg.outMin, g_cfg.outMax);

        g_prevErr = err;
        g_hasPrev = true;

        g_state.pidOut  = output;
        g_state.pwmDuty = clampF(output, 0.0f, 100.0f);
      }
    }

    g_state.fanActive = g_state.readingValid && (g_state.pwmDuty > 0.0f);
  }

  State getState()
  {
    return g_state;
  }
}
