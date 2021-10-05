#include "ArdRTOS.h"

extern Scheduler OS;
extern volatile uint8_t curr;

Mutex::Mutex() : _lock(true) , _locking_task(UINT8_MAX) {};

void Mutex::lock() {
    // interrupts are not alloud while mutexes are being sorted out
    noInterrupts();
    // this is to prevent double locking one mutex from one task;
    if(_locking_task == curr) {
        return;
    }
    // attempt to lock forever;
    while (!_lock) {
        OS.yield();
        // interrups are enabled after OS.yield(), so
        // we must dissable interrupts before we continue to prevent
        // data from changing while we are working on it
        noInterrupts();
    }
    // update lock
    _lock = false;
    // update locking task
    _locking_task = curr;
    // return, enabling interrupts too
    interrupts();
}

bool Mutex::lock(uint64_t timeout) {
    // set a timeout time.
    timeout += millis();
    noInterrupts();

        // this is to prevent double locking one mutex from one task;
    if(_locking_task == curr) {
        interrupts();
        // this true signifies that you already have the lock... dumbass...
        return true;
    }

    while (!_lock ) {
        OS.yield();
        // interrups are enabled after OS.yield(), so
        // we must dissable interrupts before we continue to prevent
        // data from changing while we are working on it
        // here is where the ISR pertaining to millis has a chance to fire
        if (timeout < millis()) return false;
        // dissallow interrupts so that we can not be interrupted when checking for the lock again
        noInterrupts();
    }
        // update lock
    _lock = false;
    // update locking task
    _locking_task = curr;
    // enable interrupts
    interrupts();
    // return successful lock
    return true;
}

bool Mutex::lockImmediate() {
    // interrupts are not alloud while mutexes are being sorted out
    noInterrupts();
    // this is to prevent double locking one mutex from one task;
    if(_locking_task == curr) {
        interrupts();
        return true;
    }
    if(_lock) {
        _lock = false;
        _locking_task = curr;
        interrupts();
        return true;
    } else {
        interrupts();
        return false;
    }
}

void Mutex::unlock() {
    _locking_task = UINT8_MAX;
    _lock = true;
}