#include <SD.h>
#include <SdFat.h>
#include <LittleFS.h>

#include "BMP388.h"
#include "DSO32.h"

#define BMP388_CS 19
#define BMP388_INT 20

#define DSO32_CS 10
#define DSO32_INT_GYRO 40
#define DSO32_INT_ACC 41

#define DSO32_SPI_SPEED 1000000
#define BMP388_SPI_SPEED 1000000
#define BUFF_LEN 8000

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO

#define RED_LED 8
#define GREEN_LED 28

typedef enum {
  ON_PAD,
  ASCEND,
  APOGEE,
  DESCEND,
  END
} flightState_t;

flightState_t state = ON_PAD;

BMP388 altimeter(BMP388_CS, BMP388_INT, BMP388_SPI_SPEED);
DSO32 imu(DSO32_CS, DSO32_INT_GYRO, DSO32_INT_ACC, DSO32_SPI_SPEED);

altiValues_t altiBuff[BUFF_LEN];
accValues_t accBuff[BUFF_LEN];
gyroValues_t gyroBuff[BUFF_LEN];

int altiBuff_l=0;
int accBuff_l=0;
int gyroBuff_l=0;

LittleFS_QPINAND myfs;
SdFat32 sd;

File gyroFile;  // Specifes that dataFile is of File type
File altiFile;
File accFile;

