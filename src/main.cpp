#include <Arduino.h>
#include "Console.h"
#include "PushButton.h"
#include "config.h"

// ── FSM types ────────────────────────────────────────────────────────────────

enum class State : uint8_t { OFF, ON };
enum class Event : uint8_t { NONE, BUTTON_PRESS };

struct Fsm {
  State current = State::OFF;
};

// ── Input sampling ────────────────────────────────────────────────────────────

static Event collectEvent(PushButton &btn) {
  if (btn.pollRisingEdge()) return Event::BUTTON_PRESS;
  return Event::NONE;
}

// ── FSM step (run-to-completion) ──────────────────────────────────────────────

static void fsmStep(Fsm &fsm, Event ev) {
  if (ev == Event::NONE) return;

  switch (fsm.current) {
    case State::OFF:
      if (ev == Event::BUTTON_PRESS) {
        fsm.current = State::ON;
        digitalWrite(Config::PIN_LED, HIGH);
        printf("State: ON\r\n");
      }
      break;

    case State::ON:
      if (ev == Event::BUTTON_PRESS) {
        fsm.current = State::OFF;
        digitalWrite(Config::PIN_LED, LOW);
        printf("State: OFF\r\n");
      }
      break;
  }
}

// ── Arduino entry points ──────────────────────────────────────────────────────

static Fsm        g_fsm;
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
  const Event ev = collectEvent(g_button);
  fsmStep(g_fsm, ev);
}
