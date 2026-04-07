#ifndef CONFIG_H
#define CONFIG_H

// --- Pin definitions (Arduino Uno) ---
#define SERVO_PIN           7     // servo PWM signal

// --- Servo physical limits (degrees) ---
#define SERVO_MIN           0     // minimum position (degrees)
#define SERVO_MAX           180   // maximum position (degrees)

// --- Signal conditioning ---
#define MEDIAN_WINDOW       5     // median filter window size (must be odd)
#define EMA_ALPHA           0.3f  // EMA weight for new sample (0.0-1.0)
#define RAMP_STEP           3     // max degrees moved per cycle during ramping

// --- Alert thresholds ---
#define ALERT_NEAR_MIN      5     // alert if position <= this value (degrees)
#define ALERT_NEAR_MAX      175   // alert if position >= this value (degrees)

// --- Timing (ms) ---
#define TASK_SIGNAL_PERIOD_MS    50    // signal conditioning period
#define TASK_REPORT_PERIOD_MS   500    // display & reporting period

#endif // CONFIG_H
