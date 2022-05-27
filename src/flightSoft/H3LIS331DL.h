#ifndef H3LIS331DL_H
#define H3LIS331DL_H

#define H3LIS331DL_CS 38
#define H3LIS331DL_INT 17
#define H3LIS331DL_SPI_SPEED 1000000

#define H3LIS331DL_REG_WHO_AM_I 0x0F
#define H3LIS331DL_REG_CTRL1 0x20
#define H3LIS331DL_REG_CTRL3 0x22   //interrupt
#define H3LIS331DL_REG_CTRL4 0x23
#define H3LIS331DL_REG_STATUS 0x27
#define H3LIS331DL_REG_OUTX_L 0x28  //inizio di 6 byte per acc X Y Z prima low significant 

#define H3LIS331DL_BIT_2_MSS  97.65 * 9.81 / 1000

#include <SPI.h>
#include <string>
#include <sstream>

typedef struct {
  unsigned long tstp = 0;
  float x,y,z = 0;
  float mod = 0;
  float filt = 0;

  std::string toString(){
    std::ostringstream ss;
    ss << tstp << "," << x << "," << y << "," << z << "," << mod << "," << filt << "\n";
    std::string s(ss.str());
    return s;  
    }
} hf_imu_values;

class H3LIS331DL
{
  public:
    int init();
    hf_imu_values measure();
    hf_imu_values curr = {};
    hf_imu_values prec = {};    
};

#endif
