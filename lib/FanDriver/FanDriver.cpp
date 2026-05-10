#include "FanDriver.h"

void FanDriver::setup(const Config &cfg) {
  m_cfg    = cfg;
  m_active = false;
  pinMode(m_cfg.pin, OUTPUT);
  setDutyPct(0.0f);
}

void FanDriver::setDutyPct(float pct) {
  if (pct < 0.0f)   pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;

  if (pct <= 0.0f) {
    m_active = false;
    analogWrite(m_cfg.pin, 0);
    return;
  }

  uint8_t raw = (uint8_t)(pct * 2.55f + 0.5f);
  if (raw < m_cfg.minPwm) raw = m_cfg.minPwm;

  m_active = true;
  analogWrite(m_cfg.pin, raw);
}

bool FanDriver::isActive() const {
  return m_active;
}
