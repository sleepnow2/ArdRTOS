/**
 * @file 3_interrupts.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief The goal of this example is to teach how to interract with the operating system through interrups.
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 * 
 * Purpose:
 *      To teach how to use interrups as part of the operating system
 * 
 * Required knowledge:
 *      Basic understanding as to what an interrupt is and how to use it
 *          (https://www.tutorialspoint.com/embedded_systems/es_interrupts.htm)
 * 
 * Required Hardware:
 *      1 Button
 */

#include <Arduino.h>
#include "ArdRTOS.h"

// this variable is what pin the button is on. the other side of the button should be connected to ground.
// this will be used for attaching an interrupt to that pin, so make sure that attachInterrupt() will work for that pin.
// use (https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) as a reference 
// for main line boards.
const unsigned char btn = 2;

/**
 * @brief this task is just to pick a new random number every 5ms.
 */
void loop1();

/**
 * @brief this task is to detect whether to print out a number or not, and to print it if it should.
 */
void loop2();

/**
 * @brief this interrupt gets fired when the button hooked up to pin btn gets pressed.
 * thus, it signals to loop2 to print out the current value of rnd when it gets the chance.
 * 
 * If the program regesters your button press twice. that is because of something called button bounce. 
 * There are techniques to mitigate that. However, they are beyond the scope of this example.
 * 
 * it is always a good idea to use as small an interrupt as possible. 
 * This is especially true in an RTOS as you do not know which tasks stack you may end up on. 
 * This could cause either an extremely long wait time for the other tasks 
 * or it could cause stack overflows because it adds to a task that is allready near its memory budget.
 */
void button();

volatile int rnd;
volatile bool toPrint = true;

void setup() {
    noInterrupts();
    Serial.begin(115200);

    // this attaches the function button to digital pin 2
    attachInterrupt(digitalPinToInterrupt(btn), button, FALLING);
    pinMode(btn, INPUT_PULLUP);

    // add our tasks.
    OS.addTask(loop1);
    OS.addTask(loop2);

    // wait for serial to finish setting up.
    while(!Serial) {
        delay(1);
    }

    // starts interrupts at an appropriate time.
    OS.begin();
}

/**
 * @brief this task is just to pick a new random number every 5ms.
 */
void loop1() {
    rnd = rand();
    OS.delay(5);
}


/**
 * @brief this task is to detect whether to print out a number or not, and to print it if it should.
 */
void loop2() {
    if (toPrint) {
        Serial.println(rnd);
        toPrint = false;
    }
    OS.delay(50);
}

/**
 * @brief this interrupt gets fired when the button hooked up to pin btn gets pressed.
 * thus, it signals to loop2 to print out the current value of rnd when it gets the chance.
 * 
 * If the program regesters your button press twice. that is because of something called button bounce. 
 * There are techniques to mitigate that. However, they are beyond the scope of this example.
 */
void button() {
    noInterrupts();
    toPrint = true;
    interrupts();
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