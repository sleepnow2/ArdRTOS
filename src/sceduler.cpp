/*
 * sceduler.cpp
 *
 * Created: 3/25/2021 9:15:19 AM
 *  Author: Sleepnow2
 */ 

#include "ArdRTOS.h"
#include "sceduler.h"
#include "task.h"
#include <Arduino.h>


//! GLOBAL VARIABLES BEGIN
extern volatile struct Task tasks[MAX_TASK_COUNT];
// there will always be at least one task, the idle task
extern volatile uint8_t num_tasks;
//! GLOBAL VARIABLES END


volatile uint8_t curr_task_num;
// the reason this has to be global is so that it doesn't mess with registers.
uint16_t curr_sp;


/**
 * @brief this is the backbone of the OS that does the context switching.
 * 
 */
ISR (SCEDULER_VECT, ISR_NAKED)  // timer0 overflow interrupt
{
	cli();
	// save the current context to the task class assoceated with the current context.
	OS_SaveContext(tasks[curr_task_num]._sp);
	SCEDULER_MASK_CLEAR();

	uint32_t m = millis(); // make sure that the compiler only reads this once
	// look for a task that is not currently sleeping

	/*
	Serial.print("isr SP = ");
	Serial.println(SP, HEX);
	Serial.flush();
	Serial.print("isr M = ");
	Serial.println(m, DEC);
	Serial.flush();
	//*/
	do {
		curr_task_num = (curr_task_num+1) % num_tasks;
		//TODO: after 50 days, the overflow may cause bad stuff.
		//TODO: test it to see if you can find a form that doesnt
	} while (tasks[curr_task_num]._validTime > m) ;

	// load the context of the task class by poping all of the registers off of the stack associated with the task
	SCEDULER_MASK_SET();
	OS_LoadContext(tasks[curr_task_num]._sp);
	// enable interrupts again and return
	sei();
	asm volatile("ret\r\n");
}

/**
 * @brief this task always makes sure that there are at least one good tasks to jump into, even if the task is just to yield again.
 * 
 * @return null 
 */
TASK_NOOP void os_standby_task() {
	for(;;)
		__OS_YIELD;
}

void os_begin() {
	// set the idle task for task 0 to be just an infiite delay
	// a little inefficient as it solicits at least 1 more contextChange for every [num tasks] context changes,
	// but it prevents the OS from being stuck in an infinite loop in the context changer if no tasks are available, as this one
	// will always be available to break it out like a watchdog.
	cli();

	createTask( &os_standby_task, 0 ); // the bare minimum
		
	// reset all registers to zero, and the special flag register too.
	SCEDULER_MASK_SET();
	SCEDULER_SET_OVF();
	OS_LoadContext(tasks[0]._sp);

	// return and simultaniously turn on interrupts.
	__OS_YIELD;
}

void os_delay(uint32_t ms) {
	// updates _validtime for the currently running task to now + delay in ms
	// do not interrupt me right now, im just about to hand over the context anyways.
	cli();
	// update the valid time i should become active again.
	tasks[curr_task_num]._validTime = millis() + ms;
	// calls the context switcher so that the next task can run while we wait.
	__OS_YIELD;
}

// calls the context switcher from a function-like statement. 
// you can also use the define if you want, but you have to have #include <avr/interrupt>
void  os_yield() {
	__OS_YIELD;
}

