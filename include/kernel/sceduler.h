/**
 * @file sceduler.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief handles the sceduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SCEDULER_H_
#define SCEDULER_H_

#include <stdint.h>

/**
 * @brief This is the main interface with the kernel that most people will interact with. nothing too fancy.
 * 
 */
class Sceduler {
private:

public:
    /**
     * @brief Construct a new Sceduler object. There really should only be one, but there can be more for no reason.
     */
    Sceduler();

    /**
     * @brief Create a task to be run, prepare the memory, and 
     * 
     * @param job the function for the task to run
     * @param stackSize how much memory you are going to use for this task
     * @return true  TASK   successfull in its creation - 
     * 		   false TASK unsuccessfull in its creation
     */
    bool createTask(osFuncCall job, uint16_t stackSize);

    /**
     * @brief begins ArdRTOS in its main loop
     * 
     * @param preemptive whether or not to use a preemptive OS.
     */
    void begin(bool preemptive = true);
    
    /**
     * @brief blocks the current task untill the timer is done.
     * 
     * @param ms how many milliseconds to block this task.
     */
    void delay(uint32_t ms);

    /**
     * @brief yields the current task to the next one.
     */
    void yield();
};

#endif /* SCEDULER_H_ */