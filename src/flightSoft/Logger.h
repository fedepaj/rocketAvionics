#ifndef Logger_H
#define Logger_H

#include <SD.h>
#include <SdFat.h>
#include <LittleFS.h>

#define BMP388_CS 19
#define BMP388_INT 20

#define DSO32_CS 10
#define DSO32_INT_GYRO 40
#define DSO32_INT_ACC 41

#define DSO32_SPI_SPEED 1000000
#define BMP388_SPI_SPEED 1000000
#define BUFF_LEN 1000

#define H3LIS331DL_CS 38
#define H3LIS331DL_INT 17
#define H3LIS331DL_SPI_SPEED 1000000

#include "BMP388.h"
#include "DSO32.h"
#include "H3LIS331DL.h"

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO

#define RED_LED 8
#define GREEN_LED 28

class Logger{
    public:
        Logger() : altimeter(BMP388_CS, BMP388_INT, BMP388_SPI_SPEED), imu(DSO32_CS, DSO32_INT_GYRO, DSO32_INT_ACC, DSO32_SPI_SPEED), accelerometer(H3LIS331DL_CS, H3LIS331DL_INT, H3LIS331DL_SPI_SPEED) {}
        void init();
        void save();

        void altimeterMeasure();
        void accMeasure();
        void gyroMeasure();
        void highAccMeasure();
    private:
        int transferLogToSD();
        BMP388 altimeter;
        DSO32 imu;
        H3LIS331DL accelerometer;

        highAccValues_t highAccBuff[BUFF_LEN];
        altiValues_t altiBuff[BUFF_LEN];
        accValues_t accBuff[BUFF_LEN];
        gyroValues_t gyroBuff[BUFF_LEN];

        int highAccBuff_l=0;
        int altiBuff_l=0;
        int accBuff_l=0;
        int gyroBuff_l=0;
        
        LittleFS_QPINAND myfs;
        SdFat32 sd;
        
        File gyroFile;  // Specifes that dataFile is of File type
        File altiFile;
        File accFile;
        File highAccFile;
        char gyroFileName[100]; 
        char altiFileName[100];
        char accFileName[100];
        char highAccFileName[10];
        bool firstG;
        bool firstA;
        bool firstAl;
        bool f1;
        bool f2;
        bool f3;
};
#endif
