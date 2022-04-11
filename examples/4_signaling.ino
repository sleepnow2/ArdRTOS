/**
 * @file 4_signaling.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief how to use semaphores to protect external resources
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 * Purpose:
 *      To teach how to use Semaphores / Mutexes to prevent race conditions/corrupting external resources
 * 
 * Required knowledge:
 *      Basic understanding as to what a Mutex / Semaphore is and how to use it.
 *          (https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/ NOTE: this is for full c++)
 * 
 * Required Hardware:
 *      NONE
 */

#include <Arduino.h>
#include "ArdRTOS.h"

// this example is split into three sections.
    // the first section is what happens when you do not use mutexes to lock away external resources
    // the second section is what happens when you attempt to acquire the lock for the mutex twice
    // the third section is what happens when you never release the mutex
    // the fourth section is what happens when you follow proper procedures and produce good code.
#define CODEBLOCK 1

// tries to write "hello from loop1!" to serial.
void loop1();
// tries to write "good morning from loop2!" to serial. 
void loop2();

void setup() {
    OS.addTask(loop1);
    OS.addTask(loop2);

    Serial.begin(115200);
    while (!Serial) {
        // wait for serial to boot up before begining the experiments.
        delay(1);
    }
    OS.begin();
}

#if defined(CODEBLOCK) && CODEBLOCK == 1 
// not using Mutexes/Semaphores causes contamination in the outputs.

void loop1() {
    // since both tasks are attempting to use the same Serial resource at the same time, they may get mixed together.
    // this is bad, as it is most likely not intentional.
    if (millis() < 1000)
        Serial.write("hello from loop1!");
    OS.delay(1);
}
void loop2() {
    // since both tasks are attempting to use the same Serial resource at the same time, they may get mixed together.
    // this is bad, as it is most likely not intentional.
    if (millis() < 1000)
        Serial.write("good morning from loop2!");
    OS.delay(1);
}

#elif defined(CODEBLOCK) && CODEBLOCK == 2
// what happens when you attempt to lock a mutex you already have

// this mutex is used to make sure only one task is using Serial at one time.
Mutex SerialLock;

void loop1() {
    // here, we wait until we acquire serial for ourselves.
    // since, we acquired the lock first, we get to go ahead. 
    // however, in our second loop, it attempts to acquire the lock we already have, so it locks up waiting on itself to release the lock.
    // this causes this task to deadlock and never work from this point on, causing every task waiting on this mutex to also deadlock.
    SerialLock.lock();
    Serial.write("hello from loop1!");
    OS.delay(100);
}
void loop2() {
    // here, we wait until we acquire serial for ourselves.
    // this never happens, because loop1 beats us to the punch and never lets go of serial, even when it is in OS.delay().
    SerialLock.lock();
    Serial.write("good morning from loop2!");
    OS.delay(100);
}

#elif defined(CODEBLOCK) && CODEBLOCK == 3
// what happens if you never release the mutex

Mutex SerialLock;

void loop1() {
    // here, we wait until we acquire serial for ourselves.
    // since, we acquired the lock first, we get to go ahead. 
    SerialLock.lock();
    // inf loop
    for (;;) {
        // notice how this task never releases the lock.
        Serial.write("hello from loop1!");
        OS.delay(100);
    }
}
void loop2() {
    // here, we wait until we acquire serial for ourselves.
    // this never happens, because loop1 beats us to the punch and never lets go of serial, even when it is in OS.delay.
    SerialLock.lock();
    // inf loop
    for (;;) {
        Serial.write("good morning from loop2!");
        OS.delay(100);
    } 
}

#else 
// good code that allows both loops to share serial.

Mutex SerialLock;

void loop1() {
    SerialLock.lock();
    Serial.write("hello from loop1!");
    // this unlock allows for other tasks to use this resource 
    SerialLock.unlock();
    OS.delay(100);
}
void loop2() {
    SerialLock.lock();
    Serial.write("good morning from loop2!");
    // this unlock allows for other tasks to use this resource 
    SerialLock.unlock();
    OS.delay(100);
}

#endif
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