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

#if not defined(interrupts)
    #if defined(sei)
        #define interrupts sei
    #elif defined(gei)
        #define interrupts gei
    #endif
#endif // !interrupts


//! INCLUDES BEGIN
#include "ArdRTOS.h"
#include <setjmp.h>
#include <alloca.h>
//! INCLUDES END

void ARDRTOS_STACK_OVERFLOW_EX();

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

    // stack size. this is used at the begining to set up the OS and for detecting stack overflow
    unsigned ss;

    // the jump buffer used to store cpu context and restore execution.
    jmp_buf jb;
    
} tasks[ARDRTOS_TASK_COUNT];

// the current task
volatile TaskID curr = 0;
// the number of tasks
// the reason it is volatile is because then i can controll exactly when it is written and read from by 
// saving its num to a register uint8_t var when i want to read it
// and updating this value with said var when i want to write to it
volatile unsigned char numt = 0;

/**
 ######   #######  ##    ## ######## ######## ##     ## ########     ######  ##      ## #### ########  ######  ##     ## ######## ########
##    ## ##     ## ###   ##    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##     ##
##       ##     ## ####  ##    ##    ##         ## ##      ##       ##       ##  ##  ##  ##     ##    ##       ##     ## ##       ##     ##
##       ##     ## ## ## ##    ##    ######      ###       ##        ######  ##  ##  ##  ##     ##    ##       ######### ######   ########
##       ##     ## ##  ####    ##    ##         ## ##      ##             ## ##  ##  ##  ##     ##    ##       ##     ## ##       ##   ##
##    ## ##     ## ##   ###    ##    ##        ##   ##     ##       ##    ## ##  ##  ##  ##     ##    ##    ## ##     ## ##       ##    ##
 ######   #######  ##    ##    ##    ######## ##     ##    ##        ######   ###  ###  ####    ##     ######  ##     ## ######## ##     ##
*/

void v() __attribute__((optimize("O0"), signal));
void v() {
    // this is for the ESP family, since they need yield in order for their internet connections to work.
    ARD_YIELD();

    if (setjmp(tasks[curr].jb) == 0) {

        if (curr == 0) {
            curr = numt;
        } else {
            curr--;
        }
        interrupts();
        longjmp(tasks[curr].jb, 1);
    }
}

void Scheduler::yield() {
    #if defined(ESP8266)
        ESP.wdtFeed();
    #endif
    serialEventRun();
    v();
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

Scheduler::Scheduler() {}

void Scheduler::addTask(osFuncCall loop, unsigned stackSize, unsigned char priority) {
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

void Scheduler::addTask(osFuncCallArg loop, void *arg, unsigned stackSize, unsigned char priority) {
    addTask((osFuncCall)loop, stackSize, priority);
    tasks[numt-1].arg = arg;
}

// NOOP is justified because alloca will be whisked away if we dont, and we dont want that.
NOOP void Scheduler::begin() {
    // transfer from describing how much space they want into 
    for(curr = 0; curr < numt; curr++) {
        // after initializing a stack, move up by the stack size you want
        if(curr != 0) {
            alloca(tasks[curr-1].ss);
        }

        if(setjmp(tasks[curr].jb) == 1) {
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

/**
 * @brief this is for stack overflow exceptions
 * 
 */
/*
void ARDRTOS_STACK_OVERFLOW_EX() {
    noInterrupts();

    // communicate to the user which task is faulty if possible.
    // otherwise, just lock up entirely.
    while (true) {
        // this is a setting that allows for serial output
        #ifdef ARDRTOS_SERIAL_DEBUG_BAUD
        if (Serial) {
            Serial.print("STACK OVERFLOW ERROR: Caused by task ")
            Serial.println(curr+1);
            Serial.flush();
        } else {
            Serial.begin(ARDRTOS_SERIAL_DEBUG_BAUD)
        }
        #endif

        // if I have accesses to LED_BUILTIN, use that too
        #ifdef LED_BUILTIN
            // communicate the fault to the user using a half second pulse
            digitalWrite(LED_BUILTIN, 1);
            delay(500);
            digitalWrite(LED_BUILTIN, 0);
            delay(500);

            // communicate the faulty task using a series of short blinks to indicate which task.
            // unfortunately, since there is such a thing as task 0, the number of blinks will be equal to the task id + 1.
            for (int i = 0; i <= curr; i++) {
                digitalWrite(LED_BUILTIN, 1);
                delay(100);
                digitalWrite(LED_BUILTIN, 0);
                delay(100);
            }
        #endif
        // delay for 1 second between communication attempts
        delay(1000);
    }
}
//*/

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
        yield();
    }
}

void Scheduler::delayMicroseconds(unsigned long us) {
    unsigned long calledTime = micros();
    while (micros() - calledTime <= us){
        yield();
    }
}

void Scheduler::delayUntill(unsigned long ms) {
    delay(ms-millis());
}

void Scheduler::delayUntillMicroseconds(unsigned long us) {
    delayMicroseconds(us-micros());
}

TaskID Scheduler::getTaskID() {
    // tasks start at index 0
    return curr;
}