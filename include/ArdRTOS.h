/**
 * @file ArdRTOS.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief this file handles all of the defines and the 
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __ARDRTOS_H__
#define __ARDRTOS_H__

#include <Arduino.h>

typedef void (osFuncCall)(void);

#define NAKED __attribute__((naked))
#define NOINLINE __attribute__((noinline))
#define NOOP __attribute__((optimize("O0")))

#ifndef MAX_TASK_COUNT
    #define MAX_TASK_COUNT 8
#elif MAX_TASK_COUNT <= 0
    #undef  MAX_TASK_COUNT
    #define MAX_TASK_COUNT 1
#endif // !MAX_TASK_COUNT

//! INCLUDES BEGIN
#include "kernel/init.h"
#include "datatypes/init.h"
//! INCLUDES END

extern Scheduler OS;

#endif // __ARDRTOS_H__