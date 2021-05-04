/**
 * @file semaphore.h
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief this file defines the api for semaphores
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <Arduino.h>

class Semaphore {
private:
    bool _token;
public:
    Semaphore();

    bool get(uint16_t timeout);
    bool release(uint16_t timeout);
};

#endif // !__SEMAPHORE_H__