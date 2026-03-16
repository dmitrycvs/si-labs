#ifndef CONFIG_H
#define CONFIG_H

// pin definitions
#define TEMP_SENSOR_PIN     18
#define GREEN_LED_PIN       16
#define RED_LED_PIN         17

// temperature threshold config (hysteresis: 25°C ± 1°C)
#define THRESHOLD_NOMINAL_C  25.0f   // nominal threshold
#define THRESHOLD_HIGH_C     26.0f   // alert triggers when temp >= this
#define THRESHOLD_LOW_C      24.0f   // alert clears when temp < this

// debounce: consecutive samples required to confirm a state change
#define DEBOUNCE_COUNT       3

// timing config
#define TASK_MEASURE_PERIOD_MS   100   // sensor acquisition period (20-100 ms)
#define TASK_REPORT_PERIOD_MS    500   // display/reporting period

// task config
#define TASK_MEASURE_STACK_SIZE  4096
#define TASK_MEASURE_PRIORITY    3

#define TASK_STATS_STACK_SIZE    4096
#define TASK_STATS_PRIORITY      3

#define TASK_REPORT_STACK_SIZE   4096
#define TASK_REPORT_PRIORITY     2

#endif
