/**
 * @file scheduler.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
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
    osFuncCall functionHandler;
    // the argument to pass if it exists. note, it must be a single void pointer, but
    // it can be filled with a class, struct, or basic type if you want. 
    void* argument;
    // stack size. this is used at the begining to set up the OS and for detecting stack overflow (future)
    unsigned stackSize;
    // the jump buffer used to store cpu context and restore execution.
    jmp_buf context;
    // the next task in the circular array!
    _TASK *next;
};

_TASK *head = nullptr;

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
    if (setjmp(head->context) == 0) {
        head = head->next;
        longjmp(head->context, 1);
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

void Scheduler::addTask(osFuncCallArg loop, void *argument, unsigned stackSize) {
    _TASK *next = new _TASK();
    next->functionHandler = (osFuncCall)loop;
    next->argument = argument;
    next->stackSize = stackSize + _JBLEN;

    // if this is the first task...
    if (head == nullptr) {
        // make it reference itself for the next context switch.
        next->next = next;
        head = next;
    } else {
        // make the new guy point to whoever is after the head
        next->next = head->next;
        // and make the head point to the new guy.
        head->next = next;
    }
}

void Scheduler::addTask(osFuncCall loop, unsigned stackSize) {
    addTask((osFuncCallArg)loop, nullptr, stackSize);
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
NOOP void Scheduler::begin() {
    while(true) {
        // if this is our second time here, it means we jumped in from another context!
        // that other context could be the context switcher, or it could be 
        if(setjmp(head->context) == 1) {
            interrupts();
            if (head->argument != nullptr){
                // slight optimization since currentTaskID will be the same for this task for the rest of time.
                // turns out, this optimization runs into an issue with "C++26: Trivial infinite loops are no longer undefined behaviour"
                // Thanks. sure. They are no longer undefined behavior, they are now just broken for me in very VERY niche tasks
                // osFuncCallArg task = (osFuncCallArg)tasks[currentTaskID].functionHandler;
                void* argument = head->argument;
                while (true) {
                    ((osFuncCallArg)head->functionHandler)(argument);
                    OS.yield();
                }
            } else {
                while (true) {
                    head->functionHandler();
                    OS.yield();
                }
            }
        }
        // clear the previous task by how much space it wants.
        // alloca is on the stack, not the heap.
        alloca(head->stackSize);
        // use this as a flag to determine when we have already been here.
        head->stackSize = 0;
        // get the next task
        head = head->next;
        // if we have gone through and allocated all of them, start the OS.
        if (head->stackSize == 0) longjmp(head->context, 1);
    }
}

/// @brief Used to "reset" the OS when used in conjunction with Unity's TEST_PROTECT() and TEST_ABORT()
void Scheduler::TEST_UTIL_CLEAN_OS() {
    if (head == nullptr) return;
    auto tail = head;
    head = head->next;
    while (head != tail) {
        auto throat = head;
        head=head->next;
        free(throat);
    }
    free(head);
    head = nullptr;
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
    return head;
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