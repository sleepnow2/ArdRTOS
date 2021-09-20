#ifndef __DATATYPES_MUTEX_H__
#define __DATATYPES_MUTEX_H__

/**
 * @brief this is a basic mutex to use for threadsafeing various resources.
 * 
 */
class Mutex {
private:

    // whether or not the token is available for locking
    // true = _lock available for locking
    // false = _lock taken
    bool _lock;

    // the task that locked the task. this is a utility to prevent a task from getting blocked attempting to re-lock a resource
    uint8_t _locking_task;

public:

    /**
     * @brief Construct a new Mutex object
     * 
     */
    Mutex();

    /**
     * @brief blocks the current task untill a lock can be aquired
     * 
     */
    void lock();

    /**
     * @brief blocks the current task untill a lock is aquired or times out
     * 
     * @param timeout the amout of time before this stops attempting to aquire the lock;
     * @return true lock successfully aquired
     * @return false timed out
     */
    bool lock(uint64_t timeout);

    /**
     * @brief used to fetch the lock if available. does not wait, does not block.
     * 
     * @return true lock fetched
     * @return false lock not available
     */
    bool lockImmediate();

    /**
     * @brief frees the lock
     * 
     */
    void unlock();

    /**
     * @brief returns the availability of the mutex
     * 
     * @return true available
     * @return false not available
     */
    bool available() {return _lock;};
};

/**
 * @brief a basic class to automatically unlock a mutex when it goes out of scope.
 * 
 * does not support Timeout and will block forever if not properly used
 */
class LockGuard {
private:
    // a pointer to the mutex that we are locking and unlocking 
    Mutex* _m;
public:

    /**
     * @brief Construct a new Lock Guard object. This locks the mutex given in the constructor. 
     * This is usefull because the deconstructor unlocks the same mutex when going out of scope
     * 
     * @param m the mutex to lock in the creation of the lock
     */
    LockGuard(Mutex &m) {
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
