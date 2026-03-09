#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

// Maximum number of tasks the scheduler can hold
const uint8_t SCHEDULER_MAX_TASKS = 8;

typedef void (*TaskFunc)(void);

struct Task
{
    TaskFunc func;
    uint32_t period;   // How often to run (ms)
    uint32_t offset;   // Initial delay before first run (ms)
    uint32_t nextRun;  // Absolute time of next scheduled run (ms)
    bool     enabled;
};

void SchedulerInit();
bool SchedulerAddTask(TaskFunc func, uint32_t period, uint32_t offset);
void SchedulerRun();  // Call once per main loop iteration

#endif
