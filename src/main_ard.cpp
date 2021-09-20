#include <Arduino.h>
#include "ArdRTOS.h"
//#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"

#define demo 0

#if demo == 0
// this demo has three lights flickering... OOO...
template<int a>
void setup_toggle() {
    pinMode(a, OUTPUT);
}

template<int a>
void loop_toggle() {
    for (uint8_t i = 0; i < 200; i++)
    {
        analogWrite(a, i);
        OS.delay(a);
    }
}

Task toggle_1(setup_toggle<5>, loop_toggle<5>);
Task toggle_2(setup_toggle<6>, loop_toggle<6>);
Task toggle_3(setup_toggle<3>, loop_toggle<3>);

void setup() {
    OS.addTask(toggle_1, 50);
    OS.addTask(toggle_2, 50);
    OS.addTask(toggle_3, 50);
    OS.begin();
    // if everything works correctly, this will never get past begin.
}

void loop() {
    // put your main code here, to run repeatedly:
}
#elif demo == 1
// this demo code is designed to show that global vars can be used...

volatile uint8_t intensity = 0;

void led_lp() {
    analogWrite(6,intensity);
}
void btn_lp() {
    pinMode(4,INPUT_PULLUP);
    while (1) {
        if (digitalRead(4) == LOW) {
            intensity++;
        }
        intensity++;
        OS.delay(4);
    }
}

Task led(led_lp);
Task btn(btn_lp);

void setup() {
    OS.addTask(led, 100);
    OS.addTask(btn, 100);
    OS.begin();
}
void loop() {}

#elif demo == 2

Stack<uint8_t, 50> s;

void prod() {
    s.push((uint8_t)rand());
    OS.delay(10);
}

void cons_s() {
    pinMode(3,OUTPUT);
}
void cons() {
    analogWrite(3, s.pop());
    OS.delay(5);
}

void setup() {
    Serial.begin(115200);
    OS.addTask(prod, 100);
    OS.addTask(cons, 100);
    OS.begin();
}

void loop() {

}

#endif