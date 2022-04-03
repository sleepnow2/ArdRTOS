/**
 * @file scheduler.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief handles the scheduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <setjmp.h>
#include <alloca.h>

/*
 .d8888b.   .d8888b.  888    888 8888888888 8888888b.  888     888 888      8888888888 8888888b.
d88P  Y88b d88P  Y88b 888    888 888        888  "Y88b 888     888 888      888        888   Y88b
Y88b.      888    888 888    888 888        888    888 888     888 888      888        888    888
 "Y888b.   888        8888888888 8888888    888    888 888     888 888      8888888    888   d88P
    "Y88b. 888        888    888 888        888    888 888     888 888      888        8888888P"
      "888 888    888 888    888 888        888    888 888     888 888      888        888 T88b
Y88b  d88P Y88b  d88P 888    888 888        888  .d88P Y88b. .d88P 888      888        888  T88b
 "Y8888P"   "Y8888P"  888    888 8888888888 8888888P"   "Y88888P"  88888888 8888888888 888   T88b
*/

/**
 * @brief This is the main interface with the kernel that most people will interact with. nothing too fancy.
 * 
 */
class Scheduler {
public:

	/**
     * @brief Construct a new Scheduler object. There really should only be one, but there can be more for no reason.
     */
    Scheduler();

	/**
     * @brief Create a task to be ran.
     * 
     * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
     */
    static void addTask(osFuncCall loop, unsigned stackSize=0x40);
	
	/**
     * @brief Create a task to be ran with the given argument. 
     * 
     * @param loop the loop function to use
	 * @param arg a pointer to the argument to feed the function
     * @param stackSize how much memory you are going to use for this task
     */
    static void addTask(osFuncCallArg loop, void *arg, unsigned stackSize=0x40);

    /**
     * @brief begins ArdRTOS after tasks are assigned
     */
    void begin();

	/**
	 * @brief calls the context switcher to move onto the next task
	 * 
	 */
	static void yield();

	/**
	 * @brief yield until the specified amount of time has passed
	 * 
	 * @param ms how long to wait in milliseconds
	 */
	static void delay(unsigned long ms);

	/**
	 * @brief yield until the specified amount of time has passed
	 * 
	 * @param ns how long to wait in microseconds
	 */
	static void delayMicroseconds(unsigned long us);

	/**
	 * @brief yield until the specified time
	 * 
	 * @param ms the time to wait till in milliseconds
	 */
	static void delayUntil(unsigned long ms);

	/**
	 * @brief yield until the specified time
	 * 
	 * @param ms the time to wait till in microseconds
	 */
	static void delayUntilMicroseconds(unsigned long us);

	/**
	 * @brief fetches the taskID of the currently running task
	 * 
	 * @return uint8_t 
	 */
	static TaskID getTaskID();
};

#endif /* SCHEDULER_H_ */

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