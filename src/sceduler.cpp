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
#include "kernel/sceduler.h"
//! INCLUDES END

// this is just a bunch of pointers to tasks to start once OS.begin() is called
// this takes up (MAX_TASK_COUNT * 2) bytes of data
Task* taskList[MAX_TASK_COUNT];
volatile uint16_t sp[MAX_TASK_COUNT];
volatile uint16_t sp_tmp;
volatile uint8_t curr_task = 0;
volatile uint8_t num_tasks = 0;

//! ============ begin context switcher ======================

bool first_through = true;

ISR (TIMER0_COMPA_vect, ISR_NAKED) {
    cli();
    OS_SaveContext(sp_tmp);

    // back track down tasks, going from last to first.
    if (curr_task == 0) {
        curr_task = num_tasks;
        first_through = false;
        }
    else {
        curr_task--;
    }
    
    sp_tmp = (uint16_t)(sp + curr_task);

    if(first_through) 
    { 
        // undo the first part of this...
        //OS_ClearContext();
        //sei();
        // yes, this technically never leaves the ISR, but we reinabled the global interrupt...
        // so it sould opperate like "normal"
        // this has the effect of starting over for each task as is.
		OS_SetSP(sp_tmp);
        taskList[curr_task]->begin();
    }

    OS_LoadContext(sp_tmp);
    reti();
}

//! ============ end context switcher ========================

Sceduler::Sceduler() {
    // turn off watchdog.
    bitClear(WDTCSR,WDIE);

    // clear contexts.

    for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)
    {
        taskList[i] = 0;
        sp[i] = 0;
    }
}

void Sceduler::addTask(Task &task, uint16_t stackSize) {

    // check to see if there is a memory violation...
    if (curr_task >= MAX_TASK_COUNT) /*TODO:*/;

    // add the pointer to the task to the end of the list.
    taskList[curr_task] = &task;

    // yes, this is weird. 
    // sp is currently being used as a data pig to store how much space it wants reserved for now.
    // this will be rectified to be actual stack pointers in Sceduler::begin();
    // the +32 is for the number of registers used.
    sp[curr_task] = stackSize + 32;

    curr_task++;
    num_tasks++;
}
void Sceduler::begin() {
    uint16_t temp1 = sp[0];
    uint16_t temp2 = 0;

    sp[0] = RAMEND - 0x80;

    // transfer from describing how much space they want into 
    for(int i = 1; i < num_tasks; i++) {
        temp2 = sp[i];
        sp[i] = sp[i-1] - temp1;
        temp1 = temp2;
    }
	num_tasks--;
	curr_task;
	sp_tmp = RAMEND;
    // set up timer
    cli();
    OCR0A = 124;
    bitSet(TIMSK0,OCIE0A);
    TIMER0_COMPA_vect();
}

void Sceduler::yield() {
    TIMER0_COMPA_vect();
}

void Sceduler::delay(uint32_t ms) {
    ms += millis();
    while (ms > millis()) {
        TIMER0_COMPA_vect();
    }
}