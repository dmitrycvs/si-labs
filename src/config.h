#ifndef CONFIG_H
#define CONFIG_H

// --- Pin definitions (Arduino Uno) ---
#define RELAY_PIN          7    // relay module IN — HIGH = relay ON → bulb lit

// --- Signal conditioning ---
#define DEBOUNCE_SAMPLES   4    // serial command must be repeated N times to be accepted
#define INPUT_SAT_MIN      0    // minimum valid command (OFF)
#define INPUT_SAT_MAX      1    // maximum valid command (ON)

// --- Alert ---
#define ALERT_ON_DURATION_MS  10000UL  // alert if relay ON continuously > 10 s

// --- Timing (ms) ---
#define TASK_SIGNAL_PERIOD_MS    50    // signal conditioning period
#define TASK_REPORT_PERIOD_MS   500    // display & reporting period

#endif // CONFIG_H
