/**
 * @file 99_UNIT_TEST.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief test to see if ArdRTOS is working correctly.
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include "ArdRTOS.h"


void reportError(const char* fun, unsigned code) {
    Serial.print("ERROR IN: ");
    Serial.print(fun);
    Serial.print("ERROR CODE: ");
    Serial.println(code);
}

void floatTest1();
void floatTest2();
void lliTest1();
void lliTest2();
void registerTest();

void setup() {
    Serial.begin(115200);

    OS.addTask(floatTest1);
    OS.addTask(floatTest2);
    OS.addTask(lliTest1);
    OS.addTask(lliTest2);
    OS.addTask(registerTest);

    while (!Serial) {
        // wait for serial to boot up before begining the experiments.
        delay(1);
    }
    OS.begin();
}

void floatTest1() {
    volatile float f1 = 1234.56789;
    volatile float f2 = 9876.54321;
    if (abs ((f1+f2) - (1234.56789 + 9876.54321)) > 0.001) {
        reportError("floatTest1", 0);
    }
    OS.yield();
    if (abs ((f1/f2) - (1234.56789 / 9876.54321)) > 0.001) {
        reportError("floatTest1", 1);
    }
    OS.yield();
}

void floatTest2() {
    volatile float f1 = 12340.56789;
    volatile float f2 = 98760.54321;
    if (abs ((f1+f2) - (12340.56789 + 98760.54321)) > 0.001) {
        reportError("floatTest2", 0);
    }
    OS.yield();
    if (abs ((f1+f2) - (12340.56789 + 98760.54321)) > 0.001) {
        reportError("floatTest2", 1);
    }
    OS.yield();
}

void lliTest1() {
    volatile long long l1 = 123456789;
    volatile long long l2 = 987654321;
    if (abs ((l1+l2) - (123456789 + 987654321)) > 0) {
        reportError("lliTest1", 0);
    }
    OS.yield();
    if (abs ((l1/l2) - (123456789 / 987654321)) > 0) {
        reportError("lliTest1", 1);
    }
    OS.yield();
}

void lliTest2() {
    volatile long long l1 = 1234567890;
    volatile long long l2 = 9876543210;
    if (abs ((l1+l2) - (1234567890 + 9876543210)) > 0) {
        reportError("lliTest2", 0);
    }
    OS.yield();
    if (abs ((l1+l2) - (1234567890 + 9876543210)) > 0) {
        reportError("lliTest2", 1);
    }
    OS.yield();
}


void registerTest() {
#ifdef __AVR__
    // untested on AVR
    asm ("\
    ldi r2, 1\
    ldi r3, 2\
    ldi r4, 3\
    ldi r5, 4\
    ldi r6, 5\
    ldi r7, 6\
    ldi r8, 7\
    ");
    OS.yield();
    asm ("\
    subi r2, 1\
    brne L1\
    subi r3, 2\
    brne L1\
    subi r4, 3\
    brne L1\
    subi r5, 4\
    brne L1\
    subi r6, 5\
    brne L1\
    subi r7, 6\
    brne L1\
    subi r8, 7\
    brne L1\
    jmp L2\
    L1:"
    :\
    :);
    reportError("registerTest", 0);
    asm ("\
    L2:\
    ");
    OS.yield();
#else

#endif
}