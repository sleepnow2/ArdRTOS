#include <Arduino.h>
#include "ArdRTOS.h"

uint8_t LEDS[] = {3, 5, 6};
uint8_t BTN = 4;

void blink(void* l) {
    pinMode(*(uint8_t*)l, OUTPUT);
    bool b = true;

    while (1) {
        digitalWrite (*(uint8_t*)l, b);
        b = !b;
        OS.delay((*(uint8_t*)l)*200);
    }
}

void blink6(void) {
    pinMode(6, OUTPUT);
    bool b = true;

    while (1) {
        digitalWrite (6, b);
        b = !b;
        OS.delay(6*200);
    }
}

void setup() {
    OS.addTask(blink, LEDS);
    OS.addTask(blink, LEDS+1);
    OS.addTask(blink6);

    OS.beginCooporative();
}

void loop() {/*unused*/}