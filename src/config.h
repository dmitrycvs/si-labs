#ifndef CONFIG_H
#define CONFIG_H

// --- Pin definitions (Arduino Uno) ---
#define DHT_PIN             3     // DHT22 data pin
#define RELAY_PIN           12    // relay IN pin (HIGH = energised)
#define BTN_UP_PIN          9     // setpoint UP button (INPUT_PULLUP, pressed = LOW)
#define BTN_DOWN_PIN        8     // setpoint DOWN button

// --- Default control parameters ---
#define DEFAULT_SETPOINT    25.0f // default setpoint (°C)
#define DEFAULT_HYSTERESIS   2.0f // hysteresis half-band (°C)
#define SETPOINT_STEP        1.0f // °C per button press

// --- Timing (ms) ---
#define TASK_MEASURE_PERIOD_MS   2000  // DHT22 needs at least 2 s between reads
#define TASK_CONTROL_PERIOD_MS   1000  // hysteresis evaluation
#define TASK_REPORT_PERIOD_MS    2000  // serial plotter line
#define TASK_REPORT_HUMAN_MS     4000  // human-readable status block
#define BTN_DEBOUNCE_MS            50  // button debounce interval

#endif // CONFIG_H
