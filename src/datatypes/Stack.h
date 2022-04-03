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

template<typename T, unsigned int i, typename L = Semaphore, typename IT = __IT_TYPE__(i)>
class Stack {
private:
    // this threadsafes our Stack for use;
    L _m;
    // the main data storage
    T _data[i];
    // how far are we on our storage
    IT _num;
public:
    /**
     * @brief Construct a new Stack object
     * 
     */
    Stack();

    /**
     * @brief pushes data onto the stack
     * 
     * @param data the data to push to the stack
     * @return true successfully pushed to stack
     * @return false stack was full
     */
    bool push(T data);
    /**
     * @brief pushes data onto the stack with a time limit
     * 
     * @param data the data to push onto the stack
     * @param timeout the amount of milliseconds you are willing to wait
     * @return true successfully pushed onto the stack
     * @return false stack was full for over [timeout] milliseconds
     */
    bool push(T data, uint64_t timeout);

    /**
     * @brief pop data off of the stack
     * 
     * @return T the data off of the top of the stack if there is any, otherwise a copy of the last element is returned
     */
    T pop();
    /**
     * @brief pop data off of the stack with a time limit.
     * 
     * @param timeout the amount of milliseconds you are willing to wait
     * @return T the data off of the top of the stack if there is any, otherwise a copy of the last element is returned
     */
    T pop(uint64_t timeout);
    /**
     * @brief peek at the top of the stack without changing it
     * 
     * @return T the data off of the top of the stack if there is any, otherwise a copy of the last element is returned
     */
    T top();

    /**
     * @brief returns the number of elements stored in the stack
     * 
     * @return IT the number of elements stored in the stack
     */
    IT size() {return _num;};

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
    bool isFull() {return _num == i;};

    /**
     * @brief clears all elements from the stack
     * 
     */
    void clear() {_num = 0;};

    _Locking& getLock() {return _m;}
    void lock() {_m.lock();}
    bool lock(unsigned long long t) {return _m.lock(t);}
    bool lockImmediate() {return _m.lockImmediate();}
    void unlock() {_m.unlock();}
    bool available() {return _m.available();}
};

template<typename T, unsigned int i, typename L, typename IT>
Stack<T, i, L, IT>::Stack(): _num(0) {};

template<typename T, unsigned int i, typename L, typename IT>
bool Stack<T, i, L, IT>::push(T inp) {
    LockGuard l(_m);
    if (isFull())
        return false;
    _data[_num] = inp;
    _num += 1;
    return true;
};

template<typename T, unsigned int i, typename L, typename IT>
bool Stack<T, i, L, IT>::push(T inp, uint64_t timeout) {
    timeout += millis();

    _m.lock();
    while (isFull() && timeout < millis()) {
        _m.unlock();
        OS.yield();
        _m.lock();
    }

    if (timeout >= millis()) {
        _m.unlock();
        return false;
    }

    _data[_num++] = inp;
    _m.unlock();
    return true;
};

template<typename T, unsigned int i, typename L, typename IT>
T Stack<T, i, L, IT>::pop() {
    LockGuard l(_m);
    if (isEmpty())
        return _data[_num];
    return _data[--_num];
}
template<typename T, unsigned int i, typename L, typename IT>
T Stack<T, i, L, IT>::pop(uint64_t timeout) {
    timeout += millis();
    _m.lock();
    while (isEmpty() && timeout >= millis()) {
        _m.unlock();
        OS.yield();
        _m.lock();
    }
    // since I already have the lock, this should not spend any time waiting.
    return pop();
}
template<typename T, unsigned int i, typename L, typename IT>
T Stack<T, i, L, IT>::top() {
    LockGuard l(_m);
    return _data[_num];
}

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