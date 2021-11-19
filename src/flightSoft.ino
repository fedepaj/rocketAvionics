//#include <SPI.h>

#include "BMP388.h"
#include "DSO32.h"

#define BMP388_CS 19
#define BMP388_INT 20

#define DSO32_CS 10
#define DSO32_INT_GYRO 40
#define DSO32_INT_ACC 41

//#define SPI_SCK 13
//#define SPI_MISO 12
//#define SPI_MOSI 11

#define DSO32_SPI_SPEED 1000000
#define BMP388_SPI_SPEED 1000000

BMP388 altimeter(BMP388_CS, BMP388_INT, BMP388_SPI_SPEED);
DSO32 imu(DSO32_CS, DSO32_INT_GYRO, DSO32_INT_ACC, DSO32_SPI_SPEED);

void setup() {
  Serial.begin(512000);
  while(!Serial)
    delay(5);

  pinMode(DSO32_CS, OUTPUT);
  pinMode(DSO32_INT_GYRO, INPUT);
  pinMode(DSO32_INT_ACC, INPUT);
  pinMode(BMP388_CS, OUTPUT);
  pinMode(BMP388_INT, INPUT);
  digitalWrite(DSO32_CS, HIGH);
  digitalWrite(BMP388_CS, HIGH);
 
  SPI.begin();
  
  imu.setup();
  altimeter.setup();

  //attachInterrupts
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), printAccMeasure,RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), printGyroMeasure,RISING);
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), printAltimeterMeasure,RISING);

}

void loop() {}

void printAltimeterMeasure(){
  unsigned long tstp = millis();
  altiValues_t altiValues = altimeter.getMeasure();
  Serial.println("["+String(tstp)+"]" + " alti(t,p,a): " + String(altiValues.t, 2) + ", " + String(altiValues.p, 2) + ", " + String(altiValues.altitude,2));
  //this will return something in a near future  
}

void printAccMeasure(){
  unsigned long tstp = millis();
  accValues_t accValues = imu.getAcc();
  Serial.println("["+String(tstp)+"]" + " acc(x,y,z): " + String(accValues.x, 2) + ", " + String(accValues.y, 2) + ", " + String(accValues.z, 2));
}
  
void printGyroMeasure(){
  unsigned long tstp = millis();
  gyroValues_t gyroValues = imu.getGyro();
  Serial.println("["+String(tstp)+"]" + " gyro(x,y,z): " + String(gyroValues.x, 2) + ", " + String(gyroValues.y, 2) + ", " + String(gyroValues.z, 2));
}
