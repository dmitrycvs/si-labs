#ifndef CONFIG_H
#define CONFIG_H

// pin definitions
#define NTC_SENSOR_PIN      34  // ADC1_CH6 — analog input from NTC module
#define RED_LED_PIN         17  // alert indicator

// NTC thermistor parameters (Wokwi defaults)
#define NTC_BETA            3950.0f   // B-coefficient
#define NTC_R0              10000.0f  // resistance at T0 (ohms)
#define NTC_T0_K            298.15f   // reference temperature 25 °C in Kelvin
#define NTC_R_SERIES        10000.0f  // series resistor in the module (ohms)
#define ADC_MAX             4095.0f   // 12-bit ADC maximum value

// saturation limits (°C) — clamp readings outside this range
#define TEMP_SAT_MIN_C      (-40.0f)
#define TEMP_SAT_MAX_C      125.0f

// median filter window size (must be odd)
#define MEDIAN_FILTER_SIZE  5

// exponential moving average weight (0.0 – 1.0; higher = more responsive)
#define EMA_ALPHA           0.2f

// alert thresholds (°C) — flagged in the report
#define ALERT_HIGH_C        50.0f
#define ALERT_LOW_C         (-10.0f)

// timing config
#define TASK_MEASURE_PERIOD_MS   50   // sensor acquisition period (20-100 ms)
#define TASK_REPORT_PERIOD_MS    500  // display & reporting period

// task config
#define TASK_MEASURE_STACK_SIZE  8192
#define TASK_MEASURE_PRIORITY    3

#define TASK_STATS_STACK_SIZE    8192
#define TASK_STATS_PRIORITY      3

#define TASK_REPORT_STACK_SIZE   8192
#define TASK_REPORT_PRIORITY     2

#endif
