/**
 * @file 1_blinking_args.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief demonstrate how to pass arguments to tasks
 * @version 0.2
 * @date 2026-09-25
 * 
 * @copyright MIT Copyright (c) 2026 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set pass basic arguments to tasks
 * 
 *  Required knowledge:
 *      Basic understanding of pointers and how to use them.
 *          (https://www.cplusplus.com/doc/tutorial/pointers/)
 *      Basic understanding of structures and how to use them.
 * 
 *  Required hardware:
 *      3 leds
 */

#include <Arduino.h>
#include "ArdRTOS.h"

// you can use structures/classes to pass large amounts of data in one pointer.
struct BlinkInputStruct{
    unsigned char LED;
    unsigned long DELAY;
};

// structure initialization

BlinkInputStruct BIS1 = {0, 200};
BlinkInputStruct BIS2 = {1, 400};
BlinkInputStruct BIS3 = {13, 800};

void blink(void*);

/**
 * @brief this is the basic setup task used by arduino.
 * 
 */
void setup() {
    OS.addTask(blink, &BIS1, 256);
    OS.addTask(blink, &BIS2, 256);
    OS.addTask(blink, &BIS3, 256);
    
    OS.begin();
    // program never gets here
}

/*
void loop() {
  // proof we can delete this with no concequences!
}
*/

/**
 * @brief 
 * 
 * @param bis_inp a pointer to the argument. In this instance, it is a pointer to the data structure we want to pass it.
 */
void blink(void* bis_inp) {
    // task setup
    BlinkInputStruct bis = *(BlinkInputStruct*)bis_inp;

    // this will be used later to make sure our blinking lights do not slowly drift off.
    uint32_t last_blink = 1000;
    pinMode(bis.LED, OUTPUT);

    // task main loop
    for(;;){
        // delayUntil works exactly like you think it does. 
        // It waits until millis() is exactly this value before resuming task behavior!
        OS.delayUntil(last_blink += bis.DELAY);
        digitalWrite(bis.LED, 0); 
        OS.delayUntil(last_blink += bis.DELAY);
        digitalWrite(bis.LED, 1); 
    }
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