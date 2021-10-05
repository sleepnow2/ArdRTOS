/**
 * @file sceduler.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief handles the sceduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

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
     * @brief Create a task to be run, prepare the memory, and 
     * 
     * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
     */
    void addTask(osFuncCall &loop, unsigned stackSize=0x40, unsigned char priority=1);

    /**
     * @brief begins ArdRTOS after tasks are asigned in preemptive mode
     */
    void begin();

	/**
     * @brief simple alias for begin() if you want to be explicit in your usage of preemptive mode
     */
    void beginPreemptive();

	/**
     * @brief begins ArdRTOS after tasks are assigned in cooporative mode
     */
    void beginCooporative();

	/**
	 * @brief calls the context switcher to move onto the next task
	 * 
	 */
	void yield();

	/**
	 * @brief untill the amount time spesified passes, it calls the context switcher.
	 * 
	 * @param ms how long to wait
	 */
	void delay(unsigned long ms);

	/**
	 * @brief untill the time spesified approaches, it calls the context switcher.
	 * 
	 * @param ms the time to wait till
	 */
	void delayUntill(unsigned long ms);

	/**
	 * @brief enables the sceduler ISR
	 * 
	 */
	void enable();

	/**
	 * @brief dissables the sceduler ISR without dissabling the rest of interrupts as a whole.
	 * 
	 */
	void dissable();

	/**
	 * @brief fetches the taskID of the currently running task
	 * 
	 * @return uint8_t 
	 */
	TaskID getTaskID();

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

	friend class Semaphore;
	friend class Mutex;
private:
	/**
	 * @brief gives your priority to another task
	 * 
	 * @param taskID the task you wish to donate too
	 */
	void donatePriority(TaskID taskID);
	
	/**
	 * @brief resets the priority of the currently running task
	 * 
	 */
	void resetPriority();

};

#endif /* SCHEDULER_H_ */