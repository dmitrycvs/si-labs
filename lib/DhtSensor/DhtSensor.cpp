#include "DhtSensor.h"

#include <DHT.h>
#include <math.h>

namespace {
  DHT *g_dht = nullptr;
}

void DhtSensor::setup(const Config &cfg) {
  m_dataPin = cfg.dataPin;

  if (g_dht != nullptr) {
    delete g_dht;
    g_dht = nullptr;
  }

  g_dht = new DHT(m_dataPin, DHT22);
  g_dht->begin();
}

bool DhtSensor::readTemperature(float &outDegC) {
  if (g_dht == nullptr) return false;

  const float t = g_dht->readTemperature();
  if (isnan(t)) return false;

  outDegC = t;
  return true;
}
