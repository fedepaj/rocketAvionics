#ifndef BMP388_H
#define BMP388_H

#include <SPI.h>

#define BMP388_REG_WHO_AM_I 0x00
#define BMP388_REG_STATUS 0x03
#define BMP388_REG_OUTP 0x04 //primo byte pressione, sono 3
#define BMP388_REG_OUTT 0x07 //primo byte temperatura, sono 3
#define BMP388_REG_INT_CTRL 0x19  //01000010
#define BMP388_REG_PWR_CTRL 0x1B  //00110011
#define BMP388_REG_OSR 0x1C  //00001010  oversampling press 2x  temp 2x
#define BMP388_REG_ODR 0x1D  //00000001  100Hz
#define BMP388_REG_CONFIG 0x1F  //00000010 iir filter coeff 1

typedef struct{
  double par_t1;
  double par_t2;
  double par_t3;
  double par_p1;
  double par_p2;
  double par_p3;
  double par_p4;
  double par_p5;
  double par_p6;
  double par_p7;
  double par_p8;
  double par_p9;
  double par_p10;
  double par_p11;
  double t_lin;
} calibData_t;

typedef struct{
  unsigned long tstp;
  float p, t, altitude;
  float filtAlti;
} altiValues_t;

class BMP388
{
  public:
    BMP388(int csPin, int intPin, int spiSpeed);
    int setup();
    float getP();
    float getT();
    float getAltitude(float p,float t);
    altiValues_t getMeasure(float prec, float filtPrec);
  private:
    const float seaLevelPressure = 101325; // in Pa
    calibData_t calibData;
    uint32_t rawPressure, rawTemp;
    float pressure; //in Pa
    float temp; //in C
    int _csPin;
    int _intPin;
    int _spiSpeed;
    void calib();
    float compensateT(uint32_t uncomp_temp);
    float compensateP(uint32_t uncomp_press);
};

#endif
