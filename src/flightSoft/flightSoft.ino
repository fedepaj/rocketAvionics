#include <LittleFS.h>
#include "SdFat.h"

#include "BMP388.h"
#include "DSO32.h"

#define BMP388_CS 19
#define BMP388_INT 20

#define DSO32_CS 10
#define DSO32_INT_GYRO 40
#define DSO32_INT_ACC 41

#define DSO32_SPI_SPEED 1000000
#define BMP388_SPI_SPEED 1000000
#define BUFF_LEN 1000

#define SD_CONFIG  SdioConfig(FIFO_SDIO)  // Use Teensy SDIO

typedef enum {
  ON_PAD,
  ASCEND,
  APOGEE,
  DESCEND,
  END
} flightState_t;

fligthState_t state = ON_PAD;

BMP388 altimeter(BMP388_CS, BMP388_INT, BMP388_SPI_SPEED);
DSO32 imu(DSO32_CS, DSO32_INT_GYRO, DSO32_INT_ACC, DSO32_SPI_SPEED);

altiValues_t altiBuff[BUFF_LEN];
accValues_t accBuff[BUFF_LEN];
gyroValues_t gyroBuff[BUFF_LEN];

altiBuff_l=0;
accBuff_l=0;
gyroBuff_l=0;

LittleFS_QPINAND myfs;
SdFs sd;

File gyroFile;  // Specifes that dataFile is of File type
File altiFile;
File accFile;

void setup() {
  Serial.begin(512000);
  while(!Serial)
    delay(5);
    
  Serial.print("Initializing LittleFS ...");

  // see if the Flash is present and can be initialized:
  if (!myfs.begin()) {
    Serial.printf("Error starting %s\n", "QSPI FLASH");
    error();
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("LittleFS initialized.");

  if (!sd.begin(SD_CONFIG)) {
    Serial.println("SD initialization failed!");
  }
  else{
    if(!transferLogToSD()){
      Serial.println("Failed to copy files from flash to SD card");
      error();
    }
  }
  
  gyroFile = myfs.open("gyro.bin", FILE_WRITE);
  altiFile = myfs.open("alti.bin", FILE_WRITE);
  accFile = myfs.open("acc.bin", FILE_WRITE);
  
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
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure, RISING);

}

void error(){
  //TODO: Light up error led
  //TODO2: Make macro
  }

int transferLogToSD(){
  gyroFile = myfs.open("gyro.bin", FILE_WRITE);
  altiFile = myfs.open("alti.bin", FILE_WRITE);
  accFile = myfs.open("acc.bin", FILE_WRITE);
  return 0;
}

void loop() {
  if(altiBuff_l >= BUFF_LEN){
    detachInterrupt(digitalPinToInterrupt(BMP388_INT))
    fwrite(altiBuff, sizeof(altiValues_t), altiBuff_l, &altiFile);
    altiBuff_l=0;
    attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure,RISING);
  }
  
  if(gyroBuff_l >= BUFF_LEN){
    detachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO))
    fwrite(gyroBuff, sizeof(gyroValues_t), gyroBuff_l, &gyroFile);
    gyroBuff_l=0;
    attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure,RISING);
  }
  
  if(accBuff_l >= BUFF_LEN){
    detachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC))
    fwrite(accBuff, sizeof(accValues_t), accBuff_l, &accFile);
    accBuff_l=0;
    attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure,RISING);
  }
    
  switch(state){
    case ON_PAD:
      // Accendi led per capire che siamo pronti
      state=ASCEND;
      break;
    case ASCEND:
      state=APOGEE;
      break;
    case APOGEE:
      state=DESCEND;
      break;
    case DESCEND:
      state=END;
      break;
    case END:
      //Remember to save the remain of the buffers bf closing
      //Remember to close the file buffers
      break;
    }
  }

void logData(altiValues_t *buff, File file, int intPin){
  
}

void altimeterMeasure(){
  unsigned long tstp = millis();
  altiBuff[altiBuff_l] = altimeter.getMeasure();
  altiBuff_l++;
  Serial.println("["+String(tstp)+"]" + " alti(t,p,a): " + String(altiValues.t, 2) + ", " + String(altiValues.p, 2) + ", " + String(altiValues.altitude,2));
  //this will return something in a near future  
}

void accMeasure(){
  unsigned long tstp = millis();
  accBuff[accBuff_l] = imu.getAcc();
  accBuff_l++;
  Serial.println("["+String(tstp)+"]" + " acc(x,y,z): " + String(accValues.x, 2) + ", " + String(accValues.y, 2) + ", " + String(accValues.z, 2));
}
  
void gyroMeasure(){
  unsigned long tstp = millis();
  gyroBuff = imu.getGyro();
  gyroBuff_l++;
  Serial.println("["+String(tstp)+"]" + " gyro(x,y,z): " + String(gyroValues.x, 2) + ", " + String(gyroValues.y, 2) + ", " + String(gyroValues.z, 2));
}
