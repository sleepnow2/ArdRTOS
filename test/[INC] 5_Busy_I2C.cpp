#include <ArdRTOS.h>
#include <Arduino.h>

#include <Wire.h>

const uint8_t EEPROM_ADDR  = 0b10100000;
const uint8_t EEPROM_PAGE_COUNT = 8;

// absolutely nessessary, since we will have a busy I2C bus with two tasks logging data to an EEPROM on the I2C bus
// and another task to regurgitate the state of the EEPROM to serial.
Semaphore wireLock;

// not nessessary, but good practice to keep external resources locked... 
//      ... even if there is only one task using it.
Semaphore serialLock;

// locks the write position on the EEPROM so double write events dont happen.
Semaphore writePosLock;
// where to write our new data to. the first eight bits is the page, the second eight bits is the address in the page.
volatile uint16_t writePos = 0;

// this takes the entire address space of the EEPROM and pushes it through serial in a nice to view manner. 
void vomitEEPROM() {
    // attempt to fetch the lock for I2C
    wireLock.lock();
    // attempt to fetch the lock for serial within 5 ms.
    if (serialLock.lock(5)) {
        // if the lock is achieved, then begin the vomit.

    } else {
        // if the lock is not achieved, just release the resource and try again later.
        wireLock.unlock();
    }
}

void cleanEEPROM() {}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    OS.begin();
}
void loop(){}