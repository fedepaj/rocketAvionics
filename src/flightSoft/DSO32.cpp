#include "DSO32.h"

/**********************************************************************************************
 * FUNZIONI PER LA IMU
 */

int DSO32::setup(){
  pinMode(DSO32_CS, OUTPUT);
  pinMode(DSO32_INT_GYRO, INPUT);
  pinMode(DSO32_INT_ACC, INPUT);

  digitalWrite(DSO32_CS, HIGH);
  SPI.begin();
  
  SPI.beginTransaction(SPISettings(DSO32_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(DSO32_CS, LOW);
  byte buffero[2] = {DSO32_REG_WHO_AM_I | 0b10000000};
  SPI.transfer(buffero, 2);
  digitalWrite(DSO32_CS, HIGH);
  SPI.endTransaction();
  delayMicroseconds(500);
  SPI.beginTransaction(SPISettings(DSO32_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(DSO32_CS, LOW);
  //pin interrupt sensore
  byte intBuff[3] = {DSO32_REG_INT1_CTRL, 0x02, 0x01}; //indirizzo int1, valore int1 gyro, valore int2 acc
  SPI.transfer(intBuff, 3);
  digitalWrite(DSO32_CS, HIGH);
  digitalWrite(DSO32_CS, LOW);
  //Data Rates e Full Scale accelerometro e gyro
  byte odrScaleBuff[3] = {DSO32_REG_CTRL1_XL, 0b01110100, 0b01111100}; //vedi CTRL1_XL e CTRL2_G, 833Hz, 32g,acc lpf2 disattivo, 2000dps
  SPI.transfer(odrScaleBuff,3);
  digitalWrite(DSO32_CS, HIGH);
  digitalWrite(DSO32_CS, LOW);
  //dataReady mask
  byte dRBuff[2] = {DSO32_REG_CTRL4_C, 0b0000000};
  SPI.transfer(dRBuff, 2);
  digitalWrite(DSO32_CS, HIGH);
  SPI.endTransaction();
  // After the setup we need to do this otherwhise we wont get back any data
  SPI.beginTransaction(SPISettings(DSO32_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(DSO32_CS, LOW);
  uint8_t laller[3] = {DSO32_REG_INT1_CTRL | 0b10000000, 0xA2, 0xA1};
  SPI.transfer(laller, 3);
  digitalWrite(DSO32_CS, HIGH);
  SPI.endTransaction();
  //Serial.print("valori interrupt: ");
  //Serial.print(laller[1], BIN);
  //Serial.print("   ");
  //Serial.println(laller[2], BIN);
  return 0; // the return value of the setup function will be used for error handling
}

imu_values DSO32::measureGyro(imu_values prec){
  imu_values values;
  byte buff[6];
  SPI.beginTransaction(SPISettings(DSO32_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(DSO32_CS, LOW);
  SPI.transfer(DSO32_REG_OUTX_L_G | 0b10000000);
  SPI.transfer(buff,6);
  digitalWrite(DSO32_CS, HIGH);
  SPI.endTransaction();

  int16_t rawGyroX = buff[1] << 8 | buff[0];
  int16_t rawGyroY = buff[3] << 8 | buff[2];
  int16_t rawGyroZ = buff[5] << 8 | buff[4];
  
  values.x = rawGyroX * DSO32_BIT_2_RAD_SEC;
  values.y = rawGyroY * DSO32_BIT_2_RAD_SEC;
  values.z = rawGyroZ * DSO32_BIT_2_RAD_SEC;
  values.tstp = millis();
  values.mod=sqrt(pow(values.x,2)+pow(values.y,2)+pow(values.z,2));
  float R=1.0;
  float Q=1e-05;
  float p=1.0;

  //filtro di Kalman
  float Pp=p+Q;
  //xp=x;
  float K=Pp/(Pp+R);
  float e=values.mod-prec.filt;
  p=(1-K)*Pp;
  values.filt=prec.filt+K*e;
  return values;
//  gyroValues.filtGyro=0.90476*prec.filtAcc+0.04761* gyroValues.mod+0.04761*prev;
}

imu_values DSO32::measureAcc(imu_values prec){
  imu_values values;
  byte buff[6];
  SPI.beginTransaction(SPISettings(DSO32_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(DSO32_CS, LOW);
  SPI.transfer(DSO32_REG_OUTX_L_A | 0b10000000);
  SPI.transfer(buff,6);
  digitalWrite(DSO32_CS, HIGH);
  SPI.endTransaction();

  int16_t rawAccX = buff[1] << 8 | buff[0];
  int16_t rawAccY = buff[3] << 8 | buff[2];
  int16_t rawAccZ = buff[5] << 8 | buff[4];
  values.tstp = millis();
  values.x = rawAccX * DSO32_BIT_2_MSS;
  values.y = rawAccY * DSO32_BIT_2_MSS;
  values.z = rawAccZ * DSO32_BIT_2_MSS;
  values.mod=sqrt(pow(values.x,2)+pow(values.y,2)+pow(values.z,2));
  float R=1.0;
  float Q=1e-05;
  float p=1.0;

  //filtro di Kalman
  float Pp=p+Q;
  //xp=x;
  float K=Pp/(Pp+R);
  float e=values.mod-prec.filt;
  p=(1-K)*Pp;
  values.filt=prec.filt+K*e;
  //accValues.filtAcc=0.90476*filtPrev+0.04761*accValues.mod+0.04761*prev;
  return values;
}
