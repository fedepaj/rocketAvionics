#ifndef Logger_H
#define Logger_H

#define RED_LED 8
#define GREEN_LED 28
#include <SD.h>
#include <SdFat.h>
#include <LittleFS.h>
#include <iostream>
#include <string>

#include "data.h"
#include "settings.h"

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO

class Logger{
    public:
        void setup(String filename);
        template <typename T> void save(T q, String filename);
        void save_state(State *state);
        void ready();
        template <typename T>  int transferLogToSD(String filename);
    private:
        LittleFS_QSPI myfs;
        SdFat32 sd;
        
        File dump;
        File state;
        char _dumpFileName[100]; 
        char _stateFileName[100];
        int curr_dump;
};
#endif
