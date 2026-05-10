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

bool DhtSensor::readHumidity(float &outPct) {
  if (g_dht == nullptr) return false;

  const float h = g_dht->readHumidity();
  if (isnan(h)) return false;

  outPct = h;
  return true;
}
