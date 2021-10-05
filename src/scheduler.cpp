/**
 * @file sceduler.cpp
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief this contains the main kernel for ArdRTOS, the sceduler class and the ISR to operate the context switcher
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

//! INCLUDES BEGIN
#include "ArdRTOS.h"
#include "avr/wdt.h"
#include <setjmp.h>
#include <alloca.h>
//! INCLUDES END

struct _TASK {
    osFuncCall* fc;
    unsigned ss;
    jmp_buf jb;
    unsigned char p;
    unsigned char tp;
} tasks[MAX_TASK_COUNT+1];

// the current task
volatile TaskID curr = 0;
// the number of tasks
// the reason it is volatile is because then i can controll exactly when it is written and read from by 
// saving its num to a register uint8_t var when i want to read it
// and updating this value with said var when i want to write to it
volatile unsigned char numt = 0;
unsigned char sl = 0;

bool cooporative = false;

//! ============ begin context switcher ======================

// NOOP justified because this is exactly what i want and the compiler WILL mess this up.
NOOP void loop(osFuncCall f) {
    while (1)
        f();
}

// thie interrupt saves the special register and all of the function call registers
ISR (TIMER0_COMPA_vect, NOOP) {
    // dissable interrupts as I do not want to be interrupted while context switching whatsoever.
    noInterrupts();
    // setjmp saves the function return registers
    if (setjmp(tasks[curr].jb) == 0) {
        // this is done so we dont have to keep grabing and placing memory all the time
        // a bit of optimization since we know we wont be interrupted
        register unsigned char c = curr;
        if (sl == 0) {
            if (c == 0) {
                c = numt;
            } else {
                c--;
                sl = tasks[c].tp;
            }
            curr = c;
        } else {
            sl--;
        }
        longjmp(tasks[c].jb, 1);
    }
}

//! ============ end context switcher ========================
Scheduler::Scheduler() {
}

void Scheduler::addTask(osFuncCall &loop, unsigned stackSize, unsigned char priority) {
    //grab the value of numt and store it
    register unsigned char n = numt;
    // save the pointer to the function to loop over
    tasks[n].fc = loop;
    // save how big you want the stack to be
    tasks[n].ss = stackSize + _JBLEN;

    tasks[n].p = priority;
    tasks[n].tp = priority;

    // increment numt
    numt = n + 1;
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
NOOP void Scheduler::begin() {
    noInterrupts();
    // this is done so that i can controll exactly when memory is written to and read from;
    register unsigned char nt = numt;
    // transfer from describing how much space they want into 
    for(curr = 0; curr < nt; curr++) {
        // after initializing a stack, move up by the stack size you want
        if(curr > 0) 
            alloca(tasks[curr-1].ss);

        if(setjmp(tasks[curr].jb) == 1) {
            interrupts();
            loop(tasks[curr].fc); 
        }
    }
    // write to memory
    numt = nt-1;
    curr = 0;

    // set up timer
    if (!cooporative) {
        // if we are cooporative, we wouldnt want the interrupt to keep going off
        OCR0A = 20;
        enable();
    }
    longjmp(tasks[0].jb, 1);
}
void Scheduler::beginPreemptive() {begin();}
void Scheduler::beginCooporative() {cooporative = true; begin(); }

void Scheduler::yield() {
    // works in either cooporative or preemptive all the same.
    // give up all remaining slots left
    sl = 0;
    // yield time to the processor
    TIMER0_COMPA_vect();
}

void Scheduler::delay(unsigned long ms) {
    ms += millis();
    while (ms > millis()) {
        yield();
    }
}
void Scheduler::delayUntill(unsigned long ms) {
    while (ms > millis()) {
        yield();
    }
}

void Scheduler::enable() {
    // if we are in cooporative mode, we shouldnt turn on the preemptive OS
    if (!cooporative) bitSet(TIMSK0,OCIE0A);
}

void Scheduler::dissable() {
    // if we are in cooporative mode, we shouldnt turn on the preemptive OS
    if (!cooporative) bitClear(TIMSK0,OCIE0A);
}

TaskID Scheduler::getTaskID() {
    return curr;
}

void Scheduler::setPriority(unsigned char priority) {
    tasks[curr].p = priority;
    tasks[curr].tp = priority;
}

void Scheduler::setPriority(TaskID taskID, unsigned char priority) {
    tasks[taskID].p = priority;
    tasks[taskID].tp = priority;
}

void Scheduler::donatePriority(TaskID taskID) {
    // do a simple overflow check, and if so, give untill you have the max priority;
    if ((unsigned)tasks[taskID].tp + (unsigned)tasks[curr].tp > 0xFF) {
        tasks[taskID].tp = 0xFF;
    } else {
        tasks[taskID].tp += tasks[curr].tp;
    }
}

void Scheduler::resetPriority() {
    tasks[curr].tp = tasks[curr].p;
}