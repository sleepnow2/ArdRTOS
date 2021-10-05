#ifndef __DATATYPES_STACK_H__
#define __DATATYPES_STACK_H__

#include "Mutex.h"

extern Scheduler OS;

/** 
 * this is here so that we can use as little data as possible in this datatype.
 * what happens is that the datatype used to index the queue will always be the smallest 
 * kind of integer that can index the whole queue.
 * 
 * if you only need 10 indexes, the data saved for doing this would be 3 bytes. which if you are saving uint8 values then
 * you have a 18.75% smaller datatype with no change whatsoever in capabilities.
 */
namespace __DATATYPES_STACK_HELPER__ {
    template<bool FITS8, bool FITS16> struct Index {
		using Type = uint32_t;
	};

	template<> struct Index<false, true> {
		using Type = uint16_t;
	};

	template<> struct Index<true, true> {
		using Type = uint8_t;
	};
}


template<typename T, unsigned int i, typename IT = typename __DATATYPES_STACK_HELPER__::Index<(i<UINT8_MAX),(1<UINT16_MAX)>::Type>
class Stack {
private:
    // this threadsafes our Stack for use;
    Mutex _m;
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
     * @return true sucessuflly pushed to stack
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
};

template<typename T, unsigned int i, typename IT>
Stack<T, i, IT>::Stack(): _num(0) {};

template<typename T, unsigned int i, typename IT>
bool Stack<T, i, IT>::push(T inp) {
    LockGuard l(_m);
    if (isFull())
        return false;
    _data[_num] = inp;
    _num += 1;
    return true;
};

template<typename T, unsigned int i, typename IT>
bool Stack<T, i, IT>::push(T inp, uint64_t timeout) {
    LockGuard l(_m);
    timeout += millis();
    while (isFull() && timeout < millis()) 
        OS.yield();

    if (timeout >= millis()) 
        return false;

    _data[_num++] = inp;
    return true;
};

template<typename T, unsigned int i, typename IT>
T Stack<T, i, IT>::pop() {
    LockGuard l(_m);
    if (isEmpty())
        return _data[_num];
    return _data[--_num];
}
template<typename T, unsigned int i, typename IT>
T Stack<T, i, IT>::pop(uint64_t timeout) {
    LockGuard l(_m);
    timeout += millis();
    while (isEmpty() && timeout >= millis()) 
        OS.yield();

    if (isEmpty())
        return _data[_num];
    return _data[--_num];
}
template<typename T, unsigned int i, typename IT>
T Stack<T, i, IT>::top() {
    LockGuard l(_m);
    return _data[_num];
}

#endif // !__DATATYPES_STACK_H__