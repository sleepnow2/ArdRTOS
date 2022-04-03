/**
 * @file 2_time_Sensitive.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief how to block interrupts for a period of time
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 * Purpose:
 *      To teach how to block interrupts for very time sensitive applciations.
 *      To teach the effects of leaving interrupts off for extended periods of time.
 *      To teach a common mistake that will cause ArdRTOS to lock up.
 * 
 * Required knowledge:
 *      Basic understanding as to what an interrupt is, not necessarily how to use them.
 *          (https://www.tutorialspoint.com/embedded_systems/es_interrupts.htm)
 * 
 * Required Hardware:
 *      1 HC-SR04 ultrasonic range sensor found in most Arduino kits.
 */


#include <Arduino.h>
#include "ArdRTOS.h"

//
unsigned dist;

// this is the trigger pin for the 
constexpr unsigned char trigger = 6;

// this is the echo pin for the
constexpr unsigned char echo = 5;

// speed of sound in mm/mS.
constexpr float SOS = 0.34029;

void setup() {
    Serial.begin(115200);

    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);

    OS.addTask(dispnum, 32);
    OS.addTask(distance, 128);
    OS.begin();

    // if everything works correctly, this will never get past OS.begin()
}

/**
 * @brief measures the distance between a HC-SR04 ultrasonic distance sensor and the surface in front of it.
 *  After measureing the distance, save it to "dist" in mm
 */
void distance() {
    // dissable interrupts because the next block is very time sensitive.
    noInterrupts();
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    // this function is EXTREMELY sensitive to interrupting, so it NEEDS the protection around it.
    // this function returns microseconds. multiplied by mm/mS returns mm for round trip.
    // we then need to divide that answer by to to get the distance out instead of distance for a round trip.
    dist = pulseIn(echo, HIGH)*SOS/2;
    
    // reinable interrupts. 
    // This is important because OS.delay relies on millis() which depends on an internal interrupt.
    // if you do not reinable interrupts, this task will freeze until interrupts are enabled again.
    interrupts();

    // wait till next sensor reading
    OS.delay(100);
}

void dispnum() {
    // this should be wrapped in a semaphore, but that is covered in 4_signaling.
    Serial.println(dist);
    OS.delay(500);
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