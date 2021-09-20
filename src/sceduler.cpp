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
//! INCLUDES END

// this is just a bunch of pointers to tasks to start once OS.begin() is called
// this takes up (MAX_TASK_COUNT * 2) bytes of data
Task* __sceduler_task_list[MAX_TASK_COUNT];
// this is the comprehensive list of stack pointers
volatile uint16_t __sceduler_sp[MAX_TASK_COUNT];
// this contains the value of the address where the next stack pointer can be located. usefull for the asm in the context switcher
volatile uint16_t __sceduler_sp_tmp;
// the current task
volatile uint8_t __sceduler_curr_task = 0;
// the number of tasks
volatile uint8_t __sceduler_num_tasks = 0;

//! ============ begin context switcher ======================

bool first_through = true;

ISR (TIMER0_COMPA_vect, ISR_NAKED) {
    // dissable interrupts as I do not want to be interrupted while context switching whatsoever.
    noInterrupts();
    // save the context of the registers and stack pointer to __sceduler_sp_tmp
    __OS_SaveContext(__sceduler_sp_tmp);

    // back track down tasks, going from last to first.
    if (__sceduler_curr_task == 0) {
        __sceduler_curr_task = __sceduler_num_tasks;
        first_through = false;
        }
    else {
        __sceduler_curr_task--;
    }
    
    // load the address where the stack pointer can be located into __sceduler_sp_tmp
    __sceduler_sp_tmp = (uint16_t)(__sceduler_sp + __sceduler_curr_task);

    if(first_through) 
    { 
        // yes, this technically never leaves the ISR, but we reinabled the global interrupt...
        // so it sould opperate like "normal"
        // this has the effect of starting over for each task as is.
		__OS_SetSP(__sceduler_sp_tmp);
        interrupts();
        // here, we start a brand new branch off of wherever we are headed to, regardless of how spesifics are implimented
        __sceduler_task_list[__sceduler_curr_task]->begin();
    }

    // load context
    __OS_LoadContext(__sceduler_sp_tmp);
    // return enabling interrupts
    interrupts();
}

//! ============ end context switcher ========================

Sceduler::Sceduler() {
    // turn off watchdog interrupt
    bitClear(WDTCSR,WDIE);
    // turn off watchdog reset
    bitClear(WDTCSR,WDE);

    // clear contexts.
    for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)
    {
        __sceduler_task_list[i] = 0;
        __sceduler_sp[i] = 0;
    }
}

void Sceduler::addTask(osFuncCall &loop, uint16_t stackSize) {
    addTask(*(new Task(loop)), stackSize);
}
void Sceduler::addTask(osFuncCall &setup, osFuncCall &loop, uint16_t stackSize) {
    addTask(*(new Task(setup, loop)), stackSize);
}
void Sceduler::addTask(Task &task, uint16_t stackSize) {

    // add the pointer to the task to the end of the list.
    __sceduler_task_list[__sceduler_curr_task] = &task;

    // yes, this is weird. 
    // __sceduler_sp is currently being used as a data pig to store how much space it wants reserved for now.
    // this will be rectified to be actual stack pointers in Sceduler::begin();
    // the +32 is for the number of registers used.
    __sceduler_sp[__sceduler_curr_task] = stackSize + 40;

    __sceduler_curr_task++;
    __sceduler_num_tasks++;
}

void Sceduler::begin() {
    uint16_t temp1 = __sceduler_sp[0];
    uint16_t temp2 = 0;

    __sceduler_sp[0] = RAMEND - 0x80;

    // transfer from describing how much space they want into 
    for(int i = 1; i < __sceduler_num_tasks; i++) {
        temp2 = __sceduler_sp[i];
        __sceduler_sp[i] = __sceduler_sp[i-1] - temp1;
        temp1 = temp2;
    }
	__sceduler_num_tasks--;
    __sceduler_curr_task = __sceduler_num_tasks+1;
	__sceduler_sp_tmp = RAMEND;
    // set up timer
    noInterrupts();
    OCR0A = 50;
    enable();
    yield();
}

void Sceduler::yield() {
    TIMER0_COMPA_vect();
}

void Sceduler::delay(uint32_t ms) {
    ms += millis();
    while (ms > millis()) {
        yield();
    }
}
void Sceduler::delay_untill(uint32_t ms) {
    while (ms > millis()) {
        yield();
    }
}

void Sceduler::enable() {
    bitSet(TIMSK0,OCIE0A);
}

void Sceduler::dissable() {
    bitClear(TIMSK0,OCIE0A);
}