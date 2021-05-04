/**
 * @file queue.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief this file handles the API for the Queue class.
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <Arduino.h>

class Queue {
private:
    uint8_t *_data;
    uint8_t _front, _back, _count, _qs, _is;

    void getNext(uint8_t &to_inc);
public:
    Queue(const uint16_t queue_size, const uint8_t item_size);

    bool enqueue(const uint8_t* data, const uint16_t timeout);
    bool dequeue(uint8_t* data, const uint16_t timeout);
    bool empty();

    bool isEmpty();
    bool isFull();
    uint8_t count();
};

#endif // !__QUEUE_H__

