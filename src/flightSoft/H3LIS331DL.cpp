#include "H3LIS331DL.h"

int H3LIS331DL::init(){
  pinMode(H3LIS331DL_CS, OUTPUT);
  pinMode(H3LIS331DL_INT, INPUT);

  digitalWrite(H3LIS331DL_CS, HIGH);
  SPI1.setMISO(39);
  SPI1.begin();
  
  SPI1.beginTransaction(SPISettings(H3LIS331DL_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(H3LIS331DL_CS, LOW);
  byte buffero[2] = {H3LIS331DL_REG_WHO_AM_I | 0b11000000};
  SPI1.transfer(buffero, 2);
  digitalWrite(H3LIS331DL_CS, HIGH);
  SPI1.endTransaction();
  Serial.println(buffero[1]); //deve dare 50
  delayMicroseconds(500);
  SPI1.beginTransaction(SPISettings(H3LIS331DL_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(H3LIS331DL_CS, LOW);
  byte odrBuff[2] = {H3LIS331DL_REG_CTRL1, 0b00111111};
  SPI1.transfer(odrBuff,2);
  digitalWrite(H3LIS331DL_CS, HIGH);
  digitalWrite(H3LIS331DL_CS, LOW);
  //pin interrupt sensore
  byte intBuff[2] = {H3LIS331DL_REG_CTRL3, 0b00000010}; //dataReady su INT1
  SPI1.transfer(intBuff, 2);
  digitalWrite(H3LIS331DL_CS, HIGH);
  digitalWrite(H3LIS331DL_CS, LOW);
  //Full Scale accelerometro
  byte scaleBuff[2] = {H3LIS331DL_REG_CTRL4, 0b10010000}; //200g
  SPI1.transfer(scaleBuff,3);
  digitalWrite(H3LIS331DL_CS, HIGH);
  SPI1.endTransaction();
  
  SPI1.beginTransaction(SPISettings(H3LIS331DL_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(H3LIS331DL_CS, LOW);
  uint8_t laller[3] = {H3LIS331DL_REG_CTRL3 | 0b11000000, 0xA2, 0xA1};
  SPI1.transfer(laller, 3);
  digitalWrite(H3LIS331DL_CS, HIGH);
  SPI1.endTransaction();
  Serial.println(laller[1]);
// After the setup we need to do this otherwhise we wont get back any data
/*
  uint8_t buff[6];
  SPI1.beginTransaction(SPISettings(H3LIS331DL_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(H3LIS331DL_CS, LOW);
  SPI1.transfer(H3LIS331DL_REG_OUTX_L | 0b11000000);
  SPI1.transfer(buff,6);
  digitalWrite(H3LIS331DL_CS, HIGH);
  SPI1.endTransaction();
  */
  return (int16_t)laller[1];
}

hf_imu_values H3LIS331DL::measure(){
  prec = curr;
  hf_imu_values values;
  uint8_t buff[6];
  SPI1.beginTransaction(SPISettings(H3LIS331DL_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(H3LIS331DL_CS, LOW);
  SPI1.transfer(H3LIS331DL_REG_OUTX_L | 0b11000000);
  SPI1.transfer(buff,6);
  digitalWrite(H3LIS331DL_CS, HIGH);
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
  values.tstp = millis();
  // The data that comes out is 12-bit data, left justified, so the lower
  //  four bits of the data are always zero. We need to right shift by four,
  //  then typecase the upper data to an integer type so it does a signed
  //  right shift.
  rawAccX >>= 4;
  rawAccY >>= 4;
  rawAccZ >>= 4;
  //Serial.println(rawAccX);
  values.x = ((float)rawAccX * H3LIS331DL_BIT_2_MSS);
  values.y = ((float)rawAccY * H3LIS331DL_BIT_2_MSS);
  values.z = ((float)rawAccZ * H3LIS331DL_BIT_2_MSS);
//  Serial.print(values.x);
//  Serial.print(" , ");
//  Serial.print(values.y);
//  Serial.print(" , ");
//  Serial.println(values.z);
  float module = 0;
  module = values.x*values.x + values.y*values.y + values.z*values.z;
  module = sqrt(module);
  Serial.println(module);
  curr = values;
  return values;
}
