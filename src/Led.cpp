#include "Led.h"

static uint8_t ledStates[20] = {0};

void LedInit(uint8_t pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    if (pin < 20)
    {
        ledStates[pin] = LOW;
    }
}

void LedSetState(uint8_t pin, LedState state)
{
    digitalWrite(pin, state);
    if (pin < 20)
    {
        ledStates[pin] = state;
    }
}

void LedOn(uint8_t pin)
{
    LedSetState(pin, LED_ON);
}

void LedOff(uint8_t pin)
{
    LedSetState(pin, LED_OFF);
}

void LedToggle(uint8_t pin)
{
    if (pin < 20)
    {
        ledStates[pin] = !ledStates[pin];
        digitalWrite(pin, ledStates[pin]);
    }
}
