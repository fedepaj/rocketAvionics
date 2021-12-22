#ifndef DSO32_H
#define DSO32_H

#include <SPI.h>

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

typedef struct {
  unsigned long tstp;
  float x,y,z; //in rad/s
  float mod,filtGyro;
} gyroValues_t;

typedef struct {
  unsigned long tstp;
  float x,y,z; //in m/ss
  float mod,filtAcc;
} accValues_t;

class DSO32
{
  public:
    DSO32(int csPin, int intGyroPin, int intAccPin, int spiSpeed);
    int setup();
    gyroValues_t getGyro(float prev, float filtPrev);
    accValues_t getAcc(float prev, float filtPrev);
  private:
    int _csPin;
    int _intAccPin;
    int _intGyroPin;
    int _spiSpeed;
    //variabili per convertire i valori letti dal sensore in SI, il primo valore delle espressioni dipende dalla scala vedi il datasheet
    const float dso32BitToRadSec =  70 * (PI/180) / 1000; //valido per 2000dps
    const float dso32BitToMss = 0.976 * 9.80665 /1000; //valido per 32g, pare che g a roma sia 9.80353 o 9.80322
    int16_t rawAccX, rawAccY, rawAccZ, rawGyroX, rawGyroY, rawGyroZ;
};

#endif
