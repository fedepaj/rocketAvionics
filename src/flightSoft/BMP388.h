#ifndef BMP388_H
#define BMP388_H
#include "settings.h"

#define BMP388_CS 19
#define BMP388_INT 20
#define BMP388_SPI_SPEED 1000000

#define BMP388_REG_WHO_AM_I 0x00
#define BMP388_REG_STATUS 0x03
#define BMP388_REG_OUTP 0x04 //primo byte pressione, sono 3
#define BMP388_REG_OUTT 0x07 //primo byte temperatura, sono 3
#define BMP388_REG_INT_CTRL 0x19  //01000010
#define BMP388_REG_PWR_CTRL 0x1B  //00110011
#define BMP388_REG_OSR 0x1C  //00001010  oversampling press 2x  temp 2x
#define BMP388_REG_ODR 0x1D  //00000001  100Hz
#define BMP388_REG_CONFIG 0x1F  //00000010 iir filter coeff 1
#define SEA_LEVEL_PRESSURE 101325 // in Pa
#include <SPI.h>
#include <string>
#include <sstream>

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

typedef struct {
  unsigned long tstp = 0;
  float p = 0; 
  float t = 0;
  float altitude = 0;
  float filtAlti = 0;
  float filtVel = 0;
  std::string toString() {
    std::ostringstream ss;
    ss << tstp << "," << p << "," << t << "," << altitude << "," << filtAlti << "\n";
    std::string s(ss.str());
    return s;
  }
} altiValues_t;

class BMP388
{
  public:
    int setup();
    float getP();
    float getT();
    float getAltitude(float p,float t);
    altiValues_t measure(altiValues_t prec);
  private:
    calibData_t calibData;
    uint32_t rawPressure, rawTemp;
    float pressure; //in Pa
    float temp; //in C
    void calib();
    float compensateT(uint32_t uncomp_temp);
    float compensateP(uint32_t uncomp_press);
};

#endif
