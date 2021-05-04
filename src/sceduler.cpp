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
#include <Arduino.h>
#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"

//! INCLUDES END

//! PRIVATE TYPEDEFS BEGIN
struct Task {
	uint32_t _validTime;
	uint16_t _sp;
};
//! PRIVATE TYPEDEFS END

//! GLOBAL VARIABLES BEGIN
static volatile struct Task tasks[MAX_TASK_COUNT];
// there will always be at least one task, the idle task at spot 0
static volatile uint8_t num_tasks = 1;
// ArdRTOS starts at spot 0, the idle task and then initiates its kernel by yielding 
static volatile uint8_t curr_task_num = 0;
//! GLOBAL VARIABLES END

//! CONTEXT SWITCHER BEGIN
/**
 * @brief this is the backbone of the OS that does the context switching.
 * 
 */
ISR ( NOOP SCEDULER_VECT, ISR_NAKED)  // timer0 overflow interrupt
{
	// do not interrupt me while switching context!
	// no matter what it is, it can not be a higher task than switching to another brain right about now
	cli();
	// save the current context to the task class assoceated with the current context.
	asm volatile(
		" PUSH	r0  \n\t"
		" push  r1  \n\t"
		" PUSH	r2  \n\t"
		" PUSH	r3  \n\t"
		" PUSH	r4  \n\t"
		" PUSH	r5  \n\t"
		" PUSH	r6  \n\t"
		" PUSH	r7  \n\t"
		" PUSH	r8  \n\t"
		" PUSH	r9  \n\t"
		" PUSH	r10 \n\t"
		" PUSH	r11 \n\t"
		" PUSH	r12 \n\t"
		" PUSH	r13 \n\t"
		" PUSH	r14 \n\t"
		" PUSH	r15 \n\t"
		" PUSH	r16 \n\t"
		" PUSH	r17 \n\t"
		" PUSH	r18 \n\t"
		" PUSH	r19 \n\t"
		" PUSH	r20 \n\t"
		" PUSH	r21 \n\t"
		" PUSH	r22 \n\t"
		" PUSH	r23 \n\t"
		" PUSH	r24 \n\t"
		" PUSH	r25 \n\t"
		" PUSH	r26 \n\t"
		" PUSH	r27 \n\t"
		" PUSH	r28 \n\t"
		" PUSH	r29 \n\t"
		" PUSH	r30 \n\t"
		" PUSH	r31 \n\t"
        /* save the current flag register to stack */
		" IN	r0, __SREG__	\n\t"
		" PUSH	r0				\n\t");
	tasks[curr_task_num]._sp = SP;
	dbg_breakpoint();
	#if !defined(__DEBUG__)
Serial.println("t");
Serial.flush();
	#endif

	uint32_t m = millis(); // make sure that the compiler only reads this once
	// look for a task that is not currently sleeping

	#

	do {
		curr_task_num = (curr_task_num+1) % num_tasks;
		//TODO: after 50 days, the overflow may cause bad stuff.
		//TODO: test it to see if you can find a form that doesnt
	} while (tasks[curr_task_num]._validTime > m) ;

	// load the context of the task class by poping all of the registers off of the stack associated with the task
	SP = tasks[curr_task_num]._sp;
	OS_LoadContext();
	
	// enable interrupts again and return
	reti();
}
//! CONTEXT SWITCHER END

//! SCEDULER MEMBER FUNCTION DEFINITIONS BEGIN
/**
 * @brief Construct a new Sceduler
 */
Sceduler::Sceduler() {
	// initialize all tasks to never run and have a stack pointer at 0
	curr_task_num = 0;
	for (uint8_t i = 0; i < MAX_TASK_COUNT; i++) {
		tasks[i]._sp = (uint16_t)0;
		tasks[i]._validTime = (uint32_t)UINT32_MAX;
	}
};

/**
 * @brief Create a task to be run, prepare the memory, and 
 * 
 * @param job the function for the task to run
 * @param stackSize how much memory you are going to use for this task
 * @return true  TASK   successfull in its creation - 
 * 		   false TASK unsuccessfull in its creation
 */
