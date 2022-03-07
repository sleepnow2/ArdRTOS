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

// if there is a problem, main.cpp will report it.
#define ARDRTOS_NO_WARNINGS

//! INCLUDES BEGIN
#include "ArdRTOS.h"
#include <setjmp.h>
#include <alloca.h>
//! INCLUDES END


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

// this is the arduino basic yield. this is included for any boards like the ESP8266 that rely on this for processing.
// the reason this needs to be aliased is because of a name collision with the Scheduler class.
constexpr auto ARD_YIELD = yield;

struct _TASK {
    // the function pointer to call
    osFuncCall fc;

    // the arg to pass if it exists. note, it must be a single void pointer, but
    // it can be filled with a class, struct, or basic type if you want. 
    void* arg;

    // stack size. this is used at the begining to set up the OS.
    unsigned ss;

    // the jump buffer used to store cpu context and restore execution.
    jmp_buf jb;
    
    #ifndef NO_PRIORITIES
        // priority and temporary priority. priority isnt a thing with cooporative opperating systems.
        // And since we couldnt find 
        unsigned char p;
        unsigned char tp;
    #endif
    
} tasks[ARDRTOS_TASK_COUNT];

// the current task
volatile TaskID curr = 0;
// the number of tasks
// the reason it is volatile is because then i can controll exactly when it is written and read from by 
// saving its num to a register uint8_t var when i want to read it
// and updating this value with said var when i want to write to it
volatile unsigned char numt = 0;


#ifndef NO_PRIORITIES
// slots left.
// if we are running in coop only, then we do not need to be concerned with priority or slots.
unsigned char sl = 0;
#endif // !COOP_ONLY


bool cooporative = false;

/**
 ######   #######  ##    ## ######## ######## ##     ## ########     ######  ##      ## #### ########  ######  ##     ## ######## ########
##    ## ##     ## ###   ##    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##     ##
##       ##     ## ####  ##    ##    ##         ## ##      ##       ##       ##  ##  ##  ##     ##    ##       ##     ## ##       ##     ##
##       ##     ## ## ## ##    ##    ######      ###       ##        ######  ##  ##  ##  ##     ##    ##       ######### ######   ########
##       ##     ## ##  ####    ##    ##         ## ##      ##             ## ##  ##  ##  ##     ##    ##       ##     ## ##       ##   ##
##    ## ##     ## ##   ###    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##    ##
 ######   #######  ##    ##    ##    ######## ##     ##    ##        ######   ###  ###  ####    ##     ######  ##     ## ######## ##     ##
*/

// the interrupt saves the special register and all of the function call registers
ISR(CONTEXT_SWITCHER_ISR_VECT) {
    #ifndef ARDRTOS_INTERUPTABLE
        // dissable interrupts as I do not want to be interrupted while context switching whatsoever.
        noInterrupts();
    #endif
    // setjmp saves the function return registers
    if (setjmp(tasks[curr].jb) == 0) {
        // check for stack overflows

        #ifdef NO_PRIORITIES
            // if we are coop only, then we dont have priority
            if (curr == 0) {
                curr = numt;
            } else {
                curr--;
            }
        #else
            // if we are preemptive, then we have priority
            if (sl == 0) {
                if (curr == 0) {
                    curr = numt;
                } else {
                    curr--;
                    sl = tasks[curr].tp;
                }
            } else {
                sl--;
            }
        #endif
        longjmp(tasks[curr].jb, 1);
    }
}

