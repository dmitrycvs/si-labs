#ifndef KEYPAD_WRAPPER_H
#define KEYPAD_WRAPPER_H

#include <Arduino.h>
#include <Keypad.h>

// Keypad Configuration
const uint8_t KEYPAD_ROWS = 4;
const uint8_t KEYPAD_COLS = 4;
const char NO_KEY_PRESSED = '\0';

// Pin definitions for 4x4 keypad (matching Wokwi diagram)
// Rows: R1=8, R2=7, R3=6, R4=5
// Cols: C1=4, C2=3, C3=2, C4=1

void KeypadInit();
char KeypadGetKey();

#endif
