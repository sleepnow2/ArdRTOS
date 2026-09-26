/**
 * @file Stack.h
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief provides a queue class for users.
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 */

#ifndef __DATATYPES_STACK_H__
#define __DATATYPES_STACK_H__

template<typename T, uint32_t MAX_SIZE, typename LOCK_TYPE = Semaphore, typename ITERATOR_TYPE = __IT_TYPE__(MAX_SIZE)>
class Stack {
private:
    
    // the main data storage
    T _data[MAX_SIZE];
    // how far are we on our storage
    ITERATOR_TYPE _num;
public:
    // this threadsafes our Stack for use;
    LOCK_TYPE _lock;
    
    /**
     * @brief Construct a new Stack object
     * 
     */
    Stack() : _num(0) {};

    /**
     * @brief pushes data onto the stack
     * 
     * @param data the data to push to the stack
     * @return true successfully pushed to stack
     * @return false stack was full
     */
    void push(T data) {
        LockGuard l(_lock);
        while (isFull()) {
            // temporarily unlock the lockguard.
            _lock.unlock();
            OS.yield();
            // relock the lockguard
            _lock.lock();
        }
        _data[_num++] = data;
    }
    /**
     * @brief pushes data onto the stack with a time limit
     * 
     * @param data the data to push onto the stack
     * @param timeout the amount of milliseconds you are willing to wait
     * @return true successfully pushed onto the stack
     * @return false stack was full for over [timeout] milliseconds
     */
    bool push(T data, uint64_t timeout) {
        uint64_t deadline = millis()+timeout;
        bool insideDeadline = !_lock.lock(deadline - millis()); // lock the queue so we can investigate this.
        while (isFull() && insideDeadline){
            _lock.unlock(); // CAREFUL! unlocks both from the bottom of the loop, and the top of the function.
            OS.yield();
            insideDeadline = !_lock.lock(deadline - millis());
        }

        if (insideDeadline) {
            data[_num++] = data;
            _lock.unlock();
            return true;
        }
        return false;
    };

    /**
     * @brief pop data off of the stack
     * 
     * @return T the data off of the top of the stack if there is any, otherwise a copy of the last element is returned
     */
    T pop();

    /**
     * @brief peek at the top of the stack without changing it
     * 
     * @return T the data off of the top of the stack if there is any, otherwise a copy of the last element is returned
     */
    T top() {
        LockGuard l(_lock);
        return _data[_num];
    }

    /**
     * @brief returns the number of elements stored in the stack
     * 
     * @return ITERATOR_TYPE the number of elements stored in the stack
     */
    ITERATOR_TYPE size() {return _num;};

    /**
     * @brief returns whether the stack is empty or not
     * 
     * @return true 
     * @return false 
     */
    bool isEmpty() {return _num == 0;};
    /**
     * @brief returns whether the stack is full or not
     * 
     * @return true 
     * @return false 
     */
    bool isFull() {return _num == MAX_SIZE;};

    /**
     * @brief clears all elements from the stack
     * 
     */
    void clear() {_num = 0;};

    _Locking& getLock() {return _lock;}
    bool available() {return _lock.available();}
};

#endif // !__DATATYPES_STACK_H__

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