/*
 * sceduler.h
 *
 * Created: 3/25/2021 9:10:29 AM
 *  Author: Sleepnow2
 */ 


#ifndef SCEDULER_H_
#define SCEDULER_H_

#include <stdint.h>

void os_begin();

void os_delay(uint32_t ms);
void os_yield();
// directly calls the ISR as a function. kind of nasty, but faster than calling a function to call a function to call the isr.
// most likely that will be optimized away anyways, but might as well let the OS itself use this version.
#define __OS_YIELD SCEDULER_VECT()


#endif /* SCEDULER_H_ */