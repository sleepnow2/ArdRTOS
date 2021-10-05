#include <Arduino.h>
#include "ArdRTOS.h"

uint8_t constexpr LEDS[] = {3, 5, 6};
Semaphore test;

// this test is to make sure that semaphores cant double lock.
void SemaphoreTest () {
    digitalWrite(3, 0);
    test.lock();
    test.lock();
    digitalWrite(3, 1);
    test.unlock();
}

void setup() {
    Serial.begin(115200);

    pinMode(3, OUTPUT);

    OS.addTask(SemaphoreTest, 200, 1);
    
    OS.begin();
    // if everything works correctly, this will never get past OS.begin().
}

void loop() {
    // if everything works correctly, this will never get into loop();
}