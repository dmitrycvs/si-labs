#ifndef TASK_REPORT_H
#define TASK_REPORT_H

#include <Arduino.h>

// Call once in setup()
void task_report_init();

// Call every loop iteration — internally rate-limits to TASK_REPORT_PERIOD_MS
void task_report_run();

#endif // TASK_REPORT_H
