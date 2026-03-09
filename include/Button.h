#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// Debounce time in milliseconds
const uint16_t BUTTON_DEBOUNCE_MS = 50;

void ButtonInit(uint8_t pin);

// Returns true once per physical press (edge detection + debounce)
bool ButtonPressed(uint8_t pin);

#endif
