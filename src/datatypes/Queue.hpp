/**
 * @file Queue.h
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief provides a queue class for users.
 * @version 0.2
 * @date 2026-09-24
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 */

#ifndef __DATATYPES_QUEUE_H__
#define __DATATYPES_QUEUE_H__

/**
 * @brief This is a basic threadsafe container for queueing data 
 * 
 * @tparam T The type of data stored in the Queue
 * @tparam MAX_SIZE The maximum number of datapoints in the Queue. defaults to 10
 * @tparam LOCK_TYPE the type of locking mechanism to use. defaults to Semaphore
 * @tparam ITERATOR_TYPE Generated at compile time. Do not put insert anything into this spot. 
 */
template<typename T, uint32_t MAX_SIZE=10, typename LOCK_TYPE = Semaphore, typename ITERATOR_TYPE = __IT_TYPE__(MAX_SIZE)>
class Queue {
protected:
    
    T _data[MAX_SIZE]; // where the data is actually stored 
    ITERATOR_TYPE _front, _back, _count; // one of the counters used in operation 

    /**
     * @brief essentially n++ but wraps around MAX_SIZE
     * 
     * @param n the var to inc
     * @return ITERATOR_TYPE returns n before incrementing
     */
    ITERATOR_TYPE next(ITERATOR_TYPE &input) {
        input++;
        if (input >= MAX_SIZE) { 
            input = 0;
            return input;
        }
        return input-1;
    }

public:
    LOCK_TYPE _lock; // the locking device used to threadsafe the queue

    /**
     * @brief Construct a new Queue object
     * 
     */
    Queue() : _front(0), _back(0), _count(0) {};

    /**
     * @brief Used to push another item at the end of the queue
     * 
     * @attention Will wait forever if nothing is popping this queue.
     * 
     * @param inp The item to push onto the end of the queue
     */
    void push(const T inp) {
        LockGuard l(_lock); // lock the queue so we can investigate this.
        while (isFull()){
            // temporarily unlock the lockguard
            _lock.unlock();
            OS.yield();
            // relock it
            _lock.lock();
        }
        _count++;
        _data[next(_front)] = inp; // copy the data into the queue.
    }
    
    /**
     * @brief used to push another item at the end of the queue
     * 
     * @param inp The item to push onto the end of the queue
     * @param timeout how long to wait before returning failure
     * @return true successful queueing
     * @return false failure queueing
     */
    bool push(const T inp, uint64_t timeout) {
        uint64_t deadline = millis()+timeout;
        bool insideDeadline = !_lock.lock(deadline - millis()); // lock the queue so we can investigate this.
        while (isFull() && insideDeadline){
            _lock.unlock(); // CAREFUL! unlocks both from the bottom of the loop, and the top of the function.
            OS.yield();
            insideDeadline = !_lock.lock(deadline - millis());
        }

        if (insideDeadline) {
            // if we achieved the lock within the timeframe, AND there is space for a new one, then punch it.
            _data[_front] = inp; // copy the data into the queue.
            _front = next(_front); // increment the front of the queue.
            _count++;
            // we got here because we got the lock.
            _lock.unlock();
            return true; // got it
        } else {
            return false;
        }
    }

    /**
     * @brief Take an item off of the queue. 
     * @attention If here is no item left to pop, then block until there is something to pop.
     * 
     * @return T The item dequeued
     */
    T pop() {
        LockGuard l(_lock);
        while (isEmpty()) {
            _lock.unlock();
            OS.yield();
            _lock.lock();
        }
        _count--;
        return _data[next(_back)];
    }

    ITERATOR_TYPE size() {return _count;}
    bool isEmpty() {return _count == 0;}
    bool isFull() {return _count == MAX_SIZE;}
};

#endif // !__DATATYPES_QUEUE_H__

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