#ifndef TASK_H_
#define TASK_H_

#include <Arduino.h>
/**
 * @brief this type is to help simplify reading what the task is and to help simplify writing.
 * this is essentailly any function that takes in no arguments and returns no data.
 * 
 */
typedef void (osFuncCall)(void);

/**
 * @brief this is a basic task to 
 * 
 */
class Task
{
private:
    // setup for the task
    osFuncCall* _set;
    // loop for the task
    osFuncCall* _loop;
public:
    /**
     * @brief Construct a new Task object with no setup, only a loop
     * 
     * @param l the loop to perform
     */
    Task(osFuncCall &l);
    /**
     * @brief Construct a new Task object with both setup and loop
     * 
     * @param s the setup to perform
     * @param l the loop to perform
     */
    Task(osFuncCall &s, osFuncCall &l);
    /**
     * @brief Set the Setup function for the current task
     * 
     * @param s the setup to perform
     */
    void setSetup(osFuncCall &s);
    /**
     * @brief Set the Loop function for the current task
     * 
     * @param l the loop to perform
     */
    void setLoop(osFuncCall &l);
    /**
     * @brief begins the task as though it is in a brand new context.
     * 
     */
    void begin();
};

#endif // TASK_H_