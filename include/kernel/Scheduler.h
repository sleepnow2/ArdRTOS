/**
 * @file sceduler.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief handles the sceduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <setjmp.h>
#include <alloca.h>

#include "kernel/ports/init.h"

#if defined(COOP_ONLY) and (not defined(NO_PRIORITIES))
    #define NO_PRIORITIES
#endif
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
     * @brief Construct a new Sceduler object. There really should only be one, but there can be more for no reason.
     */
    Scheduler();

	/**
     * @brief Create a task to be ran.
     * 
     * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
	 * @param priority how many time slots you want this task to take up
     */
    void addTask(osFuncCall loop, unsigned stackSize=0x40, unsigned char priority=1);
		/**
     * @brief Create a task to be ran with the given argument. 
     * 
     * @param loop the loop function to use
	 * @param arg a pointer to the argument to feed the function
     * @param stackSize how much memory you are going to use for this task
	 * @param priority how many time slots you want this task to take up
     */
    void addTask(osFuncCallArg loop, void *arg, unsigned stackSize=0x40, unsigned char priority=1);

    /**
     * @brief begins ArdRTOS after tasks are asigned in preemptive mode
     */
    void begin();

	/**
     * @brief simple alias for begin() if you want to be explicit in your usage of preemptive mode
     */
    void beginPreemptive();

	/**
     * @brief begins ArdRTOS after tasks are assigned in cooporative mode. 
     * the best way to turn ArdRTOS into a cooporative OS would be to define COOP_ONLY prior to inclusion.
     * this would compile the library completely differently as to save data and time.
     */
    void beginCooporative();

	/**
	 * @brief calls the context switcher to move onto the next task
	 * 
	 */
	void yield();

	/**
	 * @brief yield untill the spesified amount of time has passed
	 * 
	 * @param ms how long to wait in milliseconds
	 */
	void delay(unsigned long ms);

	/**
	 * @brief yield untill the spesified amount of time has passed
	 * 
	 * @param ns how long to wait in microseconds
	 */
	void delayMicroseconds(unsigned long us);

	/**
	 * @brief yield untill the spesified time
	 * 
	 * @param ms the time to wait till in milliseconds
	 */
	void delayUntill(unsigned long ms);

	/**
	 * @brief yield untill the spesified time
	 * 
	 * @param ms the time to wait till in microseconds
	 */
	void delayUntillMicroseconds(unsigned long us);

	/**
	 * @brief fetches the taskID of the currently running task
	 * 
	 * @return uint8_t 
	 */
	TaskID getTaskID();

#ifndef COOP_ONLY

	/**
	 * @brief Set the Priority for the currently running task
	 * 
	 * @param char how many timeslots you want this task to take up
	 */
	void setPriority(unsigned char);

	/**
	 * @brief Set the Priority for another task
	 * 
	 * @param TaskID the task to set priority of
	 * @param char the priority you want to set that task to
	 */
	void setPriority(TaskID taskID, unsigned char priority);

	// so i can get priorities.
	friend class Mutex;
private:
	/**
	 * @brief gives another task a timeslot equal to the max timeslot between the current task and the named task.
	 * 
	 * @param taskID the task you wish to donate too
	 */
	void donatePriority(TaskID taskID);
	
	/**
	 * @brief resets the priority of the currently running task
	 * 
	 */
	void resetPriority();
#endif // !COOP_ONLY
};

#endif /* SCHEDULER_H_ */