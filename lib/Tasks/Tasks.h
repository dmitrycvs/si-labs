#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdbool.h>

// hardware pin definitions
#define PIN_LED_RED 12    // red led    - long press indicator
#define PIN_LED_GREEN 11  // green led  - short press indicator
#define PIN_LED_YELLOW 10 // yellow led - blink counter
#define PIN_BUTTON 2      // button input

// task timing configuration
// task 1: button detector (10ms)
#define TASK1_PERIOD_MS 10
#define TASK1_OFFSET_MS 0

// task 2: press counter & yellow blink (50ms)
#define TASK2_PERIOD_MS 50
#define TASK2_OFFSET_MS 5

// task 3: periodic report (10s)
#define TASK3_PERIOD_MS 10000
#define TASK3_OFFSET_MS 1000

// debounce configuration
#define DEBOUNCE_DELAY_MS 50
#define DEBOUNCE_STABLE_COUNT (DEBOUNCE_DELAY_MS / TASK1_PERIOD_MS)

// press classification threshold
#define SHORT_PRESS_THRESHOLD_MS 500

// yellow LED blink configuration
#define SHORT_PRESS_BLINK_COUNT 5 // blinks for short press
#define LONG_PRESS_BLINK_COUNT 10 // blinks for long press
#define BLINK_PERIOD_MS 100       // 100ms per half-period (on or off)

// shared state (volatile for isr/task safety)
extern volatile bool g_buttonPressed;           // set by task 1, consumed by task 2
extern volatile uint32_t g_lastPressDurationMs; // duration of last completed press [ms]
extern volatile uint16_t g_pushCount;           // total press count
extern volatile uint16_t g_shortPressCount;     // short press count (<500ms)
extern volatile uint16_t g_longPressCount;      // long press count (>=500ms)
extern volatile uint32_t g_totalDurationMs;     // sum of all press durations [ms]

// task function declarations
void taskButtonDetector(void); // debounce, measure duration, signal red/green LED
void taskPressCounter(void);   // update stats, blink yellow LED
void taskStatusMonitor(void);  // report stats every 10s, reset counters

// initialization
void tasksInit(void);

#endif // TASKS_H
