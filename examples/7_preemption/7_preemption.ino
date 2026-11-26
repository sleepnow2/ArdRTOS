/**
 * @file 7_preemption.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief this example uses 1_blinking_args.cpp as a base to allow for users to build their own preemptive kernel.
 * @version 0.2
 * @date 2026-11-25
 * 
 * @copyright MIT Copyright (c) 2026 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set up basic preemption using ArdRTOS.
 * 
 *  Required knowledge:
 *      Basic understanding of hardware timers in C++
 *      
 *  Required hardware:
 *      3 leds.
 *      This example was built for the Arduino nano, or really any arduino that uses the  Atmel Atmega328P.
 */
#include <ArdRTOS.h>
#include <Arduino.h>

/**
 * @brief Constructs the basic preemption context switcher.
 *      This is an advanced topic because it is highly dependant on the architecture of the micro controller.
 *      This example is built for the ATmega128 and ATmega328.
 *      Check your test board to see if it uses one of those boards to see if this example   
 *      Known micro controllers that will not support preemption:
 *          ARM Coretex-M based boards
 *          (more will be added as they are discovered)
 * 
 *      In order to set up preemption for your board, it is a good idea to learn how timers and interrupts work on your board.
 */
ISR(TIMER0_COMPA_vect) {
    OS.yield();
}

// performs this task just before context switching... mostly for your own implementation of preemption.
// other uses of this hook might be:
//      1. profiling your tasks by closing measuring times just before context switching.
//      2. checking to see if you overflowed your stack buffer, by either checking the stack buffer directly, or having whitespace.
//      3. 
void _PRE_CONTEXT_SWITCH() {
    // noInterrupts(); // you do not need to do this, as ISR's for AVR micro controllers already enter with no interrupts.
    // this is more a demonstration to show you that these functions exist. 
}

// performs this task just after the context switch, and just before returning to regular service.
void _POST_CONTEXT_SWITCH(){
    // interrupts(); // once again, this is not necessary.
    // in fact, it is actively detrimental as another ISR could fire INSIDE your ISR, thus eating more stack, and requiring more stack space for ALL tasks.
}


// to pass the inputs to the tasks.
struct BlinkInputStruct{
    unsigned char LED; // what pin the positive lead on your LED is attached to
    unsigned char GND; // what pin you are using for your negative lead on your LED.
    unsigned long DELAY; // how fast the LED should blink (half duty cycle);
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
    // disable interrupts as it would be disastrous if it tried to switch context durring setup.
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
        // if preemption was not working, this would never pass processor time off to the other tasks.
        // this is because the default delay does not pass context to the other processes.
        // it is still HIGHLY suggested to use OS.delay instead, as you do not spend an entire ISR cycle just sleeping
        //      when other tasks could be working right now.
        digitalWrite(bis.LED, 1); delay(bis.DELAY);
        digitalWrite(bis.LED, 0); delay(bis.DELAY);
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