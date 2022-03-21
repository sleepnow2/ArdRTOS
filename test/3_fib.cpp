// the goal of this demo is to demonstrate the usage of Semaphores and Queues.
// Semaphores are used to protect resources from being accessed at the same time by two different tasks
// Queues are used to queue up data for handling at a later date. 
//      more information on how queues are used can be found here: 

//*
#include <Arduino.h>
#include "ArdRTOS.h"

Queue<unsigned long> toCalc;
Queue<unsigned long> doneCalc;
Semaphore serialSemaphore;

// a simple helper
inline bool isDigit(char inp) {
    return inp >='0' && inp <= '9';
}

// an efficient-ish function for finding fibonachi numbers without recursion
// recursion is a problem for microcontrollers because of their limited memory.
unsigned long fib(uint8_t i) {
    if (i <= 1) return 1;
    unsigned long long a=1, b=1, c=2;
    while(i-- > 2) {
        c = a + b;
        a = b;
        b = c;
    }
    return c;
}

void SerialInput() {
    static unsigned long inum;
    // first, lock the serial port so we can use this resource
    serialSemaphore.lock();
    // check to see if characters are available
    if (Serial.available()) {
        char inp = Serial.read();
        Serial.print(inp);
        // if it is a digit, keep feeding those digits into a number.
        if (isDigit(inp)) {
            inum *= 10;
            inum += inp-'0';
        }
        // otherwise, pass that number off to the fibonachi process.
        else {
            Serial.println();
            Serial.print("taken number: ");
            Serial.println(inum);
            // since enqueue handles locking and unlocking, it is not nessesary to 
            // get the lock for this and lock it.
            toCalc.enqueue(inum);
            inum = 0;
        }
    }
    serialSemaphore.unlock();
    OS.yield();
}

void SerialOutput() {
    doneCalc.getLock().lock();
    if (!doneCalc.isEmpty()) {
        unsigned long out = doneCalc.dequeue();
        serialSemaphore.lock();
        Serial.print("output: ");
        Serial.println(out);
        serialSemaphore.unlock();
    }
    doneCalc.getLock().unlock();
    OS.yield();
}

void FibProcess() {
    toCalc.getLock().lock();
    if (!toCalc.isEmpty()) {
        uint8_t inp = toCalc.dequeue();
        toCalc.getLock().unlock();
        digitalWrite(3,1);
        doneCalc.enqueue(fib(inp));
        digitalWrite(3,0);
    } else {
        toCalc.getLock().unlock();
        OS.yield();
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(3, OUTPUT);

    OS.addTask(SerialInput, 200, 1);
    OS.addTask(SerialOutput, 200, 1);
    //OS.addTask(FibProcess, 200, 5);
    
    OS.begin();
    // if everything works correctly, this will never get past OS.begin().
}

void loop() {
    // if everything works correctly, this will never get into loop();
}
//*/