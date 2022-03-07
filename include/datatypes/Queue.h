#ifndef __DATATYPES_QUEUE_H__
#define __DATATYPES_QUEUE_H__

/** 
 * this is here so that we can use as little data as possible in this datatype.
 * what happens is that the datatype used to index the queue will always be the smallest 
 * kind of integer that can index the whole queue.
 * 
 * if you only need 10 indexes, the data saved for doing this would be 9 bytes. which if you are saving uint8 values then
 * you have a 37.5% smaller datatype with no change whatsoever in capabilities.
 */
namespace __DATATYPES_QUEUE_HELPER__ {
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
// used to hide the jumble from users
#define __Queue_IT_TYPE__ typename __DATATYPES_QUEUE_HELPER__::Index<(i<UINT8_MAX-1),(i<UINT16_MAX-1)>::Type

/**
 * @brief This is a basic threadsafe container for queueing data 
 * 
 * @tparam T The type of data stored in the Queue
 * @tparam i The maximum number of datapoints in the Queue. defaults to 10
 * @tparam L the type of locking mechanism to use. defaults to Semaphore
 * @tparam IT Generated at compile time. Do not put insert anything into this spot. 
 */
template<typename T, unsigned int i=10, typename L = Semaphore, typename IT = __Queue_IT_TYPE__>
class Queue {
private:
    L _m;                      /** the locking device used to threadsafe the queue */
    T _data[i];                 /** where the data is actually stored */
    IT _front, _back, _count;   /** one of the counters used in opperation */

    /**
     * @brief essencially n++ but wraps around i
     * 
     * @param n the var to inc
     * @return IT returns n before incrementation
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
     * @return false failiure queueing
     */
    bool enqueue(const T inp);
    /**
     * @brief used to enqueue another item at the end of the queue
     * 
     * @param inp The item to push onto the end of the queue
     * @param timeout how long to wait before returning failiure
     * @return true successfull queueing
     * @return false failiure queueing
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