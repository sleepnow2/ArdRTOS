/**
 * @file 8_thread_pooling.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief this example uses 1_blinking_args.cpp as a base to allow for to see how to use basic thread pools.
 * @version 0.1
 * @date 2026-11-25
 * 
 * @copyright MIT Copyright (c) 2026 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to set up thread pools using the built in functions.
 * 
 *  Required knowledge:
 *      N/A
 *      
 *  Required hardware:
 *      3 leds.
 */

#include <ArdRTOS.h>
#include <Arduino.h>

/**
 * Have you ever wanted more RAM? 
 * Have you ever had a bunch of tiny tasks all soaking up your RAM, but not being called very often?
 * Have you ever thought to yourself that you would totally download more RAM if you could, but the Arduino just doest't connect to the internet?
 * 
 * Well now is your chance! with ThreadPools!
 * 
 * In this lesson, we will explore the structure of a ThreadPool, how to use it, and why you would use it!
 * 
 * First off, A thread pool is simply a way for multiple task like objects to share the same stack space.
 * This means, that when you have three separate tasks, you don't have to give each of them their own space in RAM for their stack.
 * 
 * This is just like downloading more RAM! Sweet!
 * 
 * Here is the kicker though... since they are all pooled together into the same stack, it is impossible to have them interrupt each other.
 * This means that OS.delay() and OS.yield() will instead pass the context to other threads OUTSIDE the pool instead of inside the pool.
 * 
 * This is not cool for very long or very dependant tasks, as they will hog all the time on the thread pool!
 * 
 * I'll go over this, and tricks to get around this limitation in a second. for now, lets see how to initialize it.
 */

// This is how you initialize your threadpool, and tell the compiler how many tasks you want in your threadpool.
const uint16_t NUMBER_OF_TASKS_IN_THREAD_POOL = 3;
ThreadPool threadPool(NUMBER_OF_TASKS_IN_THREAD_POOL);

// simple as.
// next up, lets give our good old blinking with args task bits some initialization.


enum LED_State { // this is new... wonder what we are going to do with this?
    INIT,
    ON,
    OFF
};

struct BlinkInputStruct{
    unsigned char LED;
    unsigned char GND;
    unsigned long DELAY;
    LED_State STATE; // hmmm. seems we have to store the state somehow... but why? that's a secret for later!
};

// structure initialization
BlinkInputStruct BIS1 = {7, 6, 200, LED_State::INIT};
BlinkInputStruct BIS2 = {5, 4, 400, LED_State::INIT};
BlinkInputStruct BIS3 = {3, 2, 800, LED_State::INIT};

void blink(void*);


// alright. nothing too bad. I'll explane the state bit once we get to the definition of our blink() function.
void setup() {
    threadPool.addTask(blink, &BIS1, BIS1.DELAY);
    threadPool.addTask(blink, &BIS2, BIS2.DELAY);
    threadPool.addTask(blink, &BIS3, BIS3.DELAY);

    /**
     * That is new. Why are we using the delay for our stack size?
     * 
     * We aren't. Remember when I said that using OS.delay() and OS.yield() would hog up the pool and pass it to other tasks outside the pool?
     * This means that we can't define how often our pooled tasks are going to be called from INSIDE the function, 
     *      so we need some way of telling the pool how long to wait.
     * 
     * That is what this is! We are telling the pool to wait X amount of time between the last time you called this function, and the next time. 
     * Don't worry, this is modifiable.
     * 
     * This, however, this isn't the end of our setup, next is actually adding the threadPool to the OS.
     */

    
    const uint16_t THREAD_POOL_STACK_SIZE = 512;
    OS.addPool(threadPool, THREAD_POOL_STACK_SIZE);

    /**
     * Ahhh... so HERE is where we define the Stack size!
     * 
     * word of warning, it will have to be a bit bigger than the largest task you would put in it, as it has to store some of the overhead
     *      of actually running the ThreadPool.
     */

    // standard.
    OS.begin();
}

void blink(void* bis_input) {
    // standard casting into our desired items... nothing new here
    BlinkInputStruct *bis = (BlinkInputStruct*)bis_input;

    // here is where the magic of that state variable gets used!
    // since we can't have OS.delay() or OS.yield() in our function... we have to rely on the threadPool to appropriately 
    //      space out our calls... so our calls are exactly bis->DELAY apart!
    // We did this when adding our functions to the ThreadPool during setup.
    switch (bis->STATE) {    
        // if the LED is on, turn it off and wait for the next task's go-around
        case LED_State::ON:
            digitalWrite(bis->LED, 0);
            bis->STATE = LED_State::OFF;
            break;
        // if the LED is off, turn it on and wait for the next task's go-around
        case LED_State::OFF:
            digitalWrite(bis->LED, 1);
            bis->STATE = LED_State::ON;
            break;

        // if we have never run before, initialize our stuff and set our output to on.
        case LED_State::INIT:
            pinMode(bis->LED, OUTPUT);
            digitalWrite(bis->LED, 1);
            bis->STATE = LED_State::ON;

            if (bis->GND) {
                pinMode(bis->GND, OUTPUT);
                digitalWrite(bis->GND, 0);
            }
            break;
    } // end switch(bis->STATE);

    /**
     * Basically, you have to structure your pooled threads like they are ISR's. Make sure they dont:
     *      1. Delay(), OS.Delay(), or OS.yield() in the --ISR-- pooled thread.
     *      2. Wait on Mutexes...
     *      3. Wait on queues / stacks. (to inlude pushing and popping, as they wait if full or empty!)
     *      4. Run for a long time.
     * 
     * Try not to call big functions here.  
     * While hogging the threadpool won't bring down other functions in a well written system, 
     *      it will bring down the rest of the tasks in this pool as they wait for your task to finish so they can use the stack!
     * 
     * While this may not seem all that useful right now, I can assure you that it can be, 
     *      especially for lots of little tasks that all need to be done periodically.
     */
}

/**
 * ... And that's it! we have our own ThreadPool that takes care of the blinky lights in a third of the stack space we were using!
 * 
 * Now that's what I call downloading RAM!
 */


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