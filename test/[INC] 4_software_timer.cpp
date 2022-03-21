#define ARDRTOS_NO_WARNINGS

#include <Arduino.h>
#include "ArdRTOS.h"

Semaphore serialLock;

void t1();
void t2();
void t3();

// the pins on the test board
uint16_t taskDelays[3] = {100, 220, 500};
osFuncCall subtasks[3] = {t1, t2, t3};

Queue<osFuncCall, 10> tasksToComplete;

void Timer() {
    static uint16_t timesToNext[3] = {100, 220, 500};

    // wait untill the next task needs to be queued.
    uint16_t minTimeToNext = UINT16_MAX;
    for (uint8_t i = 0; i < 3; i++) {
        minTimeToNext = min(minTimeToNext, timesToNext[i]);
    }
    OS.delay(minTimeToNext);

    // update times and queue tasks that need to be done
    for (uint8_t i = 0; i < 3; i++) {
        timesToNext[i] -= minTimeToNext;
        if (timesToNext[i] == 0) {
            timesToNext[i] = taskDelays[i];

            tasksToComplete.lock();
            tasksToComplete.enqueue(subtasks[i]);
            tasksToComplete.unlock();
        }
    }
}

void Executer() {
    tasksToComplete.lock();
    if (!tasksToComplete.isEmpty()) {
        osFuncCall myTask = tasksToComplete.dequeue();
        tasksToComplete.unlock();
        myTask();
    }
    tasksToComplete.unlock();
}

void t1() {
    digitalWrite(3, !digitalRead(3));
}
void t2() {
    digitalWrite(5, !digitalRead(5));
}
void t3() {
    digitalWrite(6, !digitalRead(6));
}

void setup() {
    Serial.begin(115200);

    OS.addTask(Timer);
    OS.addTask(Executer);
    OS.begin();
    // program never gets here
}

void loop() {
    // do nothing
}