/**
 * @file scheduler.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief this contains the main kernel for ArdRTOS, the scheduler class and the ISR to operate the context switcher
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 */

// if there is a problem, main.cpp will report it.
#define ARDRTOS_NO_WARNINGS

//! INCLUDES BEGIN
#include "ArdRTOS.h"
#include <setjmp.h>
#include <alloca.h>
//! INCLUDES END

__attribute__((weak)) void loop() {
    // you can use loop as a task if you so desire, but it has to be here somewhere
}

/**
 ######   ##        #######  ########     ###    ##        ######
##    ##  ##       ##     ## ##     ##   ## ##   ##       ##    ##
##        ##       ##     ## ##     ##  ##   ##  ##       ##
##   #### ##       ##     ## ########  ##     ## ##        ######
##    ##  ##       ##     ## ##     ## ######### ##             ##
##    ##  ##       ##     ## ##     ## ##     ## ##       ##    ##
 ######   ########  #######  ########  ##     ## ########  ######
*/

Scheduler OS;

struct _TASK {
    // the function pointer to call
    osFuncCall fc;
    // the arg to pass if it exists. note, it must be a single void pointer, but
    // it can be filled with a class, struct, or basic type if you want. 
    void* arg;
    // stack size. this is used at the begining to set up the OS and for detecting stack overflow (future)
    unsigned ss;
    // the jump buffer used to store cpu context and restore execution.
    jmp_buf jb;
} tasks[ARDRTOS_TASK_COUNT];

// the current task
volatile uint8_t curr = 0;

// the number of tasks
volatile uint8_t numt = 0;

/**
 ######   #######  ##    ## ######## ######## ##     ## ########     ######  ##      ## #### ########  ######  ##     ## ######## ########
##    ## ##     ## ###   ##    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##     ##
##       ##     ## ####  ##    ##    ##         ## ##      ##       ##       ##  ##  ##  ##     ##    ##       ##     ## ##       ##     ##
##       ##     ## ## ## ##    ##    ######      ###       ##        ######  ##  ##  ##  ##     ##    ##       ######### ######   ########
##       ##     ## ##  ####    ##    ##         ## ##      ##             ## ##  ##  ##  ##     ##    ##       ##     ## ##       ##   ##
##    ## ##     ## ##   ###    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##    ##
 ######   #######  ##    ##    ##    ######## ##     ##    ##        ######   ###  ###  ####    ##     ######  ##     ## ######## ##     ##
*/

void Scheduler::yield() {
    noInterrupts();
    if (setjmp(tasks[curr].jb) == 0) {
        if (curr == 0) {
            curr = numt;
        } else {
            curr--;
        }
        longjmp(tasks[curr].jb, 1);
    }
    interrupts();
}

/**
 ######   ######  ##     ## ######## ########  ##     ## ##       ######## ########
##    ## ##    ## ##     ## ##       ##     ## ##     ## ##       ##       ##     ##
##       ##       ##     ## ##       ##     ## ##     ## ##       ##       ##     ##
 ######  ##       ######### ######   ##     ## ##     ## ##       ######   ########
      ## ##       ##     ## ##       ##     ## ##     ## ##       ##       ##   ##
##    ## ##    ## ##     ## ##       ##     ## ##     ## ##       ##       ##    ##
 ######   ######  ##     ## ######## ########   #######  ######## ######## ##     ##
*/

Scheduler::Scheduler() {
}

void Scheduler::addTask(osFuncCall loop, unsigned stackSize) {
    //grab the value of numt and store it
    unsigned char n = numt;
    // save the pointer to the function to loop over
    tasks[n].fc = loop;
    tasks[n].arg = (void*)0;
    // save how big you want the stack to be
    tasks[n].ss = stackSize + _JBLEN;

    // increment numt
    numt = n + 1;
}

void Scheduler::addTask(osFuncCallArg loop, void *arg, unsigned stackSize) {
    addTask((osFuncCall)loop, stackSize);
    tasks[numt-1].arg = arg;
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
__ATTR_NORETURN__ NOOP void Scheduler::begin() {
    // transfer from describing how much space they want into 
    for(curr = 0; curr < numt; curr++) {
        // after initializing a stack, move up by the stack size you want
        if(curr != 0) {
            alloca(tasks[curr-1].ss);
        }

        if(setjmp(tasks[curr].jb) == 1) {
            interrupts();
            if (tasks[curr].arg != 0){
                // slight optimization since curr will be the same for this task for the rest of time.
                osFuncCallArg t = (osFuncCallArg)tasks[curr].fc;
                void* a = tasks[curr].arg;
                while (true) {
                    t(a);
                    OS.yield();
                }
            } else {
                while (true) {
                    tasks[curr].fc();
                    OS.yield();
                }
            }
        }
    }

    // write to memory
    numt = numt-1;
    curr = 0;

    // start the OS
    longjmp(tasks[0].jb, 1);
}

/*
########  ######## ##          ###    ##    ##  ######
##     ## ##       ##         ## ##    ##  ##  ##    ##
##     ## ##       ##        ##   ##    ####   ##
##     ## ######   ##       ##     ##    ##     ######
##     ## ##       ##       #########    ##          ##
##     ## ##       ##       ##     ##    ##    ##    ##
########  ######## ######## ##     ##    ##     ######
*/

void Scheduler::delay(unsigned long ms) {
    unsigned long calledTime = millis();
    while (millis() - calledTime <= ms){
        Scheduler::yield();
    }
}

void Scheduler::delayMicroseconds(unsigned long us) {
    unsigned long calledTime = micros();
    while (micros() - calledTime <= us){
        Scheduler::yield();
    }
}

void Scheduler::delayUntil(unsigned long ms) {
    delay(ms-millis());
}

void Scheduler::delayUntilMicroseconds(unsigned long us) {
    delayMicroseconds(us-micros());
}

TaskID Scheduler::getTaskID() {
    // tasks start at index 0
    return curr;
}

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