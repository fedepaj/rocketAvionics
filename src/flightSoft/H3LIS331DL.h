#ifndef H3LIS331DL_H
#define H3LIS331DL_H

#include <SPI.h>

#define H3LIS331DL_REG_WHO_AM_I 0x0F
#define H3LIS331DL_REG_CTRL1 0x20
#define H3LIS331DL_REG_CTRL3 0x22   //interrupt
#define H3LIS331DL_REG_CTRL4 0x23
#define H3LIS331DL_REG_STATUS 0x27
#define H3LIS331DL_REG_OUTX_L 0x28  //inizio di 6 byte per acc X Y Z prima low significant 


typedef struct {
  unsigned long tstp;
  float x,y,z;
} highAccValues_t;

class H3LIS331DL
{
  public:
    H3LIS331DL(int csPin, int intAccPin, int spiSpeed);
    int setup();
    highAccValues_t getAcc();
  private:
    int _csPin;
    int _intAccPin;
    int _spiSpeed;
    const float h3lis331dlBitToMss = 97.65 * 9.81 / 1000;
    int16_t rawAccX, rawAccY, rawAccZ;
};

#endif
