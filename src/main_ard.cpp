//*
#include <Arduino.h>
#include "ArdRTOS.h"
//#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"

uint8_t constexpr LEDS[] = {3, 5, 6};

template<int a>
void loop_toggle() {
    static unsigned long long T = 0;
    for (uint8_t i = 0; i < 200; i++)
    {
        analogWrite(a, i);
        OS.delayUntill(T);
        T += a*3;
    }
}

void Serial_printer() {
    static char i = 0;
    static unsigned long long T = 0;
    Serial.print(i);
    Serial.flush();
    i++;
    OS.delayUntill(T);
    T += 1000;
}

void setup() {
    Serial.begin(115200);

    pinMode(3, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    OS.addTask(Serial_printer, 200, 3);
    OS.addTask(loop_toggle<3>, 40, 1);
    OS.addTask(loop_toggle<5>, 40, 1);
    OS.addTask(loop_toggle<6>, 40, 1);
    
    OS.begin();
    // if everything works correctly, this will never get past OS.begin().
}

void loop() {
    // if everything works correctly, this will never get into loop();
}
//*/