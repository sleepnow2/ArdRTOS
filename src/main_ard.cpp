#include <Arduino.h>
#include "ArdRTOS.h"

// the pins on the test board
uint8_t LEDS[] = {3, 5, 6};

void blink(void* l) {
    uint8_t led = *(uint8_t*)l;
    // setup
    pinMode(led, OUTPUT);

    // task main loop
    for(;;){
        digitalWrite(led, 1); delay(led * 150);
        digitalWrite(led, 0); delay(led * 100);
    }
}

void setup() {
    OS.addTask(blink, LEDS);
    OS.addTask(blink, LEDS + 1);
    OS.addTask(blink, LEDS + 2);

    OS.begin();
    // program never gets here
}

void loop() {
    // do nothing
}