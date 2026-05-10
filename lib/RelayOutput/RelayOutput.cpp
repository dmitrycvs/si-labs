#include "RelayOutput.h"

void RelayOutput::setup(const Config &cfg) {
  m_cfg = cfg;
  pinMode(m_cfg.pin, OUTPUT);
  set(false);
}

void RelayOutput::set(bool on) {
  m_on = on;
  const uint8_t level = m_cfg.activeHigh ? (m_on ? HIGH : LOW) : (m_on ? LOW : HIGH);
  digitalWrite(m_cfg.pin, level);
}

bool RelayOutput::isOn() const {
  return m_on;
}
