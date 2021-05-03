
#include <Arduino.h>
//#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"
#include "ArdRTOS.h"

TASK void print_toggle_y() {
    for(;;) {
        digitalWrite(LED_BUILTIN, 1);
        os_delay(250);
        digitalWrite(LED_BUILTIN, 0);
        os_delay(250);
    }
}

void setup() {
    //dbg_start()
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    // put your setup code here, to run once:
    createTask(&print_toggle_y, 50);
    os_begin();
    for(;;);
    print_toggle_y();
}

void loop() {
    // put your main code here, to run repeatedly:
}