#include <Arduino.h>
#include "data_structures/queue.h"
#include "ArdRTOS.h"

void Queue::getNext(uint8_t &to_inc) {
    to_inc = (to_inc + _is) % _qs;
}

Queue::Queue(const uint16_t queue_size, const uint8_t item_size):
    _front(0),
    _back(0),
    _count(0),
    _qs(queue_size),
    _is(item_size) 
{
    // allocate enough space for this queue!
    _data = (uint8_t*)malloc(queue_size * item_size);
}

bool Queue::enqueue(const uint8_t* data, const uint16_t timeout) {
    uint32_t time = millis() + timeout;
    while (isFull()) {
        if (millis() > time) 
            return false;
        OS.yield();
    }

    memcpy(_data+_front, data, _is);
    getNext(_front);
    _count++;
    return true;
}
bool Queue::dequeue(uint8_t* data, const uint16_t timeout) {
     uint32_t time = millis() + timeout;
    while (isEmpty()) {
        if (millis() > time)
            return false;
        OS.yield();
    }
    memcpy(data, _data+_back, _is);
    getNext(_back);
    _count--;
    return true;
}

bool Queue::empty() {
    _front = _back = _count = 0;
}

bool Queue::isEmpty() {
    return _count == 0;
}
bool Queue::isFull() {
    return _count == _qs;
}

uint8_t Queue::count() {
    return _count;
}