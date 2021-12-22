#include "DSO32.h"

/**********************************************************************************************
 * FUNZIONI PER LA IMU
 */
DSO32::DSO32(int csPin, int intGyroPin, int intAccPin, int spiSpeed){
  _spiSpeed=spiSpeed;
  _intGyroPin=intGyroPin;
  _intAccPin=intAccPin;
  _csPin=csPin;
}

int DSO32::setup(){
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  byte buffero[2] = {DSO32_REG_WHO_AM_I | 0b10000000};
  SPI.transfer(buffero, 2);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();
  delayMicroseconds(500);
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  //pin interrupt sensore
  byte intBuff[3] = {DSO32_REG_INT1_CTRL, 0x02, 0x01}; //indirizzo int1, valore int1 gyro, valore int2 acc
  SPI.transfer(intBuff, 3);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //Data Rates e Full Scale accelerometro e gyro
  byte odrScaleBuff[3] = {DSO32_REG_CTRL1_XL, 0b01110100, 0b01111100}; //vedi CTRL1_XL e CTRL2_G, 833Hz, 32g,acc lpf2 disattivo, 2000dps
  SPI.transfer(odrScaleBuff,3);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //dataReady mask
  byte dRBuff[2] = {DSO32_REG_CTRL4_C, 0b0000000};
  SPI.transfer(dRBuff, 2);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();
  // After the setup we need to do this otherwhise we wont get back any data
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  uint8_t laller[3] = {DSO32_REG_INT1_CTRL | 0b10000000, 0xA2, 0xA1};
  SPI.transfer(laller, 3);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();
  //Serial.print("valori interrupt: ");
  //Serial.print(laller[1], BIN);
  //Serial.print("   ");
  //Serial.println(laller[2], BIN);
  return 0; // the return value of the setup function will be used for error handling
}

gyroValues_t DSO32::getGyro(float prev, float filtPrev){
  gyroValues_t gyroValues;
  byte buff[6];
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  SPI.transfer(DSO32_REG_OUTX_L_G | 0b10000000);
  SPI.transfer(buff,6);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();

  int16_t rawGyroX = buff[1] << 8 | buff[0];
  int16_t rawGyroY = buff[3] << 8 | buff[2];
  int16_t rawGyroZ = buff[5] << 8 | buff[4];
  
  gyroValues.x = rawGyroX * dso32BitToRadSec;
  gyroValues.y = rawGyroY * dso32BitToRadSec;
  gyroValues.z = rawGyroZ * dso32BitToRadSec;
  gyroValues.tstp = millis();
  gyroValues.mod=sqrt(pow(gyroValues.x,2)+pow(gyroValues.y,2)+pow(gyroValues.z,2));
  gyroValues.filtGyro=0.90476*filtPrev+0.04761* gyroValues.mod+0.04761*prev;
  return gyroValues;
}

accValues_t DSO32::getAcc(float prev, float filtPrev){
  accValues_t accValues;
  byte buff[6];
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  SPI.transfer(DSO32_REG_OUTX_L_A | 0b10000000);
  SPI.transfer(buff,6);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();

  int16_t rawAccX = buff[1] << 8 | buff[0];
  int16_t rawAccY = buff[3] << 8 | buff[2];
  int16_t rawAccZ = buff[5] << 8 | buff[4];
  accValues.tstp = millis();
  accValues.x = rawAccX * dso32BitToMss;
  accValues.y = rawAccY * dso32BitToMss;
  accValues.z = rawAccZ * dso32BitToMss;
  accValues.mod=sqrt(pow(accValues.x,2)+pow(accValues.y,2)+pow(accValues.z,2));
  accValues.filtAcc=0.90476*filtPrev+0.04761*accValues.mod+0.04761*prev;
  return accValues;
}
