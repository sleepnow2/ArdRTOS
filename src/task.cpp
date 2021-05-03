/*
 * task.cpp
 *
 * Created: 3/25/2021 9:11:29 AM
 *  Author: Sleepnow2
 */ 

#include "ArdRTOS.h"
#include "task.h"
#include <Arduino.h>
#include <stdlib.h>


//! GLOBAL VARIABLES BEGIN
extern volatile struct Task tasks[MAX_TASK_COUNT];
// there will always be at least one task, the idle task
extern volatile uint8_t num_tasks;
//! GLOBAL VARIABLES END


// this is just in case someone returns from their task... then run this!
extern osFuncCall os_standby_task;

/**
 * @brief Create a task to be run, prepare the memory, and 
 * 
 * @param job the function for the task to run
 * @param stackSize how much memory you are going to use for this task
 * @return true  TASK   successfull in its creation - 
 * 		   false TASK unsuccessfull in its creation
 */
bool createTask(osFuncCall job, uint16_t stackSize) {

	if (num_tasks == MAX_TASK_COUNT) return false;

	// shove the heap head down by how large your stack is and th 37 bytes for context switching and guards
	osMemPtr stack_pointer = (osMemPtr)malloc(stackSize + 35);
	if (stack_pointer == 0) return false;
	stack_pointer += stackSize + 35;

	// the problem is that the gcc compiler stores numbers little endian, but the hardware stores the return addresses as big endian.
	// shove the 2 byte address of the job onto the "stack"
	*stack_pointer-- = (uint8_t)(((uint16_t)job) & 0xFF);
	*stack_pointer-- = (uint8_t)(((uint16_t)job) >> 8);

	// shove a blank context onto the "stack
	// 32 bits for the registers
	// 1 bit for special flag registers
	for (uint8_t i = 0; i < 33; i++) {
		*stack_pointer-- = 0;
	}
	//stack_pointer++;
	
	// save heap-37 (33 for registers, 2 for function call to "return to", and 2 for the "oopsies" protection loop) to _sp
	Serial.print("func SP set to :");
	Serial.println((uint16_t)stack_pointer, HEX);
	Serial.flush();
	tasks[num_tasks]._sp = (uint16_t)stack_pointer;
	tasks[num_tasks]._validTime = 0;
	num_tasks++;
	return true;
}