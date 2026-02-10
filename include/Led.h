#ifndef LED_H
#define LED_H

#include <Arduino.h>

enum LedState
{
    LED_OFF = LOW,
    LED_ON = HIGH
};

void LedInit(uint8_t pin);
void LedSetState(uint8_t pin, LedState state);
void LedOn(uint8_t pin);
void LedOff(uint8_t pin);
void LedToggle(uint8_t pin);

#endif
