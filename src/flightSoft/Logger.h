#ifndef Logger_H
#define Logger_H

#define FILE_APPEND 0x08
#include <SD.h>
#include <SdFat.h>
#include <iostream>
#include <string>
#
#include "settings.h"
#include "commons.h"

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO

class Logger{
    public:
        
        void init();
        void save_states(State states[], int len);
        
        #if defined(__DSO32__) || defined(__ISM330__)
        void save_acc(imu_values v[], int len);
        void save_gyro(imu_values v[], int len);
        #endif
        
        #ifdef __BMP388__
        void save_alti(altiValues_t v[], int len);
        #endif
        
    private:
        
        SdFat32 sd;
        bool sd_ok;
        void currFolderNumber();

        template <typename T> void save(T q[], int len, String filename);

        String statesFileName = "states_";
        String gyroFileName = "gyro_";
        String accFileName = "acc_";
        String altiFileName = "alti_";
        String highAccFileName = "highAcc_";
        
        int file_count = 0;
        int folder_count = 0;
        
};
#endif