void setup() {
  Serial.begin(512000);
  
  // see if the Flash is present and can be initialized:
  if (!myfs.begin()) {
    Serial.println("Error starting QSPI FLASH");
    //error();
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("LittleFS initialized.");

  if (!sd.begin(SD_CONFIG)) {
    Serial.println("SD initialization failed!");
    while (1) {
      // Error, so don't do anything more - stay stuck here
       }
  }
  Serial.println("SD initialized.");

  if(!transferLogToSD()){
    Serial.println("Failed to copy files from flash to SD card");
    while (1) {
      // Error, so don't do anything more - stay stuck here
     }
  }
  
  pinMode(DSO32_CS, OUTPUT);
  pinMode(DSO32_INT_GYRO, INPUT);
  pinMode(DSO32_INT_ACC, INPUT);
  pinMode(BMP388_CS, OUTPUT);
  pinMode(BMP388_INT, INPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  digitalWrite(DSO32_CS, HIGH);
  digitalWrite(BMP388_CS, HIGH);
  Serial.println("Pins setted up.");
  SPI.begin();
  imu.setup();
  altimeter.setup();
  Serial.println("Sensors setted up.");
  //attachInterrupts
  
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure, RISING);
  Serial.println("Interrupts setted up.");
  Serial.println("---");
  
  digitalWrite(GREEN_LED,HIGH);
  
}

int transferLogToSD(){
  int c=0;
  String folderName = "Experiment_";
  while (sd.exists(folderName+String(c))) c++;
  if (!sd.mkdir(folderName+String(c))) {
    Serial.println("Folder cration failed!");
    while(1){}
  }
  if (!sd.chdir(folderName+String(c))) {
    Serial.println("Chdir failed.");
    while(1){}
  }
  
  Serial.println("We are now in folder "+folderName+String(c));
  File in;
  File32 ex;
  
  /*****GYRO*****/
  in = myfs.open("gyro.bin", FILE_READ);
  
  gyroValues_t gyroB;
  if (!ex.open("gyro.csv", O_WRONLY | O_CREAT)) {
    Serial.println("Gyro csv file creation failed!");
  }
  Serial.println("Copying gyro from SPI to SD");
  digitalWrite(RED_LED,HIGH);
  ex.print("tstp,x,y,z\n");
  while (in.available()) {
    in.read((byte *)&gyroB, sizeof(gyroValues_t)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    ex.print(String(gyroB.tstp)+", "+String(gyroB.x)+", "+String(gyroB.y)+", "+String(gyroB.z)+"\n");
  }
  Serial.println("Gyroscope measures are now on the sd card!");
  digitalWrite(RED_LED,LOW);
  in.close();
  ex.close();
  /*****END GYRO*****/
  
  /*****ALTI*****/
  in = myfs.open("alti.bin", FILE_READ);
  
  altiValues_t altiB;
  if (!ex.open("alti.csv", O_WRONLY | O_CREAT)) {
    //error("create Folder1/file1.txt failed");
  }
  digitalWrite(RED_LED,HIGH);
  Serial.println("Copying alti from SPI to SD");
  ex.print("tstp,t,p,alt\n");
  while (in.available()) {
    in.read((byte *)&altiB, sizeof(altiValues_t)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1  
    ex.print(String(altiB.tstp)+", "+String(altiB.t)+", "+String(altiB.p)+", "+String(altiB.altitude)+"\n");
  }
  Serial.println("Altimeter measures are now on the sd card!");
  digitalWrite(RED_LED,LOW);
  in.close();
  ex.close();
  /*****END ALTI*****/
  
  /*****ACC*****/
  in = myfs.open("acc.bin", FILE_READ);
  
  accValues_t accB;
  if (!ex.open("acc.csv", O_WRONLY | O_CREAT)) {
    //error("create Folder1/file1.txt failed");
  }
  digitalWrite(RED_LED,HIGH);
  Serial.println("Copying acc from SPI to SD");
  ex.print("tstp,x,y,z\n");
  while (in.available()) {
    in.read((byte *)&accB, sizeof(accValues_t)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    ex.print(String(accB.tstp)+", "+String(accB.x)+", "+String(accB.y)+", "+String(accB.z)+"\n");
  }
    Serial.println("Accelerometer measures are now on the sd card!");
  digitalWrite(RED_LED,LOW);
  in.close();
  ex.close();
  /*****END ACC*****/
  return 1;
}

void loop() {
  if(altiBuff_l >= (int)(0.7*BUFF_LEN)){
    digitalWrite(RED_LED,HIGH);
    altiFile = myfs.open("alti.bin", FILE_WRITE);
    //detachInterrupt(digitalPinToInterrupt(BMP388_INT));
    altiFile.write((byte *)&altiBuff, sizeof(altiValues_t)*altiBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    altiBuff_l=0;
    Serial.println("Saving alti data");
    altiFile.close();
    digitalWrite(RED_LED,LOW);
    //attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure,RISING);
  }
  
  if(gyroBuff_l >= (int)(0.7*BUFF_LEN)){
    digitalWrite(RED_LED,HIGH);
    gyroFile = myfs.open("gyro.bin", FILE_WRITE);
    //detachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO));
    gyroFile.write((byte *)&gyroBuff, sizeof(gyroValues_t)*gyroBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    gyroBuff_l=0;
    Serial.println("Saving gyro data");
    gyroFile.close();
    digitalWrite(RED_LED,LOW);
    //attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure, RISING);
  }
  
  if(accBuff_l >= (int)(0.7*BUFF_LEN)){
    digitalWrite(RED_LED,HIGH);
    accFile = myfs.open("acc.bin", FILE_WRITE);
    //detachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC));
    accFile.write((byte *)&accBuff, sizeof(accValues_t)*accBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    accBuff_l=0;
    Serial.println("Saving acc data");
    accFile.close();
    digitalWrite(RED_LED,LOW);
    //attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure, RISING);
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

void altimeterMeasure(){
  altiBuff[altiBuff_l] = altimeter.getMeasure();
  //Serial.println("["+String(altiBuff[altiBuff_l].tstp)+"]" + " alti(t,p,a): " + String(altiBuff[altiBuff_l].t, 2) + ", " + String(altiBuff[altiBuff_l].p, 2) + ", " + String(altiBuff[altiBuff_l].altitude,2));
  altiBuff_l++;
  //this will return something in a near future  
}

void accMeasure(){
  accBuff[accBuff_l] = imu.getAcc();
  //Serial.println("["+String(altiBuff[altiBuff_l].tstp)+"]" + " acc(x,y,z): " + String(accBuff[accBuff_l].x, 2) + ", " + String(accBuff[accBuff_l].y, 2) + ", " + String(accBuff[accBuff_l].z, 2));
  accBuff_l++;
}
  
void gyroMeasure(){
  gyroBuff[gyroBuff_l] = imu.getGyro();
  //Serial.println("["+String(altiBuff[altiBuff_l].tstp)+"]" + " gyro(x,y,z): " + String(gyroBuff[gyroBuff_l].x, 2) + ", " + String(gyroBuff[gyroBuff_l].y, 2) + ", " + String(gyroBuff[gyroBuff_l].z, 2));
  gyroBuff_l++;
}
