#include "Tasks.h"
#include "../Scheduler/Scheduler.h"
#include "../LedDriver/LedController.h"
#include "../ButtonDriver/ButtonController.h"
#include <Arduino.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// shared global state
// ---------------------------------------------------------------------------
volatile bool g_buttonPressed = false;
volatile uint32_t g_lastPressDurationMs = 0;
volatile uint16_t g_pushCount = 0;
volatile uint16_t g_shortPressCount = 0;
volatile uint16_t g_longPressCount = 0;
volatile uint32_t g_totalDurationMs = 0;

// ---------------------------------------------------------------------------
// task 1 private state
// ---------------------------------------------------------------------------
static uint8_t s_debounceCounter = 0;
static bool s_lastButtonState = false;
static bool s_inPress = false;        // true while button is held
static uint32_t s_pressStartTime = 0; // tick when press began

// ---------------------------------------------------------------------------
// task 2 private state
// ---------------------------------------------------------------------------
static bool s_blinkActive = false;
static uint8_t s_blinkHalfCycles = 0; // remaining half-cycles
static uint32_t s_lastBlinkTime = 0;

// ---------------------------------------------------------------------------
// LED & button controller instances
// ---------------------------------------------------------------------------
static LedController ledRed(PIN_LED_RED);
static LedController ledGreen(PIN_LED_GREEN);
static LedController ledYellow(PIN_LED_YELLOW);
static ButtonController button(PIN_BUTTON);

// ---------------------------------------------------------------------------
// task 1: button detector (10ms period, 0ms offset)
// debounces the button, measures press duration, sets g_lastPressDurationMs
// and g_buttonPressed on release, lights green (short) or red (long) LED
// ---------------------------------------------------------------------------
void taskButtonDetector(void)
{
  bool currentState = button.isPressed();

  if (currentState != s_lastButtonState)
  {
    // state changed – restart debounce
    s_debounceCounter = 0;
    s_lastButtonState = currentState;
  }
  else
  {
    if (s_debounceCounter < DEBOUNCE_STABLE_COUNT)
      s_debounceCounter++;

    // stable press detected (button down)
    if (s_debounceCounter == DEBOUNCE_STABLE_COUNT && currentState && !s_inPress)
    {
      s_inPress = true;
      s_pressStartTime = Scheduler::getInstance().getTickCount();
    }

    // stable release detected (button up)
    if (s_debounceCounter == DEBOUNCE_STABLE_COUNT && !currentState && s_inPress)
    {
      s_inPress = false;

      uint32_t duration = Scheduler::getInstance().getTickCount() - s_pressStartTime;
      g_lastPressDurationMs = duration;
      g_buttonPressed = true;

      // visual feedback: green = short press, red = long press
      if (duration < SHORT_PRESS_THRESHOLD_MS)
      {
        ledGreen.turnOn();
        ledRed.turnOff();
      }
      else
      {
        ledRed.turnOn();
        ledGreen.turnOff();
      }
    }
  }
}

// ---------------------------------------------------------------------------
// task 2: press counter & yellow blink (50ms period, 5ms offset)
// increments statistics, blinks yellow LED 5× (short) or 10× (long)
// ---------------------------------------------------------------------------
void taskPressCounter(void)
{
  uint32_t currentTime = Scheduler::getInstance().getTickCount();

  if (g_buttonPressed)
  {
    g_buttonPressed = false;

    uint32_t dur = g_lastPressDurationMs;
    bool isShort = (dur < SHORT_PRESS_THRESHOLD_MS);

    // update statistics
    g_pushCount++;
    g_totalDurationMs += dur;
    if (isShort)
      g_shortPressCount++;
    else
      g_longPressCount++;

    // start yellow blink sequence
    uint8_t blinks = isShort ? SHORT_PRESS_BLINK_COUNT : LONG_PRESS_BLINK_COUNT;
    s_blinkHalfCycles = blinks * 2; // each blink = on + off = 2 half-cycles
    s_blinkActive = true;
    s_lastBlinkTime = currentTime - BLINK_PERIOD_MS; // trigger first toggle immediately
  }

  // yellow LED blink state machine
  if (s_blinkActive)
  {
    if (currentTime - s_lastBlinkTime >= BLINK_PERIOD_MS)
    {
      s_lastBlinkTime = currentTime;
      ledYellow.toggle();
      s_blinkHalfCycles--;

      if (s_blinkHalfCycles == 0)
      {
        s_blinkActive = false;
        ledYellow.turnOff();
      }
    }
  }
}

// ---------------------------------------------------------------------------
// task 3: periodic reporter (10s period, 1s offset)
// prints statistics over STDIO and resets all counters
// ---------------------------------------------------------------------------
void taskStatusMonitor(void)
{
  // read atomically
  noInterrupts();
  uint16_t count = g_pushCount;
  uint16_t shortCount = g_shortPressCount;
  uint16_t longCount = g_longPressCount;
  uint32_t totalDur = g_totalDurationMs;
  interrupts();

  uint32_t avgDur = (count > 0) ? (totalDur / count) : 0;

  printf("--- Press Statistics (last 10s) ---\n");
  printf("Total presses  : %u\n", count);
  printf("Short (<500ms) : %u\n", shortCount);
  printf("Long  (>=500ms): %u\n", longCount);
  printf("Avg duration   : %lu ms\n", avgDur);
  printf("-----------------------------------\n");

  // reset statistics
  noInterrupts();
  g_pushCount = 0;
  g_shortPressCount = 0;
  g_longPressCount = 0;
  g_totalDurationMs = 0;
  interrupts();
}

// ---------------------------------------------------------------------------
// initialization
// ---------------------------------------------------------------------------
void tasksInit(void)
{
  // setup hardware
  ledRed.setup();
  ledGreen.setup();
  ledYellow.setup();
  button.setup();

  // initialize shared state
  g_buttonPressed = false;
  g_lastPressDurationMs = 0;
  g_pushCount = 0;
  g_shortPressCount = 0;
  g_longPressCount = 0;
  g_totalDurationMs = 0;

  // initialize task 1 state machine
  s_debounceCounter = 0;
  s_lastButtonState = false;
  s_inPress = false;
  s_pressStartTime = 0;

  // initialize task 2 state machine
  s_blinkActive = false;
  s_blinkHalfCycles = 0;
  s_lastBlinkTime = 0;

  // register tasks
  Scheduler &scheduler = Scheduler::getInstance();
  scheduler.addTask(taskButtonDetector, TASK1_PERIOD_MS, TASK1_OFFSET_MS);
  scheduler.addTask(taskPressCounter, TASK2_PERIOD_MS, TASK2_OFFSET_MS);
  scheduler.addTask(taskStatusMonitor, TASK3_PERIOD_MS, TASK3_OFFSET_MS);
}
