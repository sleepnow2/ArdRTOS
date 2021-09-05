#include "kernel/task.h"

/**
 * @brief Construct a new task::task object. this is responsible for holding tasks 
 */
Task::Task(void) 
{
    _set = NULL;
    _loop = NULL;
}
Task::Task(osFuncCall &l)
{
    _set = NULL;
    _loop = l;
}
Task::Task(osFuncCall &s, osFuncCall &l)
{
    _set = s;
    _loop = l;
}
void Task::setSetup(osFuncCall &s)
{
    _set = s;
}
void Task::setLoop(osFuncCall &l)
{
    _loop = l;
}
void Task::begin()
{
    if(_set) 
        _set();
    if(_loop) {
        while(1) {
            _loop();
        }
    }
    
}