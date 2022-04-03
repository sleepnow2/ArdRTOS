/**
 * @file 0_blinking.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief uses ArdRTOS to schedule three blinking leds
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set up basic tasks using ArdRTOS
 *      To demonstrate a common pitfall of using delay() instead of OS.delay()
 * 
 *  Required knowledge:
 *      Basic c++ programming.
 *   
 *  Required hardware:
 *      3 leds
 */

#include <Arduino.h>
#include "ArdRTOS.h"

unsigned char LEDS[] = {13, 11, 9};
unsigned char GRNDS[] = {12, 10, 8};

// blinks 
void loopBlink1();
void loopBlink2();
void loopBlink3();

void setup() {
    // tasks to blink leds.
    // this regesters these functions for the scheduler to call.
    OS.addTask(loopBlink1);
    OS.addTask(loopBlink2);
    OS.addTask(loopBlink3);

    OS.begin();
    // execution never returns from begin
}

void loop() {
    // since OS.begin never returns, this function will never be called.
    // it is also entirely unecessary and can be removed in its entirety. 
}
/**
 * @brief 
 * 
 */
void loopBlink1() {

    unsigned char led = LEDS[0];
    pinMode(led, OUTPUT);
    pinMode(GRNDS[0], OUTPUT);
    digitalWrite(GRNDS[0], 0); 

    for(;;){
        digitalWrite(led, 1); 
        // notice the use of OS.delay instead of just delay. 
        // this is important as OS.delay allows for the changing of tasks, where delay does not.
        OS.delay(100);
        digitalWrite(led, 0); 
        OS.delay(100);
    }
}

// from her on out, this is the same as loopBlink1.
void loopBlink2() {
    unsigned char led = LEDS[1];
    pinMode(led, OUTPUT);
    pinMode(GRNDS[1], OUTPUT);
    digitalWrite(GRNDS[1], 0); 

    for(;;){
        digitalWrite(led, 1); 
        OS.delay(200);
        digitalWrite(led, 0); 
        OS.delay(200);
    }
}

void loopBlink3() {
    unsigned char led = LEDS[2];
    pinMode(led, OUTPUT);
    pinMode(GRNDS[2], OUTPUT);
    digitalWrite(GRNDS[2], 0); 

    for(;;){
        digitalWrite(led, 1); 
        OS.delay(300);
        digitalWrite(led, 0); 
        OS.delay(300);
    }
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