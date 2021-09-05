#ifndef TASK_H_
#define TASK_H_

#include <Arduino.h>
typedef void (osFuncCall)(void);

class Task
{
private:
    osFuncCall* _set;
    osFuncCall* _loop;
public:
    Task(void);
    Task(osFuncCall &l);
    Task(osFuncCall &s, osFuncCall &l);
    void setSetup(osFuncCall &s);
    void setLoop(osFuncCall &l);
    void begin();
};


#endif // TASK_H_