#ifndef __ARDRTOS_H__
#define __ARDRTOS_H__

#include <Arduino.h>

typedef void (osFuncCall)(void);
typedef uint8_t* osMemPtr;

#define NAKED __attribute__((naked))
#define NOINLINE __attribute__((noinline))
#define WEAK __attribute__((weak))
#define NOOP __attribute__((optimize("-Os")))

#define TASK_NOOP __attribute((naked, noinline, optimize("-0s")))
#define TASK __attribute__((naked, noinline))

#ifndef MAX_TASK_COUNT
    #define MAX_TASK_COUNT 8
#endif // !MAX_TASK_COUNT
#if MAX_TASK_COUNT <= 0
    #undef  MAX_TASK_COUNT
    #define MAX_TASK_COUNT 1
#endif

//! INCLUDES BEGIN
#include "port.h"
#include "task.h"
#include "sceduler.h"
//! INCLUDES END

#endif // __ARDRTOS_H__