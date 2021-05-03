/*
 * ArdRTOS.cpp
 *
 * Created: 3/25/2021 9:36:42 AM
 *  Author: Sleepnow2
 */ 
#include "ArdRTOS.h"

//! GLOBAL VARIABLES BEGIN
volatile struct Task tasks[MAX_TASK_COUNT];
// there will always be at least one task, the idle task
volatile uint8_t num_tasks = 0;
//! GLOBAL VARIABLES END