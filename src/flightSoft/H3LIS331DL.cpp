#include "H3LIS331DL"

H3LIS331DL::H3LIS331DL(int csPin, int intAccPin, int spiSpeed){
	_spiSpeed=spiSpeed;
	_intAccPin=intAccPin;
	_csPin=csPin;
}

int H3LIS331DL::setup(){
	SPI1.beginTransaction(SPI1Settings(_spiSpeed, MSBFIRST, SPI1_MODE3));
  digitalWrite(_csPin, LOW);
  byte buffero[2] = {H3LIS331DL_REG_WHO_AM_I | 0b10000000};
  SPI1.transfer(buffero, 2);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  delayMicroseconds(500);
  SPI1.beginTransaction(SPI1Settings(_spiSpeed, MSBFIRST, SPI1_MODE3));
  digitalWrite(_csPin, LOW);
  byte odrBuff[2] = {H3LIS331DL_REG_CTRL1, 0b00111111};
  SPI1.transfer(odrBuff,2)
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //pin interrupt sensore
  byte intBuff[2] = {H3LIS331DL_REG_CTRL3, 0b00000010}; //dataReady su INT1
  SPI1.transfer(intBuff, 2);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_csPin, LOW);
  //Full Scale accelerometro
  byte scaleBuff[2] = {H3LIS331DL_REG_CTRL4, 0b10010000}; //200g
  SPI1.transfer(scaleBuff,3);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  // After the setup we need to do this otherwhise we wont get back any data
  /*SPI1.beginTransaction(SPI1Settings(_spiSpeed, MSBFIRST, SPI1_MODE3));
  digitalWrite(_csPin, LOW);
  uint8_t laller[3] = {DSO32_REG_INT1_CTRL | 0b10000000, 0xA2, 0xA1};
  SPI1.transfer(laller, 3);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();
  return 0;
  */
}

highAccValues_t H3LIS331DL::getAcc(){
  highAccValues_t accValues;
  byte buff[6];
  SPI1.beginTransaction(SPI1Settings(_spiSpeed, MSBFIRST, SPI1_MODE3));
  digitalWrite(_csPin, LOW);
  SPI1.transfer(H3LIS331DL_REG_OUTX_L | 0b10000000);
  SPI1.transfer(buff,6);
  digitalWrite(_csPin, HIGH);
  SPI1.endTransaction();

  int16_t rawAccX = buff[1] << 8 | buff[0];
  int16_t rawAccY = buff[3] << 8 | buff[2];
  int16_t rawAccZ = buff[5] << 8 | buff[4];
  accValues.tstp = millis();
  // The data that comes out is 12-bit data, left justified, so the lower
  //  four bits of the data are always zero. We need to right shift by four,
  //  then typecase the upper data to an integer type so it does a signed
  //  right shift.
  rawAccX = rawAccX >> 4;
  rawAccY = rawAccY >> 4;
  rawAccZ = rawAccZ >> 4;
  accValues.x = rawAccX * h3lis331dlBitToMss;
  accValues.y = rawAccY * h3lis331dlBitToMss;
  accValues.z = rawAccZ * h3lis331dlBitToMss;
  return accValues;
}