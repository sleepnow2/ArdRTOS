/**
 * @file Queue.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief provides a queue class for users.
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 */

#ifndef __DATATYPES_QUEUE_H__
#define __DATATYPES_QUEUE_H__


/**
 * @brief This is a basic threadsafe container for queueing data 
 * 
 * @tparam T The type of data stored in the Queue
 * @tparam i The maximum number of datapoints in the Queue. defaults to 10
 * @tparam L the type of locking mechanism to use. defaults to Semaphore
 * @tparam IT Generated at compile time. Do not put insert anything into this spot. 
 */
template<typename T, unsigned int i=10, typename L = Semaphore, typename IT = __IT_TYPE__(i)>
class Queue {
private:
    L _m;                      /** the locking device used to threadsafe the queue */
    T _data[i];                 /** where the data is actually stored */
    IT _front, _back, _count;   /** one of the counters used in opperation */

    /**
     * @brief essentially n++ but wraps around i
     * 
     * @param n the var to inc
     * @return IT returns n before incrementing
     */
    IT next(IT &n);
public:
    /**
     * @brief Construct a new Queue object
     * 
     */
    Queue();

    /**
     * @brief used to enqueue another item at the end of the queue without waiting for anything
     * 
     * @param inp The item to push onto the end of the queue
     * @return true successfull queueing
     * @return false failure queueing
     */
    bool enqueue(const T inp);
    /**
     * @brief used to enqueue another item at the end of the queue
     * 
     * @param inp The item to push onto the end of the queue
     * @param timeout how long to wait before returning failure
     * @return true successfull queueing
     * @return false failure queueing
     */
    bool enqueue(const T inp, uint64_t timeout);

    /**
     * @brief Take an item off of the queue. 
     * If here is no item left to dequeue, return the last element that was in the queue
     * 
     * @return T The item dequeued
     */
    T dequeue();
    /**
     * @brief Take an item off of the queue. 
     * If here is no item left to dequeue, return the last element that was in the queue
     * 
     * @param timeout the time to wait for an item to be put onto the stack
     * @return T The item dequeued
     */
    T dequeue(uint64_t timeout);

    T peek() {LockGuard l(_m); return _data[_back];}

    IT size() {return _count;}
    bool isEmpty() {return _count == 0;}
    bool isFull() {return _count == i;}


    
    _Locking& getLock() {return _m;}
    void lock() {_m.lock();}
    bool lock(unsigned long long t) {return _m.lock(t);}
    bool lockImmediate() {return _m.lockImmediate();}
    void unlock() {_m.unlock();}
    bool available() {return _m.available();}
};

template<typename T, unsigned int i, typename L, typename IT>
IT Queue<T, i, L, IT>::next(IT &n) {
    n++;
    if (n >= i) { 
        n = 0;
        return i;
    }
    return n-1;
}

template<typename T, unsigned int i, typename L, typename IT>
Queue<T, i, L, IT>::Queue(): _front(0), _back(0), _count(0) {};

template<typename T, unsigned int i, typename L, typename IT>
bool Queue<T, i, L, IT>::enqueue(const T inp) {
    LockGuard l(_m);
    if (isFull())
        return false;
    _data[next(_front)] = inp;
    return true;
}

template<typename T, unsigned int i, typename L, typename IT>
bool Queue<T, i, L, IT>::enqueue(const T inp, uint64_t timeout) {
    timeout += millis();
    while(!_m.lockImmediate() && timeout < millis()) {
        OS.yield();
    }

    while (isFull() && timeout < millis()) {
        _m.unlock();
        OS.yield();
        while(!_m.lockImmediate() && timeout < millis()) {
            OS.yield();
        }
    }

    if (isFull()) {
        _m.unlock();
        return false;
    } else if (timeout < millis()) {
        // we dont have a lock anyways.
        return false;
    } else {
        _data[next(_front)] = inp;
        _m.unlock();
        return true;
    }
}

template<typename T, unsigned int i, typename L, typename IT>
T Queue<T, i, L, IT>::dequeue() {
    LockGuard l(_m);
    if (isEmpty())
        return _data[_back];
    return _data[next(_back)];
}

template<typename T, unsigned int i, typename L, typename IT>
T Queue<T, i, L, IT>::dequeue(uint64_t timeout) {
    timeout += millis();
    while(!_m.lockImmediate() && timeout < millis()) {
        OS.yield();
    }
    while (isFull() && timeout < millis()){
        _m.unlock();
        OS.yield();
        // while we dont have the lock, and we are still within timeout...
        while(!_m.lockImmediate() && timeout < millis()){
            OS.yield();
        }
    }
    // this will call the deconstructor, unlocking it when we return.
    LockGuard l(_m);
    if (isEmpty())
        return _data[_back];
    return _data[next(_back)];
}

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