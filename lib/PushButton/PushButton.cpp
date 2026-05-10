#include "PushButton.h"

namespace {
  bool readRaw(uint8_t pin, bool activeLow) {
    return activeLow ? (digitalRead(pin) == LOW) : (digitalRead(pin) == HIGH);
  }
}

void PushButton::setup(const Config &cfg) {
  m_cfg        = cfg;
  pinMode(m_cfg.pin, m_cfg.activeLow ? INPUT_PULLUP : INPUT);
  m_prevRaw    = readRaw(m_cfg.pin, m_cfg.activeLow);
  m_stableState = m_prevRaw;
  m_changeMs   = millis();
}

bool PushButton::pollRisingEdge() {
  const uint32_t now = millis();
  const bool     raw = readRaw(m_cfg.pin, m_cfg.activeLow);

  if (raw != m_prevRaw) {
    m_prevRaw  = raw;
    m_changeMs = now;
  }

  if ((uint32_t)(now - m_changeMs) < m_cfg.debounceMs) return false;

  if (m_stableState != m_prevRaw) {
    m_stableState = m_prevRaw;
    return m_stableState;
  }
  return false;
}
