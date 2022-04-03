/**
 * @file [DOC] 6_SPI_SD_card.cpp
 * @author Alex Olson (aolson1714@gmail.com)
 * @brief demonstrate interfacing with external hardware
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright MIT Copyright (c) 2022 Alex Olson. All rights reserved. details at bottom of file.
 * 
 *  Purpose:
 *      To demonstrate how to communicate to external hardware through other libraries
 * 
 *  Required knowledge:
 *      Basic c++ programming.
 *   
 *  Required hardware:
 *      1 SD card reader module
 *      1 SD card (tested using exFAT card)
 */

#include <Arduino.h>
#include "ArdRTOS.h"

// this option is if you are using an exFAT SD card.
//#define SdFat

// this option is if you want to use delayUntil instead of just delay
#define PRECISE_TIME

// this option is for whether you have an exFAT SD card or not.
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

// this semaphore is not necessary as there is only one task
// using the SPI bus (used by the SD library). However, it is still good practice to 
// lock external resources used by tasks anyways.
Semaphore SPILock;

// below is the option for exFAT sd cards. it uses a different, but functionally similar library.
#ifdef SdFat
    SdExFat SD;
#endif

void mainTask();
void listener();

void setup() {
    Serial.begin(115200);

    // begin SD card communication
    #ifdef SdFat
        Serial.println(SD.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(8))));
    #else
        Serial.println(SD.begin(SD_CS_PIN));
    #endif

    OS.addTask(listener, 256);
    OS.addTask(mainTask);

    OS.begin();
    // should never reach
}

// this task just increments a variable at random.
// this is simply a placeholder for anything you may need.
void mainTask() {
    inc++;
    OS.delay(rand()/(RAND_MAX/100));
}


// listens in to the reading variable and logs it to a log file on the SDcard every second.
void listener() {
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
        // if you want to be extremely specific about the time you want to wait, then use this method.
        static u32 t = millis();
        t += LOG_FREQ_MS;
        OS.delayUntil(t);
    #else
        // if you want to be simple, then use this method.
        OS.delay(LOG_FREQ_MS);
    #endif
}

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */