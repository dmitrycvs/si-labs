#include <Arduino.h>
#include "Console.h"
#include "PushButton.h"
#include "config.h"

enum class LedState { OFF, ON };

static LedState   g_state = LedState::OFF;
static PushButton g_button;

void setup() {
  Console::init();

  pinMode(Config::PIN_LED, OUTPUT);
  digitalWrite(Config::PIN_LED, LOW);

  g_button.setup({ Config::PIN_BUTTON, true, Config::DEBOUNCE_MS });

  printf("ButtonLED FSM ready\r\n");
  printf("State: OFF\r\n");
}

void loop() {
  if (!g_button.pollRisingEdge()) return;

  switch (g_state) {
    case LedState::OFF:
      g_state = LedState::ON;
      digitalWrite(Config::PIN_LED, HIGH);
      printf("State: ON\r\n");
      break;
    case LedState::ON:
      g_state = LedState::OFF;
      digitalWrite(Config::PIN_LED, LOW);
      printf("State: OFF\r\n");
      break;
  }
}
