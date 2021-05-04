/**
 * @file semaphore.cpp
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief this handles the implementation of the semaphore class
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "data_structures/semaphore.h"
#include "ArdRTOS.h"

Semaphore::Semaphore(): _token(true) {}

bool Semaphore::get(uint16_t timeout) {
    uint32_t time = millis() + timeout;
    while (_token == false) {
        if (millis() > time) 
            return false;
        OS.yield();
    }
    _token = false;
}
bool Semaphore::release(uint16_t timeout) {
    _token = true;
}