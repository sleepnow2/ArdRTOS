#include "../.pio/libdeps/nanoatmega328/avr-debugger/avr8-stub/avr_debugger.h"
#include <Arduino.h>
#include "ArdRTOS.h"

TASK void print_toggle_y() {
    for(;;) {
        digitalWrite(LED_BUILTIN, 1);
        OS.delay(250);
        digitalWrite(LED_BUILTIN, 0);
        OS.delay(250);
    }
}

void setup() {
    dbg_start()
    #if !defined(__DEBUG__)
    Serial.begin(115200);
    Serial.println("go!");
    Serial.flush();
	#endif
    pinMode(LED_BUILTIN, OUTPUT);
    // put your setup code here, to run once:
    OS.createTask(&print_toggle_y, 50);
    //Serial.flush();
    OS.begin(false);
}

void loop() {
    // put your main code here, to run repeatedly:
}