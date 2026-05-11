#include "task_acquisition.h"

#include "PushButton.h"
#include "DhtSensor.h"

namespace
{
  TaskAcquisition::Config g_cfg{3, 9, 8, true, 50, 55, 30, 90, 1, 3.0f, 50, 2000, 1000};
  TaskAcquisition::State  g_state{};

  PushButton g_btnUp;
  PushButton g_btnDown;
  DhtSensor  g_dht;

  uint32_t g_lastBtnMs     = 0;
  uint32_t g_lastSampleMs  = 0;
  uint32_t g_lastControlMs = 0;

  int clamp(int v, int lo, int hi)
  {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }

  void refreshBounds()
  {
    g_state.lowerBoundPct = (float)g_state.setpointPct - g_cfg.hysteresisPct;
    g_state.upperBoundPct = (float)g_state.setpointPct + g_cfg.hysteresisPct;
  }
}

namespace TaskAcquisition
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;

    g_btnUp.setup(PushButton::Config{
        .pin        = g_cfg.upButtonPin,
        .activeLow  = g_cfg.buttonsActiveLow,
        .debounceMs = g_cfg.buttonDebounceMs,
    });
    g_btnDown.setup(PushButton::Config{
        .pin        = g_cfg.downButtonPin,
        .activeLow  = g_cfg.buttonsActiveLow,
        .debounceMs = g_cfg.buttonDebounceMs,
    });
    g_dht.setup(DhtSensor::Config{.dataPin = g_cfg.dhtPin});

    g_state               = {};
    g_state.humidityPct   = 0.0f;
    g_state.sensorValid   = false;
    g_state.setpointPct   = clamp(g_cfg.setpointInitPct, g_cfg.setpointMinPct, g_cfg.setpointMaxPct);
    g_state.relayRequestOn = false;
    refreshBounds();

    const uint32_t now = millis();
    g_lastBtnMs     = now - g_cfg.buttonPeriodMs;
    g_lastSampleMs  = now - g_cfg.samplePeriodMs;
    g_lastControlMs = now - g_cfg.controlPeriodMs;
  }

  void tick()
  {
    const uint32_t now = millis();

    if ((uint32_t)(now - g_lastBtnMs) >= g_cfg.buttonPeriodMs)
    {
      g_lastBtnMs = now;

      bool changed = false;
      if (g_btnUp.pollRisingEdge())
      {
        g_state.setpointPct += g_cfg.setpointStepPct;
        changed = true;
      }
      if (g_btnDown.pollRisingEdge())
      {
        g_state.setpointPct -= g_cfg.setpointStepPct;
        changed = true;
      }
      if (changed)
      {
        g_state.setpointPct = clamp(g_state.setpointPct, g_cfg.setpointMinPct, g_cfg.setpointMaxPct);
        refreshBounds();
      }
    }

    if ((uint32_t)(now - g_lastSampleMs) >= g_cfg.samplePeriodMs)
    {
      g_lastSampleMs = now;

      float h = 0.0f;
      if (g_dht.readHumidity(h))
      {
        g_state.humidityPct = h;
        g_state.sensorValid = true;
      }
      else
      {
        g_state.sensorValid = false;
      }
    }

    if ((uint32_t)(now - g_lastControlMs) >= g_cfg.controlPeriodMs)
    {
      g_lastControlMs = now;

      if (!g_state.sensorValid) return;

      // Inverse action (dehumidification): relay ON when humidity rises above upper bound.
      if (!g_state.relayRequestOn && g_state.humidityPct > g_state.upperBoundPct)
      {
        g_state.relayRequestOn = true;
      }
      else if (g_state.relayRequestOn && g_state.humidityPct < g_state.lowerBoundPct)
      {
        g_state.relayRequestOn = false;
      }
    }
  }

  State getState()
  {
    return g_state;
  }
}
