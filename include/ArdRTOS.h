/**
 * @file ArdRTOS.h
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief this file handles all of the defines, typedefs, and the scheduler class
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __ARDRTOS_H__
#define __ARDRTOS_H__

//! SETTINGS BEGIN
// numerical settings 
#define ARDRTOS_TASK_COUNT 8

// uncomment below to activate or deactivate settings
//#defined COOP_ONLY
//#defined NO_PRIORITIES
//! SETTINGS END

#include <Arduino.h>

typedef void (*osFuncCall)(void);
typedef void (*osFuncCallArg)(void*);
typedef unsigned char TaskID;

#define NOOP __attribute__((optimize("O0")))

//! INCLUDES BEGIN
#include "Scheduler.h"
extern Scheduler OS;
#include "datatypes/init.h"
//! INCLUDES END

#endif // __ARDRTOS_H__