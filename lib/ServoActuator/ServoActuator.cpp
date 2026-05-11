#include "ServoActuator.h"

#include <Servo.h>

namespace {
  Servo g_servo;
  bool  g_attached = false;
}

void ServoActuator::setup(const Config &cfg) {
  m_pin = cfg.pin;
  if (!g_attached) {
    g_servo.attach(m_pin);
    g_attached = true;
  }
  m_pos = 0;
  g_servo.write(m_pos);
}

void ServoActuator::writeDeg(int deg) {
  if (deg < POS_MIN) deg = POS_MIN;
  if (deg > 180)     deg = 180;
  m_pos = deg;
  g_servo.write(m_pos);
}

void ServoActuator::enable() {
  if (!m_running) {
    m_running     = true;
    m_dir         = 1;
    m_lastStepMs  = millis();
    writeDeg(POS_MIN);
  }
}

void ServoActuator::disable() {
  m_running = false;
  m_dir     = 1;
  writeDeg(POS_MIN);
}

void ServoActuator::update() {
  if (!m_running) return;

  const uint32_t now = millis();
  if ((uint32_t)(now - m_lastStepMs) < STEP_MS) return;
  m_lastStepMs = now;

  int next = m_pos + m_dir * STEP_DEG;
  if (next >= POS_MAX) { next = POS_MAX; m_dir = -1; }
  else if (next <= POS_MIN) { next = POS_MIN; m_dir =  1; }

  writeDeg(next);
}

int ServoActuator::getPos() const {
  return m_pos;
}
