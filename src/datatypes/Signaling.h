/**
 * @file Signaling.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief provides a suite of signaling mechanism for users.
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 */

#ifndef __DATATYPES_MUTEX_H__
#define __DATATYPES_MUTEX_H__

// hide it away from the users. they dont need to know.
class _Locking {
public:
    virtual void lock();
    virtual bool lockImmediate();
    virtual bool unlock();
    virtual bool available();
    virtual TaskID getOwner();
};

/*
 .d8888b.  8888888888 888b     d888        d8888 8888888b.  888    888  .d88888b.  8888888b.  8888888888
d88P  Y88b 888        8888b   d8888       d88888 888   Y88b 888    888 d88P" "Y88b 888   Y88b 888
Y88b.      888        88888b.d88888      d88P888 888    888 888    888 888     888 888    888 888
 "Y888b.   8888888    888Y88888P888     d88P 888 888   d88P 8888888888 888     888 888   d88P 8888888
    "Y88b. 888        888 Y888P 888    d88P  888 8888888P"  888    888 888     888 8888888P"  888
      "888 888        888  Y8P  888   d88P   888 888        888    888 888     888 888 T88b   888
Y88b  d88P 888        888   "   888  d8888888888 888        888    888 Y88b. .d88P 888  T88b  888
 "Y8888P"  8888888888 888       888 d88P     888 888        888    888  "Y88888P"  888   T88b 8888888888
*/

/**
 * @brief this is a basic Semaphore to use for threadsafeing various resources.
 * 
 */
class Semaphore: public _Locking {
private:

    // whether or not the token is available for locking
    // true = _lock available for locking
    // false = _lock taken
    volatile bool _lock;

    // the task that locked the task. this is a utility to prevent a task from getting blocked attempting to re-lock a resource
    volatile TaskID _locking_task;

public:

    /**
     * @brief Construct a new Mutex object
     * 
     */
    Semaphore() : _lock(true) , _locking_task(0xFF) {};

    /**
     * @brief blocks the current task until a lock can be acquired
     * 
     */
    void lock() {
        // interrupts are not alloud while mutexes are being sorted out
        noInterrupts();

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
        _locking_task = OS.getTaskID();
        // return, enabling interrupts too
        interrupts();
    }

    /**
     * @brief blocks the current task until a lock is acquired or times out
     * 
     * @param timeout the amount of time before this stops attempting to acquire the lock;
     * @return true lock successfully acquired
     * @return false timed out
     */
    bool lock(unsigned long long timeout) {
        // set a timeout time.
        timeout += millis();
        noInterrupts();

        while (!_lock ) {
            interrupts();
            if (timeout < millis()) {
                return false;
            }
            OS.yield();
            // interrups are enabled after OS.yield(), so
            // we must dissable interrupts before we continue to prevent
            // data from changing while we are working on it
            // here is where the ISR pertaining to millis has a chance to fire
            // dissallow interrupts so that we can not be interrupted when checking for the lock again
            noInterrupts();
        }
            // update lock
        _lock = false;
        // update locking task
        _locking_task = OS.getTaskID();
        // enable interrupts
        interrupts();
        // return successful lock
        return true;
    }

    /**
     * @brief used to fetch the lock if available. does not wait, does not block.
     * 
     * @return true lock fetched
     * @return false lock not available
     */
    bool lockImmediate() {
        // interrupts are not alloud while mutexes are being sorted out
        noInterrupts();
        
        if(_lock) {
            _lock = false;
            _locking_task = OS.getTaskID();
            interrupts();
            return true;
        } else {
            interrupts();
            return false;
        }
    }

    /**
     * @brief frees the lock
     * 
     */
    bool unlock() {
        // interrupts are not alloud while mutexes are being sorted out
        noInterrupts();
        // check to see if we own the semaphore
        if (_locking_task != OS.getTaskID()) {
            return false;
        }
        // free the lock
        _locking_task = 0xFF;
        _lock = true;
        interrupts();
        return true;
    }


    /**
     * @brief returns the availability of the mutex
     * 
     * @return true available
     * @return false not available
     */
    bool available() {return _lock;};

    /**
     * @brief returns the task that currently owns the lock
     * 
     * @return TaskID the task that owns the lock
     */
    TaskID getOwner() {return _locking_task;};
};

/*
888b     d888 888     888 88888888888 8888888888 Y88b   d88P
8888b   d8888 888     888     888     888         Y88b d88P
88888b.d88888 888     888     888     888          Y88o88P
888Y88888P888 888     888     888     8888888       Y888P
888 Y888P 888 888     888     888     888           d888b
888  Y8P  888 888     888     888     888          d88888b
888   "   888 Y88b. .d88P     888     888         d88P Y88b
888       888  "Y88888P"      888     8888888888 d88P   Y88b
*/
#ifndef COOP_ONLY
// since priorities are not a thing in cooperative Operating systems, this is simply a placeholder for the Preemptive version.
typedef Semaphore Mutex;
#else
// because the OS is limited to coop only, the mutexes and semaphores become practically the same.
typedef Semaphore Mutex;
#endif
/*
888      .d88888b.   .d8888b.  888    d8P   .d8888b.  888     888       d8888 8888888b.  8888888b.
888     d88P" "Y88b d88P  Y88b 888   d8P   d88P  Y88b 888     888      d88888 888   Y88b 888  "Y88b
888     888     888 888    888 888  d8P    888    888 888     888     d88P888 888    888 888    888
888     888     888 888        888d88K     888        888     888    d88P 888 888   d88P 888    888
888     888     888 888        8888888b    888  88888 888     888   d88P  888 8888888P"  888    888
888     888     888 888    888 888  Y88b   888    888 888     888  d88P   888 888 T88b   888    888
888     Y88b. .d88P Y88b  d88P 888   Y88b  Y88b  d88P Y88b. .d88P d8888888888 888  T88b  888  .d88P
88888888 "Y88888P"   "Y8888P"  888    Y88b  "Y8888P88  "Y88888P" d88P     888 888   T88b 8888888P"
*/

/**
 * @brief a basic class to automatically unlock a mutex when it goes out of scope.
 * 
 * does not support Timeout and will block forever if not properly used
 */
class LockGuard {
private:
    // a pointer to the mutex that we are locking and unlocking 
    _Locking* _m;
public:

    /**
     * @brief Construct a new Lock Guard object. This locks the mutex given in the constructor. 
     * This is usefull because the deconstructor unlocks the same mutex when going out of scope
     * 
     * @param m the mutex to lock in the creation of the lock
     */
    LockGuard(_Locking &m) {
        m.lock();
        _m = &m;
    }

    /**
     * @brief Destroy the Lock Guard object freeing the lock in the process. 
     * this can be called directly or can be called by the compiler when going out of scope
     * 
     */
    ~LockGuard() {_m->unlock();}
};

#endif // !__DATATYPES_MUTEX_H__

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */