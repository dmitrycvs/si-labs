#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "Console.h"
#include "PushButton.h"
#include "config.h"

// Combined intersection FSM state.
// EW has default priority (stays green until NS requests).
// Full sequence on NS request:
//   EW_GREEN → EW_YELLOW → ALL_RED → NS_GREEN → NS_YELLOW → ALL_RED → EW_GREEN
enum class TLState : uint8_t {
  EW_GREEN,
  EW_YELLOW,
  ALL_RED_1,
  NS_GREEN,
  NS_YELLOW,
  ALL_RED_2,
};

static PushButton g_button;

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

static void printState(TLState s) {
  switch (s) {
    case TLState::EW_GREEN:  printf("EW=GREEN  | NS=RED\n");    break;
    case TLState::EW_YELLOW: printf("EW=YELLOW | NS=RED\n");    break;
    case TLState::ALL_RED_1: printf("EW=RED    | NS=RED\n");    break;
    case TLState::NS_GREEN:  printf("EW=RED    | NS=GREEN\n");  break;
    case TLState::NS_YELLOW: printf("EW=RED    | NS=YELLOW\n"); break;
    case TLState::ALL_RED_2: printf("EW=RED    | NS=RED\n");    break;
  }
}

static void taskTrafficLight(void *pvParameters) {
  (void)pvParameters;
  TLState state = TLState::EW_GREEN;

  for (;;) {
    switch (state) {
      case TLState::EW_GREEN:
        setEW(true, false, false);
        setNS(false, false, true);
        printState(state);
        // Hold green until NS request is detected (debounced rising edge).
        while (!g_button.pollRisingEdge()) {
          vTaskDelay(pdMS_TO_TICKS(Config::BTN_PERIOD_MS));
        }
        state = TLState::EW_YELLOW;
        break;

      case TLState::EW_YELLOW:
        setEW(false, true, false);
        setNS(false, false, true);
        printState(state);
        vTaskDelay(pdMS_TO_TICKS(Config::YELLOW_MS));
        state = TLState::ALL_RED_1;
        break;

      case TLState::ALL_RED_1:
        setEW(false, false, true);
        setNS(false, false, true);
        printState(state);
        vTaskDelay(pdMS_TO_TICKS(Config::ALL_RED_MS));
        state = TLState::NS_GREEN;
        break;

      case TLState::NS_GREEN:
        setEW(false, false, true);
        setNS(true, false, false);
        printState(state);
        vTaskDelay(pdMS_TO_TICKS(Config::GREEN_MS));
        state = TLState::NS_YELLOW;
        break;

      case TLState::NS_YELLOW:
        setEW(false, false, true);
        setNS(false, true, false);
        printState(state);
        vTaskDelay(pdMS_TO_TICKS(Config::YELLOW_MS));
        state = TLState::ALL_RED_2;
        break;

      case TLState::ALL_RED_2:
        setEW(false, false, true);
        setNS(false, false, true);
        printState(state);
        vTaskDelay(pdMS_TO_TICKS(Config::ALL_RED_MS));
        state = TLState::EW_GREEN;
        break;
    }
  }
}

void setup() {
  Console::init();

  pinMode(Config::PIN_EW_GREEN,  OUTPUT);
  pinMode(Config::PIN_EW_YELLOW, OUTPUT);
  pinMode(Config::PIN_EW_RED,    OUTPUT);
  pinMode(Config::PIN_NS_GREEN,  OUTPUT);
  pinMode(Config::PIN_NS_YELLOW, OUTPUT);
  pinMode(Config::PIN_NS_RED,    OUTPUT);

  g_button.setup({ Config::PIN_BUTTON, true, Config::DEBOUNCE_MS });

  printf("Smart Traffic Light FSM\n");

  xTaskCreate(taskTrafficLight, "TL", 512, NULL, 1, NULL);
}

void loop() {}
