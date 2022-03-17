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
        
        void setup();
        void ready();
        
        void save_states(State *state, int len);
        
        #if defined(__DSO32__) || defined(__ISM330__)
        void save_acc(imu_values *v, int len);
        void save_gyro(imu_values *v, int len);
        #endif
        
        #ifdef __BMP388__
        void save_alti(altiValues_t *v, int len);
        #endif
        
        int transferLogsToSD();
        
    private:
        #if defined(__DSO32__) || defined(__ISM330__)
        String f_imu_acc_name = String("imu_acc");
        int f_imu_acc_cd=-1;
        String f_imu_gyro_name = String("imu_gyro");
        int f_imu_gyro_cd=-1;
        #endif

        #ifdef __BMP388__
        String f_alti_name = String("alti");
        int f_alti_cd=-1;
        #endif
        
        String f_states_name = String("states");
        int f_states_cd=-1;
        
        LittleFS_QSPI myfs;
        SdFat32 sd;

        void cdToNewFolder();
        String currDump(String filename, int *prec_dump);
        template <typename T> int transferLogToSD(File file);
        
        template <typename T> void save(T *q, int len, String filename);
};
#endif
