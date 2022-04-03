/**
 * @file 7_preemption.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief this example uses 1_blinking_args.cpp as a base to allow for users to build their own preemptive kernel.
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set up basic preemption using ArdRTOS.
 * 
 *  Required knowledge:
 *      Basic understanding of hardware timers in C++
 *      
 *  Required hardware:
 *      3 leds
 *      (this example was built for the Arduino nano)
 */
#include <ArdRTOS.h>
#include <Arduino.h>

/**
 * @brief Constructs the basic preemption context switcher.
 *      This is an advanced topic because it is highly dependant on the architecture of the microcontroller.
 *      This example is built for the ATmega128 and ATmega328.
 *      Check your test board to see if it uses one of those boards to see if this example   
 *      Known microcontrollers that will not support preemption:
 *          ARM Coretex-M based boards
 *          (more will be added as they are discovered)
 * 
 *      In order to set up preemption for your board, it is a good idea to learn how timers and interrupts work on your board.
 */
ISR(TIMER0_COMPA_vect) {
    OS.yield();
}

// to pass the inputs to the tasks.
struct BlinkInputStruct{
    unsigned char LED;
    unsigned char GND;
    unsigned long DELAY;
};

// structure initialization
BlinkInputStruct BIS1 = {3, 0, 100};
BlinkInputStruct BIS2 = {5, 0, 200};
BlinkInputStruct BIS3 = {6, 0, 300};

void blink(void*);

/**
 * @brief this is the basic setup task used by arduino.
 * 
 */
void setup() {
    // dissable interrupts as it would be disastrous if it tried to switch context durring setup.
    noInterrupts();

    // set overflowA num for timer 0
    OCR0A = 20;
    // enable overflowA interrupt on timer 0
    TIMSK0 |= 1 << OCIE0A;


    OS.addTask(blink, &BIS1);
    OS.addTask(blink, &BIS2);
    OS.addTask(blink, &BIS3);
    
    // OS.begin handles enabling interrupts at the appropriate time.
    OS.begin();
}

/**
 * @brief 
 * 
 * @param bis_inp a pointer to the argument. In this instance, it is a pointer to 
 */
void blink(void* bis_inp) {
    BlinkInputStruct bis = *(BlinkInputStruct*)bis_inp;
    // setup
    pinMode(bis.LED, OUTPUT);
    if (bis.GND) {
        pinMode(bis.GND, OUTPUT);
        digitalWrite(bis.GND, 0);
    }

    // task main loop
    for(;;){
        // notice how it is delay instead of OS.delay.
        // if preemption was not enabled, this would never pass processor time off to 
        // the other tasks.
        digitalWrite(bis.LED, 1); delay(bis.DELAY);
        digitalWrite(bis.LED, 0); delay(bis.DELAY);
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