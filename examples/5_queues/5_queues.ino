/**
 * @file 5_queues.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief this task demonstrates how to use queues to allow for asynchronous processing.
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set up and use queues for asynchronous processing (and by extention stacks)
 *      To demonstrate how to make large computations interruptable
 * 
 *  Required knowledge:
 *      Basic knowledge of a queue and stack datastructure
 *      Basic knowledge on templates and how to use dynamic datatypes
 *   
 *  Required hardware:
 *      None
 */

#include <Arduino.h>
#include <ArdRTOS.h>

#define QUEUE_MAX_SIZE 5
#define QUEUE_DELAY 100
#define CONSUME_DELAY 1000
#define TASK_COUNT 20


struct SimulatedProcess {
	uint32_t waitTime;
	uint32_t id;
};

// a queue for storing how long processor should wait before printing.
Queue<SimulatedProcess, QUEUE_MAX_SIZE> pipe;
// a semaphore for controlling access to the serial port.
Semaphore serialSemaphore;

// this function will queue up some dummy data as defined above.
void TaskQueuer();
// this function will wait for the specified amount of time and then print it.
void Processor();

void setup() {
    Serial.begin(9600);

    OS.addTask(TaskQueuer, 0x80);
    OS.addTask(Processor, 0x80);
    
    while (!Serial) {
        // wait for serial to be ready
    }

    OS.begin();
    // if everything works correctly, this will never get past OS.begin().
}

void TaskQueuer() {
	static uint32_t i = 0;
	if (i >= TASK_COUNT) {
		return;
	}

	serialSemaphore.lock();
	Serial.print("--> Process "); 
	Serial.print(i); 
	Serial.println(" will take a bit to process");
	serialSemaphore.unlock();
	
    // This has a native mutex, guarenteeing that it will be threadsafe * **.
    // * with preemtive interrupts shown on example 7
    // ** in single core processors. I do not know how to make multi-core mutexes.
	pipe.push(SimulatedProcess{CONSUME_DELAY, i++});

	serialSemaphore.lock();
	Serial.print("### There are ");
	Serial.print(pipe.size());
	Serial.println(" tasks in the queue");
	serialSemaphore.unlock();


	OS.delay(QUEUE_DELAY);
}

void Processor() {
    // this process will block until there is data. 
    // this behavior is changed as of 2.0.0 release for simplicity of use.
    // be careful, if there is nothing pushing data to this, then it will block forever.    
	SimulatedProcess inp = pipe.pop();
	    // lock the serial port so we can use it.
    serialSemaphore.lock();    
    Serial.print("<-- Process ");
	Serial.print(inp.id);
	Serial.println(" started");
    // unlock the serial port so other tasks can use it.
    serialSemaphore.unlock();

    OS.delay(inp.waitTime);

    // lock the serial port so we can use it.
    serialSemaphore.lock();    
    Serial.print("<-- Process ");
	Serial.print(inp.id);
	Serial.print(" waited for ");
    Serial.print(inp.waitTime);
    Serial.println(" ms");
    // unlock the serial port so other tasks can use it.
    serialSemaphore.unlock();
}

/**
 * MIT License
 * 
 * Copyright (c) 2026 Alex Olson
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