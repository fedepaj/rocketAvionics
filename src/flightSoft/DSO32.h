#ifndef DSO32_H
#define DSO32_H

//#define __ACCFILTDEBUG__
//#define __GYROFILTDEBUG__
#ifdef __ACCFILTDEBUG__
#define ACCFILTDEBUG(...) Serial.println(__VA_ARGS__)
#else
#define ACCFILTDEBUG(...)
#endif

#include "settings.h"
#if defined(__DSO32__) || defined(__ISM330__) // Checks if one of the two is defined in the settings file and enables the code, the fastest way I found
#define DSO32_CS 10
#define DSO32_INT_GYRO 40
#define DSO32_INT_ACC 41
#define DSO32_SPI_SPEED 1000000

#include <SPI.h>

#define DATA_RATE 416 //uguale per accelerometro e giroscopio

#define ACC_FS 16 // 2(solo ism330), 4, 8, 16, 32(solo dso32)
#define GYRO_FS 500 //250, 500, 1000. 2000, 4000(solo ism330)

#define DSO32_REG_INT1_CTRL 0x0D
#define DSO32_REG_INT2_CTRL 0x0E
#define DSO32_REG_WHO_AM_I 0x0F
#define DSO32_REG_CTRL1_XL 0x10
#define DSO32_REG_CTRL2_G 0x11
#define DSO32_REG_CTRL3_C 0x12
#define DSO32_REG_CTRL4_C 0x13
#define DSO32_REG_CTRL5_C 0x14
#define DSO32_REG_CTRL6_C 0x15
#define DSO32_REG_CTRL7_C 0x16
#define DSO32_REG_INTERNAL_FREQ_FINE 0x63
#define DSO32_REG_STATUS 0x1E
#define DSO32_REG_OUT_TEMP_L 0x20 //primo byte temperatura, 2 in totale
#define DSO32_REG_OUTX_L_G 0x22 //primo byte giroscopio, 6 in totale 3x2
#define DSO32_REG_OUTX_L_A 0x28 //primo byte accelerometro, 6 in totale 3x2

#if DATA_RATE == 104
#define DATA_RATE_VALUE 0b01000000
#endif
#if DATA_RATE == 208
#define DATA_RATE_VALUE 0b01010000
#endif
#if DATA_RATE == 416
#define DATA_RATE_VALUE 0b01100000
#endif
#if DATA_RATE == 833
#define DATA_RATE_VALUE 0b01110000
#endif
#if DATA_RATE == 1666
#define DATA_RATE_VALUE 0b10000000
#endif
#if DATA_RATE == 3332
#define DATA_RATE_VALUE 0b10010000
#endif
#if DATA_RATE == 6664
#define DATA_RATE_VALUE 0b10100000
#endif
//quando uso ism330
#ifdef __ISM330__

#if ACC_FS == 2
#define ACC_FS_VALUE 0b0000
#define ACC_SENS 0.061
#endif
#if ACC_FS == 4
#define ACC_FS_VALUE 0b1000
#define ACC_SENS 0.122
#endif
#if ACC_FS == 8
#define ACC_FS_VALUE 0b1100
#define ACC_SENS 0.244
#endif
#if ACC_FS == 16
#define ACC_FS_VALUE 0b0100
#define ACC_SENS 0.488
#endif

#endif
//quando uso dso32
#ifdef __DSO32__

#if ACC_FS == 4
#define ACC_FS_VALUE 0b0000
#define ACC_SENS 0.122
#endif
#if ACC_FS == 8
#define ACC_FS_VALUE 0b1000
#define ACC_SENS 0.244
#endif
#if ACC_FS == 16
#define ACC_FS_VALUE 0b1100
#define ACC_SENS 0.488
#endif
#if ACC_FS == 32
#define ACC_FS_VALUE 0b0100
#define ACC_SENS 0.976
#endif

#endif

#if GYRO_FS == 250
#define GYRO_FS_VALUE 0b0000
#define GYRO_SENS 8.75
#endif
#if GYRO_FS == 500
#define GYRO_FS_VALUE 0b0100
#define GYRO_SENS 17.50
#endif
#if GYRO_FS == 1000
#define GYRO_FS_VALUE 0b1000
#define GYRO_SENS 35
#endif
#if GYRO_FS == 2000
#define GYRO_FS_VALUE 0b1100
#define GYRO_SENS 70
#endif
#if GYRO_FS == 4000
#define GYRO_FS_VALUE 0b0001
#define GYRO_SENS 140
#endif

//variabili per convertire i valori letti dal sensore in SI, il primo valore delle espressioni dipende dalla scala vedi il datasheet
#define DSO32_BIT_2_RAD_SEC GYRO_SENS * (PI/180) / 1000; //valido per 2000dps
#define DSO32_BIT_2_MSS ACC_SENS * 9.80665 /1000; //valido per 32g, pare che g a roma sia 9.80353 o 9.80322

#include <SPI.h>
#include <string>
#include <sstream>

typedef struct {
  unsigned long tstp = 0;
  float x,y,z = 0; //in rad/s for gyro and in m/ss for acc
  float mod = 0;
  float filt = 0;
  std::string toString() {
    std::ostringstream ss;
    ss << tstp << "," << x << "," << y << "," << z << "," << mod << "," << filt << "\n";
    std::string s(ss.str());
    return s;
  }
} imu_values;

class DSO32
{
  public:
    int setup();
    imu_values measureGyro();
    imu_values measureAcc();
    imu_values currGyro = {};
    imu_values currAcc = {};
    imu_values precGyro = {};
    imu_values precAcc = {};
  private:
    
};

#endif
#endif
