#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "PushButton.h"
#include "config.h"

// ── EW FSM types ──────────────────────────────────────────────────────────────

enum class EWState : uint8_t { GREEN, YELLOW, RED };
enum class EWEvent : uint8_t { NONE, NS_REQUESTED, TIMER_EXPIRED, NS_DONE };

// ── NS FSM types ──────────────────────────────────────────────────────────────

enum class NSState : uint8_t { RED, GREEN, YELLOW };
enum class NSEvent : uint8_t { NONE, EW_CLEARED, TIMER_EXPIRED };

// ── Shared primitives ─────────────────────────────────────────────────────────

static SemaphoreHandle_t g_ewClearedSem; // EW gives when RED  → NS may proceed
static SemaphoreHandle_t g_nsDoneSem;    // NS gives when RED  → EW may proceed
static PushButton        g_button;

// ── Hardware helpers ──────────────────────────────────────────────────────────

static void setEW(bool g, bool y, bool r) {
  digitalWrite(Config::PIN_EW_GREEN,  g ? HIGH : LOW);
  digitalWrite(Config::PIN_EW_YELLOW, y ? HIGH : LOW);
  digitalWrite(Config::PIN_EW_RED,    r ? HIGH : LOW);
}

static void setNS(bool g, bool y, bool r) {
  digitalWrite(Config::PIN_NS_GREEN,  g ? HIGH : LOW);
  digitalWrite(Config::PIN_NS_YELLOW, y ? HIGH : LOW);
  digitalWrite(Config::PIN_NS_RED,    r ? HIGH : LOW);
}

// ── EW FSM ────────────────────────────────────────────────────────────────────

// Blocks until an event is available for the current EW state.
static EWEvent ewCollectEvent(EWState state) {
  switch (state) {
    case EWState::GREEN:
      if (g_button.pollRisingEdge()) return EWEvent::NS_REQUESTED;
      vTaskDelay(pdMS_TO_TICKS(Config::BTN_PERIOD_MS));
      return EWEvent::NONE;

    case EWState::YELLOW:
      vTaskDelay(pdMS_TO_TICKS(Config::YELLOW_MS));
      return EWEvent::TIMER_EXPIRED;

    case EWState::RED:
      xSemaphoreTake(g_nsDoneSem, portMAX_DELAY);
      vTaskDelay(pdMS_TO_TICKS(Config::ALL_RED_MS)); // all-red safety gap
      return EWEvent::NS_DONE;
  }
  return EWEvent::NONE;
}

// Run-to-completion step: (state, event) → next state + output.
static void ewFsmStep(EWState &state, EWEvent ev) {
  switch (state) {
    case EWState::GREEN:
      if (ev == EWEvent::NS_REQUESTED) {
        state = EWState::YELLOW;
        setEW(false, true, false);
        Serial.println(F("EW=YELLOW | NS=RED"));
      }
      break;

    case EWState::YELLOW:
      if (ev == EWEvent::TIMER_EXPIRED) {
        state = EWState::RED;
        setEW(false, false, true);
        Serial.println(F("EW=RED    | NS=RED"));
        xSemaphoreGive(g_ewClearedSem);
      }
      break;

    case EWState::RED:
      if (ev == EWEvent::NS_DONE) {
        state = EWState::GREEN;
        setEW(true, false, false);
        Serial.println(F("EW=GREEN  | NS=RED"));
      }
      break;
  }
}

static void taskEW(void *pvParams) {
  (void)pvParams;
  EWState state = EWState::GREEN;
  setEW(true, false, false);
  Serial.println(F("EW=GREEN  | NS=RED"));

  for (;;) {
    const EWEvent ev = ewCollectEvent(state);
    ewFsmStep(state, ev);
  }
}

// ── NS FSM ────────────────────────────────────────────────────────────────────

// Blocks until an event is available for the current NS state.
static NSEvent nsCollectEvent(NSState state) {
  switch (state) {
    case NSState::RED:
      xSemaphoreTake(g_ewClearedSem, portMAX_DELAY);
      vTaskDelay(pdMS_TO_TICKS(Config::ALL_RED_MS)); // all-red safety gap
      return NSEvent::EW_CLEARED;

    case NSState::GREEN:
      vTaskDelay(pdMS_TO_TICKS(Config::GREEN_MS));
      return NSEvent::TIMER_EXPIRED;

    case NSState::YELLOW:
      vTaskDelay(pdMS_TO_TICKS(Config::YELLOW_MS));
      return NSEvent::TIMER_EXPIRED;
  }
  return NSEvent::NONE;
}

// Run-to-completion step: (state, event) → next state + output.
static void nsFsmStep(NSState &state, NSEvent ev) {
  switch (state) {
    case NSState::RED:
      if (ev == NSEvent::EW_CLEARED) {
        state = NSState::GREEN;
        setNS(true, false, false);
        Serial.println(F("EW=RED    | NS=GREEN"));
      }
      break;

    case NSState::GREEN:
      if (ev == NSEvent::TIMER_EXPIRED) {
        state = NSState::YELLOW;
        setNS(false, true, false);
        Serial.println(F("EW=RED    | NS=YELLOW"));
      }
      break;

    case NSState::YELLOW:
      if (ev == NSEvent::TIMER_EXPIRED) {
        state = NSState::RED;
        setNS(false, false, true);
        Serial.println(F("EW=RED    | NS=RED"));
        xSemaphoreGive(g_nsDoneSem);
      }
      break;
  }
}

static void taskNS(void *pvParams) {
  (void)pvParams;
  NSState state = NSState::RED;
  setNS(false, false, true);

  for (;;) {
    const NSEvent ev = nsCollectEvent(state);
    nsFsmStep(state, ev);
  }
}

// ── Arduino entry points ──────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  pinMode(Config::PIN_EW_GREEN,  OUTPUT);
  pinMode(Config::PIN_EW_YELLOW, OUTPUT);
  pinMode(Config::PIN_EW_RED,    OUTPUT);
  pinMode(Config::PIN_NS_GREEN,  OUTPUT);
  pinMode(Config::PIN_NS_YELLOW, OUTPUT);
  pinMode(Config::PIN_NS_RED,    OUTPUT);

  g_button.setup({ Config::PIN_BUTTON, true, Config::DEBOUNCE_MS });

  g_ewClearedSem = xSemaphoreCreateBinary();
  g_nsDoneSem    = xSemaphoreCreateBinary();

  Serial.println(F("Smart Traffic Light FSM"));

  xTaskCreate(taskEW, "EW", 256, NULL, 1, NULL);
  xTaskCreate(taskNS, "NS", 256, NULL, 1, NULL);
}

void loop() {}
