#include <Arduino.h>
#include "ArdRTOS.h"
#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"

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
    dbg_init();
    DBG_EXEC(delay(1000);)
    dbg_breakpoint();
    Serial.begin(115200);

    OS.addTask(toggle_1, 50);
    OS.addTask(toggle_2, 50);
    OS.addTask(toggle_3, 50);
    OS.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
}