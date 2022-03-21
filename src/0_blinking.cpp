
// The goal of this demo is to demonstrate how to set up basic tasks using ArdRTOS

//*
#include <Arduino.h>
#include "ArdRTOS.h"

uint8_t LEDS[] = {D2, D6, D8};
uint8_t GRNDS[] = {D3, D5, D7};

void blink() {
    uint8_t led = LEDS[0];
    pinMode(led, OUTPUT);

    for(;;){
        digitalWrite(led, 1); 
        OS.delay(led*150);
        digitalWrite(led, 0); 
        OS.delay(led*100);
    }
}
void blink2() {
    uint8_t led = LEDS[1];
    pinMode(led, OUTPUT);

    for(;;){
        digitalWrite(led, 1); 
        OS.delay(led*150);
        digitalWrite(led, 0); 
        OS.delay(led*100);
    }
}
void blink3() {
    uint8_t led = LEDS[2];
    pinMode(led, OUTPUT);

    for(;;){
        digitalWrite(led, 1); 
        OS.delay(led*150);
        digitalWrite(led, 0); 
        OS.delay(led*100);
    }
}


void setup() {
    for (uint8_t i = 0; i < sizeof(GRNDS); i++) {
        pinMode(GRNDS[i], OUTPUT);
        digitalWrite(GRNDS[i], 0); 
    }

    OS.addTask(blink);
    OS.addTask(blink2);
    OS.addTask(blink3);

    OS.begin();
}

void loop() {
}
//*/