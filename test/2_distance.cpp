// The goal of this demo is to demonstrate how to dissable
// ArdRTOS for short amounts of time due to very time sensitive operations.

//*
#include <Arduino.h>
#include "ArdRTOS.h"

//
u16 dist;

void dispnum() {
    // since this is the only task displaying output to serial, a semaphore is not needed
    Serial.println(dist);
    OS.delay(500);
}

// this is the trigger pin for the 
constexpr u8 trigger = 6;
// this is the echo pin for the
constexpr u8 echo = 5;

// this is the multiplier to convert from microseconds to metres in a round trip.
constexpr float SOS = 0.034/2;
void distance() {
    // dissable ArdRTOS as this is a very time sensitive operation
    noInterrupts();
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    // this function is EXTREMELY sensitive to interrupting, so it NEEDS the protection around it.
    dist = pulseIn(echo, HIGH)*SOS;
    
    // reinable interrupts
    interrupts();

    // wait till next sensor reading
    OS.delay(100);
}

void setup() {
    Serial.begin(115200);

    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);

    OS.addTask(dispnum, 32, 1);
    OS.addTask(distance, 128, 1);
    OS.begin();

    // if everything works correctly, this will never get past OS.begin()
}

void loop() {
    // if everything works correctly, this will never get into loop()
}
//*/