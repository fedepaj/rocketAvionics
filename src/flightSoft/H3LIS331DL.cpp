#include "H3LIS331DL.h"

H3LIS331DL::H3LIS331DL(int csPin, int intAccPin, int spiSpeed){
  _spiSpeed=spiSpeed;
  _intAccPin=intAccPin;
  _csPin=csPin;
}

int H3LIS331DL::setup(){
  SPI1.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  byte buffero[2] = {H3LIS331DL_REG_WHO_AM_I | 0b11000000};
  SPI1.transfer(buffero, 2);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  Serial.println(buffero[1]); //deve dare 50
  delayMicroseconds(500);
  SPI1.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  byte odrBuff[2] = {H3LIS331DL_REG_CTRL1, 0b00111111};
  SPI1.transfer(odrBuff,2);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //pin interrupt sensore
  byte intBuff[2] = {H3LIS331DL_REG_CTRL3, 0b00010000}; //dataReady su INT2
  SPI1.transfer(intBuff, 2);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //Full Scale accelerometro
  byte scaleBuff[2] = {H3LIS331DL_REG_CTRL4, 0b10010000}; //200g
  SPI1.transfer(scaleBuff,3);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  
  SPI1.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  uint8_t laller[3] = {H3LIS331DL_REG_CTRL3 | 0b11000000, 0xA2, 0xA1};
  SPI1.transfer(laller, 3);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  Serial.println(laller[2]);
// After the setup we need to do this otherwhise we wont get back any data
/*
  uint8_t buff[6];
  SPI1.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  SPI1.transfer(H3LIS331DL_REG_OUTX_L | 0b11000000);
  SPI1.transfer(buff,6);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  */
  return (int16_t)laller[1];
}

highAccValues_t H3LIS331DL::getAcc(){
  highAccValues_t accValues;
  uint8_t buff[6];
  SPI1.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csPin, LOW);
  SPI1.transfer(H3LIS331DL_REG_OUTX_L | 0b11000000);
  SPI1.transfer(buff,6);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();

//  int16_t rawAccX = buff[0];
//  rawAccX |= ((uint16_t)buff[1]) << 8;
//  int16_t rawAccY = buff[2];
//  rawAccY |= ((uint16_t)buff[3]) << 8;
//  int16_t rawAccZ = buff[4];
//  rawAccZ |= ((uint16_t)buff[5]) << 8;
  int16_t rawAccX = buff[0] | buff[1]<<8;
  int16_t rawAccY = buff[2] | buff[3]<<8;
  int16_t rawAccZ = buff[4] | buff[5]<<8;
  accValues.tstp = millis();
  // The data that comes out is 12-bit data, left justified, so the lower
  //  four bits of the data are always zero. We need to right shift by four,
  //  then typecase the upper data to an integer type so it does a signed
  //  right shift.
  rawAccX >>= 4;
  rawAccY >>= 4;
  rawAccZ >>= 4;
  //Serial.println(rawAccX);
  accValues.x = ((float)rawAccX * h3lis331dlBitToMss);
  accValues.y = ((float)rawAccY * h3lis331dlBitToMss);
  accValues.z = ((float)rawAccZ * h3lis331dlBitToMss);
//  Serial.print(buff[1]);
//  Serial.print(" , ");
//  Serial.println(buff[0]);
//  Serial.print(buff[3]);
//  Serial.print(" , ");
//  Serial.println(buff[2]);
  return accValues;
}
