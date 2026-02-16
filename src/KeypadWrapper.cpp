#include "KeypadWrapper.h"

// Keypad layout for 4x4 matrix
static char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Pin configuration matching Wokwi diagram:
// Rows: R1=Pin8, R2=Pin7, R3=Pin6, R4=Pin5
// Cols: C1=Pin4, C2=Pin3, C3=Pin2, C4=Pin1
static byte keypadRowPins[KEYPAD_ROWS] = {8, 7, 6, 5};
static byte keypadColPins[KEYPAD_COLS] = {4, 3, 2, 1};

static Keypad customKeypad = Keypad(makeKeymap(keypadKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

void KeypadInit()
{
    // Keypad library handles initialization internally
}

char KeypadGetKey()
{
    char key = customKeypad.getKey();
    return (key == NO_KEY) ? NO_KEY_PRESSED : key;
}
