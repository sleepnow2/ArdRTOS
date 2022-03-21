// This demos goals are as follows:
//     Demonstrate SPI communications using both the stock SD card library and the SdFat library
//     Demonstrate listener tasks listening in to other tasks data for 
//        display, logging, or controlling hardware by listening in to pure software tasks. 

/*
#include <Arduino.h>
#define ARDRTOS_NO_WARNINGS
#include "ArdRTOS.h"

// this option is if you are using an exFAT SD card.
//#define SdFat

// this option is if you want to use delayUntill instead of just delay
#define PRECISE_TIME

#ifdef SdFat
    #include <SdFat.h>
#else
    #include <SD.h>
#endif

#define SD_CS_PIN 10
#define LOG_FREQ_MS 1000

// used as a very temporary dump for data
// volatile ensures that memory is written to immediately when read and accessed.
// without volatile, inc would just be cached in the registers of mainTask 
// and would not be accessible outside. 
volatile int inc;

// this semaphore is not nessessary as there is only one task
// using the SPI bus (used by the SD library). However, it is still good practice to 
// lock external resources used by tasks anyways.
Semaphore SPILock;

// below is the option for exFAT sd cards. it uses a different, but funcitonally similar library.
#ifdef SdFat
    SdExFat SD;
#endif

// this task just increments a variable at random.
// this is simply a placeholder for anything you may need.
void mainTask() {
    inc++;
    OS.delay(rand()/(RAND_MAX/100));
}


// listens in to the reading variable and logs it to a log file on the SDcard every second.
void Listener() {
    // open the log file and use this file for future calls to listener.
    #ifdef SdFat
        static ExFatFile log("log.csv", O_WRONLY | O_CREAT);
    #else
        static File log = SD.open("log.csv", FILE_WRITE);
    #endif

    //log the time stamp and the reading value to the SD card.
    Serial.print(millis());
    Serial.print(":");
    Serial.println(inc);
    Serial.flush();

    SPILock.lock();

    char buff[20];
    ultoa(millis(), buff, 10);
    log.write(buff, strlen(buff));
    log.write(',');
    itoa(inc, buff, 10);
    log.write(buff, strlen(buff));
    log.write('\n');
    log.flush();
    // release controll over the 
    SPILock.unlock();

    // wait till next log time.

    #ifdef PRECISE_TIME
        static u32 t = millis();
        t += LOG_FREQ_MS;
        OS.delayUntill(t);
    #else
        OS.delay(LOG_FREQ_MS);
    #endif
}

void setup() {
    Serial.begin(115200);

    // begin SD card communication
    #ifdef SdFat
        Serial.println(SD.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(8))));
    #else
        Serial.println(SD.begin(SD_CS_PIN));
    #endif

    OS.addTask(Listener, 256);
    OS.addTask(mainTask);

    OS.begin();
    // should never reach
}

void loop() {
    // should never reach
}
//*/