void Scheduler::yield() {
    // this version of yield is for use in OS.yield, OS.delay
    ARDRTOS_YIELD;
    ARD_YIELD();
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
// NOOP justified because this is exactly what i want and the compiler WILL mess this up.
NOOP void loop(osFuncCall f) {
    //CONTEXT_SWITCHER_ISR_VECT();
    while (true) {
        f();
        OS.yield();
    }
};

NOOP void loop(osFuncCallArg f, void* arg) {
    //CONTEXT_SWITCHER_ISR_VECT();
    while (true) {
        f(arg);
        OS.yield();
    }
};

Scheduler::Scheduler() {}

void Scheduler::addTask(osFuncCall loop, unsigned stackSize, unsigned char priority) {
    //grab the value of numt and store it
    unsigned char n = numt;
    // save the pointer to the function to loop over
    tasks[n].fc = loop;
    tasks[n].arg = (void*)0;
    // save how big you want the stack to be
    tasks[n].ss = stackSize + _JBLEN;

    #ifndef NO_PRIORITIES
        tasks[n].p = priority;
        tasks[n].tp = priority;
    #endif
    
    // increment numt
    numt = n + 1;
}

void Scheduler::addTask(osFuncCallArg loop, void *arg, unsigned stackSize, unsigned char priority) {
    addTask((osFuncCall)loop, stackSize, priority);
    tasks[numt-1].arg = arg;
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
NOOP void Scheduler::begin() {
    noInterrupts();
    // transfer from describing how much space they want into 
    for(curr = 0; curr < numt; curr++) {
        // after initializing a stack, move up by the stack size you want
        if(curr > 0) 
            alloca(tasks[curr-1].ss);

        if(setjmp(tasks[curr].jb) == 1) {
            noInterrupts();
            CONTEXT_SWITCHER_ISR_VECT();
            interrupts();
            if (tasks[curr].arg != 0){
                loop((osFuncCallArg)tasks[curr].fc, tasks[curr].arg);
            } else {
            loop(tasks[curr].fc); 
            }
        }
    }
    // write to memory
    numt = numt-1;
    curr = 0;

    // set up timer
    // this code only runs when coop is an option
    #ifndef COOP_ONLY
        // this setup also only runs if you desire preemption
        if (!cooporative) {
            // if we are cooporative, we wouldnt want the interrupt to keep going off.
            // this sets up the timers needed for the OS. since this needs to be ported, it is defined on a
            // port by port case.
            ARDRTOS_SETUP;
        }
    #endif
    longjmp(tasks[0].jb, 1);

}

#ifndef COOP_ONLY
    void Scheduler::beginPreemptive() {begin();}
    void Scheduler::beginCooporative() {cooporative = true; begin(); }
#else
    void Scheduler::beginCooporative() {begin();}
#endif // !COOP_ONLY

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
    if (ms == 0) return;
	delayUntill(ms+millis());
}

void Scheduler::delayMicroseconds(unsigned long us) {
    if (us == 0) return;
	delayUntillMicroseconds(us+micros());
}

void Scheduler::delayUntill(unsigned long ms) {
    unsigned long calledTime = millis();

    // this is to handle overflows in the time keeping.
	if (ms < calledTime) {
        // this will keep yielding untill it passes zero.
        // putting in max value for ms will not break this.
        // however, if this task gets time once every 50+ days, this function will break down.
        while (millis() >= calledTime){
            yield();
        }
	}
	while (millis() < ms){
        yield();
    }
}

void Scheduler::delayUntillMicroseconds(unsigned long ms) {
    unsigned long calledTime = micros();

    // this is to handle overflows in the time keeping.
	if (ms < calledTime) {
        // this will keep yielding untill it passes zero.
        // putting in max value for ms will not break this.
        // however, if this task gets time once every 50+ days, this function will break down.
        while (micros() >= calledTime){
            yield();
        }
	}
	while (micros() < ms){
        yield();
    }
}

/*
########  ########  ####  #######  ########  #### ######## #### ########  ######
##     ## ##     ##  ##  ##     ## ##     ##  ##     ##     ##  ##       ##    ##
##     ## ##     ##  ##  ##     ## ##     ##  ##     ##     ##  ##       ##
########  ########   ##  ##     ## ########   ##     ##     ##  ######    ######
##        ##   ##    ##  ##     ## ##   ##    ##     ##     ##  ##             ##
##        ##    ##   ##  ##     ## ##    ##   ##     ##     ##  ##       ##    ##
##        ##     ## ####  #######  ##     ## ####    ##    #### ########  ######
*/

#ifndef NO_PRIORITIES
    void Scheduler::setPriority(unsigned char priority) {
        tasks[curr].p = priority;
        tasks[curr].tp = priority;
    }

    void Scheduler::setPriority(TaskID taskID, unsigned char priority) {
        tasks[taskID].p = priority;
        tasks[taskID].tp = priority;
    }

    void Scheduler::donatePriority(TaskID taskID) {
        tasks[taskID].tp = max(tasks[taskID].tp, tasks[curr].tp);
    }

    void Scheduler::resetPriority() {
        tasks[curr].tp = tasks[curr].p;
    }
#endif

TaskID Scheduler::getTaskID() {
    return curr;
}