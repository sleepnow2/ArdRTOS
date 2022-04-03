/**
 * @file ArdRTOS.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief this file handles all of the defines, typedefs, and the scheduler class
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
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

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */