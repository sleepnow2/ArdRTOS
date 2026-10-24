/**
 * @file scheduler.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief this contains the main kernel for ArdRTOS, the scheduler class and the ISR to operate the context switcher
 * @version 0.2
 * @date 2026-10-23
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

/// @brief uses a bitfield to simplify this.
struct _TASK_FLAG {
    bool TASK_SLEEPING_MILLIS    : 1;
    bool TASK_SLEEPING_MICROS    : 1;
    bool TASK_SUSPENDED          : 1;
};

/// @brief The basic Task handler. Should not be exposed to users, as this 
struct _TASK {
    // the function pointer to call
    osFuncCall functionHandler;
    // the argument to pass if it exists. note, it must be a single void pointer, but
    // it can be filled with a class, struct, or basic type if you want. 
    void* argument;
    // stack size. this is used at the begining to set up the OS and for detecting stack overflow (future)
    unsigned stackSize;
    // the jump buffer used to store cpu context and restore execution.
    jmp_buf context;

    // these are stored here, because it means we can now check these from "kernel space" AKA globals instead of needing to jump into a stack to read it.
    // when we started sleeping (can be either ms or us)
    unsigned long sleepStart;
    // how long we are supposed to be sleeping (can be either ms or us)
    unsigned long sleepDuration;

    // a dense pack of different flags that signal
    volatile _TASK_FLAG flags;
} tasks[ARDRTOS_TASK_COUNT];

// the current task
volatile uint8_t currentTaskID = 0;

// the number of tasks
volatile uint8_t numberActiveTasks = 0;

volatile bool busy = false;

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
    //noInterrupts();
    if (setjmp(tasks[currentTaskID].context) == 0) {
        // enter an infinite loop that checks for the next open task to enter 
        while(true) {
            // switch the current task ID
            if (currentTaskID == 0) {
                currentTaskID = numberActiveTasks;
            } else {
                currentTaskID--;
            }

            // get a pointer to the current task. This is a bit more efficient because currentTaskID is volatile.
            auto *task = &tasks[currentTaskID];

            // if this task is sleeping in millis, then check to see if we are done sleeping
            if (task->flags.TASK_SLEEPING_MILLIS) {
                // if we are done sleeping, then break out
                if (millis() - task->sleepStart >= task->sleepDuration) {
                    break;
                }
                // get to the next task. this one is still sleeping. 
                continue;
            }
            
            // same as above, just for micros.
            if (task->flags.TASK_SLEEPING_MICROS) {
                if (micros() - task->sleepStart >= task->sleepDuration) {
                    break;
                }
                continue;
            }

            // if this task is suspended, skip to the next one.
            if (task->flags.TASK_SUSPENDED) continue;
        } // while(true);

        // now that we found a valid task that is not waiting, jump into context and go!;
        longjmp(tasks[currentTaskID].context, 1);
    }
    //interrupts();
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
    // save the pointer to the function to loop over
    tasks[numberActiveTasks].functionHandler = loop;
    tasks[numberActiveTasks].argument = nullptr;
    // save how big you want the stack to be
    tasks[numberActiveTasks].stackSize = stackSize + _JBLEN;

    // increment numberActiveTasks
    numberActiveTasks++;
}

void Scheduler::addTask(osFuncCallArg loop, void *argument, unsigned stackSize) {
    addTask((osFuncCall)loop, stackSize);
    tasks[numberActiveTasks-1].argument = argument;
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
NOOP void Scheduler::begin() {
    // transfer from describing how much space they want into 
    for(currentTaskID = 0; currentTaskID < numberActiveTasks; currentTaskID++) {
        // clear the previous task by how much space it wants.
        // if you are the first task, obviously you dont need to clear nobody. 
        if(currentTaskID != 0) {
            // alloca is on the stack, not the heap.
            alloca(tasks[currentTaskID-1].stackSize);
        }

        // if this is our second time here, it means we jumped in from another context!
        // that other context could be the context switcher, or it could be 
        if(setjmp(tasks[currentTaskID].context) == 1) {
            interrupts();
            if (tasks[currentTaskID].argument != nullptr){
                // slight optimization since currentTaskID will be the same for this task for the rest of time.
                // turns out, this optimization runs into an issue with "C++26: Trivial infinite loops are no longer undefined behaviour"
                // Thanks. sure. They are no longer undefined behavior, they are now just broken for me in very VERY niche tasks
                // osFuncCallArg task = (osFuncCallArg)tasks[currentTaskID].functionHandler;
                void* argument = tasks[currentTaskID].argument;
                while (true) {
                    ((osFuncCallArg)tasks[currentTaskID].functionHandler)(argument);
                    OS.yield();
                }
            } else {
                while (true) {
                    tasks[currentTaskID].functionHandler();
                    OS.yield();
                }
            }
        }
    }

    // when we get done looping, this is an optimization.
    // now, this is the index of the last active task.
    numberActiveTasks = numberActiveTasks-1;
    currentTaskID = 0;

    // start the OS
    longjmp(tasks[0].context, 1);
}

/// @brief Used to "reset" the OS when used in conjunction with Unity's TEST_PROTECT() and TEST_ABORT()
void Scheduler::TEST_UTIL_CLEAN_OS() {
    // clean off our count of tasks.
    currentTaskID = 0;
    numberActiveTasks = 0;
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
    tasks[currentTaskID].sleepStart=millis();
    tasks[currentTaskID].sleepDuration=ms;
    tasks[currentTaskID].flags.TASK_SLEEPING_MILLIS = true;
    Scheduler::yield();
}


void Scheduler::delayMicroseconds(unsigned long us) {
    // rewritten to have the checking data for 
    tasks[currentTaskID].sleepStart=micros();
    tasks[currentTaskID].sleepDuration=us;
    tasks[currentTaskID].flags.TASK_SLEEPING_MICROS = true;
    Scheduler::yield();
}

void Scheduler::delayUntil(unsigned long ms) {
    delay(ms-millis());
}

void Scheduler::delayUntilMicroseconds(unsigned long us) {
    delayMicroseconds(us-micros());
}

TaskID Scheduler::getTaskID() {
    // tasks start at index 0
    return currentTaskID;
}

void Scheduler::suspendTask(TaskID id) {
    tasks[id].flags.TASK_SUSPENDED = true;
}

void Scheduler::activateTask(TaskID id) {
    tasks[id].flags.TASK_SUSPENDED = false;
}

/**
 * MIT License
 * 
 * Copyright (c) 2026 Alex Olson
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