bool Sceduler::createTask(osFuncCall job, uint16_t stackSize) {
	if (num_tasks == MAX_TASK_COUNT) 
		return false;

	// shove the heap head down by how large your stack is and th 37 bytes for context switching and guards
	#if !defined(__AVR_ATmega2560__)
		uint8_t* stack_pointer = (uint8_t*)malloc(stackSize + 35);
	if (stack_pointer == nullptr) 
		return false;
	stack_pointer += stackSize + 35;
	#elif 
	// one extra for the 24 bit address
	uint8_t* stack_pointer = (uint8_t*)malloc(stackSize + 36);
	if (stack_pointer == nullptr) 
		return false;
	stack_pointer += stackSize + 36;
	#endif

	// the problem is that the gcc compiler stores numbers little endian, but the hardware stores the return addresses as big endian.
	// shove the 2 byte address of the job onto the "stack"
	*stack_pointer = (uint8_t)((((uint16_t)job) << 1 ) & 0xFF);
	 stack_pointer--;
	*stack_pointer = (uint8_t)((((uint16_t)job) >> 7 ) & 0xFF);
	 stack_pointer--;
	#if defined(__AVR_ATmega2560__)
	*stack_pointer = (uint8_t)((((uint16_t)job) >> 15) & 0xFF);
	 stack_pointer--;
	#endif

	// shove a blank context onto the "stack
	// 32 bits for the registers
	// 1 bit for special flag registers
	for (uint8_t i = 0; i < 33; i++) {
		*stack_pointer = 0;
		 stack_pointer--;
	}
	//stack_pointer++;
	
	// save heap-37 (33 for registers, 2 for function call to "return to", and 2 for the "oopsies" protection loop) to _sp
	//Serial.print("func SP set to :");
	//Serial.println((uint16_t)stack_pointer, HEX);
	//Serial.flush();
	tasks[num_tasks]._sp = (uint16_t)stack_pointer;
	tasks[num_tasks]._validTime = 0;
	num_tasks++;
	return true;
}

/**
 * @brief begins ArdRTOS in its main loop
 */
void Sceduler::begin(bool preemptive) {
	// set the idle task for task 0 to be just an infiite delay
	// a little inefficient as it solicits at least 1 more contextChange for every [num tasks] context changes,
	// but it prevents the OS from being stuck in an infinite loop in the context changer if no tasks are available, as this one
	// will always be available to break it out like a watchdog.
		
	// if we are going to use a preemptive OS, then use it as such
	if (preemptive) {
		SCEDULER_MASK_SET();
		SCEDULER_SET_OVF();
	}

	tasks[0]._validTime = 0;
	// this makes sure that the OS always has somewhere it can return to in the case that all of the other tasks are blocking.
	// in a sense, it makes this the 
	sei();
	while (true)
		yield();
}

/**
 * @brief blocks the current task untill the timer is done.
 * 
 * @param ms how many milliseconds to block this task.
 */
void Sceduler::delay(uint32_t ms) {
	// updates _validtime for the currently running task to now + delay in ms
	// do not interrupt me right now, im just about to hand over the context anyways.
	cli();
	// update the valid time i should become active again.
	tasks[curr_task_num]._validTime = millis() + ms;
	// calls the context switcher so that the next task can run while we wait.
	yield();
}

/**
 * @brief yields the current task to the next one.
 * 
 * @note this makes sure that it deploys a return address when it yields instead of optimizing into some sort of jump instead of a call, thus breaking our OS
 */
//TODO: our major problem is here! GCC wants to use jump and we need it to use call!
NOINLINE void Sceduler::yield() {
	SCEDULER_VECT(); // doesnt work. it correctly calls the func, but uses jump
	//asm("call %P0 \r\n" : : "p"(SCEDULER_VECT_NUM)); // didn't work. it caused errors
	//asm("call __vector_14 \r\n");
}
//! SCEDULER MEMBER FUNCTION DEFININTIONS END