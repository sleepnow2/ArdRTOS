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
#include "ArdRTOS.h"

// a queue for storing how long processor should wait before printing.
Queue<unsigned long> toWait;
// a semaphore for controlling access to the serial port.
Semaphore serialSemaphore;

// this function will parse serial input and store the time to wait in the queue.
void SerialInput();
// this function will wait for the specified amount of time and then print it.
void Processor();

void setup() {
    Serial.begin(115200);

    OS.addTask(SerialInput, 200);
    OS.addTask(Processor, 200);
    
    while (!Serial) {
        // wait for serial to be ready
    }

    OS.begin();
    // if everything works correctly, this will never get past OS.begin().
}

void SerialInput() {
    static unsigned long inum;
    // first, lock the serial port so we can use this resource
    serialSemaphore.lock();

    // check to see if characters are available
    if (Serial.available()) {
        char inp = Serial.read();
        Serial.print(inp); // echo the character back to the terminal

        // if it is a digit, keep feeding those digits into a number.
        if (isDigit(inp)) {
            inum *= 10;
            inum += inp-'0';
        }
        // otherwise, pass that number off to the fibonacci process.
        else {
            Serial.println();
            Serial.print("taken number: ");
            Serial.println(inum);
            // since enqueue handles locking and unlocking, it is not necessary to 
            // get the lock for this and lock it.
            toWait.enqueue(inum);
            inum = 0;
        }
    }
    serialSemaphore.unlock();
    OS.yield();
}

void Processor() {
    // lock the queue so we can use it.
    toWait.lock();
    // if there is something in the queue, process it.
    if (!toWait.isEmpty()) {
        unsigned long inp = toWait.dequeue();
        // once we have the number, unlock the queue.
        toWait.unlock();

        // this demo will not use OS.delay for this in order to demonstrate how to make a task interruptable.
        
        // wait for the specified amount of time.
        unsigned long start = millis();
        while (millis() - start < inp) {
            // OS.yield() can be used to break up a larger computation into smaller pieces in order to allow other tasks to run.
            OS.yield();
        }

        // lock the serial port so we can use it.
        serialSemaphore.lock();
        Serial.print("Processor waited for ");
        Serial.print(inp);
        Serial.println(" ms");
        // unlock the serial port so other tasks can use it.
        serialSemaphore.unlock();
    }
    // unlocking a semaphore only works if we currently own it, so feel free to spam it.
    // it also returns whether the unlock was successful.
    toWait.unlock();
    OS.yield();
}

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
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