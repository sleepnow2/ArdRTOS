#include "kernel/Task.h"

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
    // this is in the same manor of an arduino program.
    if(_set) 
        _set();
    while(1) {
        _loop();
    }
}
