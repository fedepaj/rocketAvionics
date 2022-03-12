#include "Logger.h"
//
int Logger::transferLogToSD(){
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
  return 0;
  
//  Serial.println("We are now in folder "+folderName+String(c));
//  File in;
//  File32 ex;
//  /*****GYRO*****/
//    int count=0;
//    char b[100];
//    String fname="dump_"+String(count)+".bin";
//    fname.toCharArray(b, fname.length());
//    while(in=myfs.open(b, FILE_READ)){
//    
//    
//      Buffers buffers;
//      fname = "dump_"+String(count)+".csv";
//      fname.toCharArray(b, fname.length());
//      if (!ex.open(b, O_WRONLY | O_CREAT)) {
//        Serial.println("Gyro csv file creation failed!");
//      }
//      Serial.println("Copying gyro from SPI to SD");
//      digitalWrite(RED_LED,HIGH);
//      ex.print("tstp,x,y,z\n");
//      while (in.available()) {
//          in.read((byte *)&buffers, sizeof(buffers)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//          ex.print(String(gyroB.tstp)+", "+String(gyroB.x)+", "+String(gyroB.y)+", "+String(gyroB.z)+"\n");
//      }
//      Serial.println("Gyroscope measures are now on the sd card!");
//      digitalWrite(RED_LED,LOW);
//      in.close();
//      ex.close();
//    count++;
//    fname="gyro_"+String(count)+".bin";
//    fname.toCharArray(b, fname.length());
//  }
  /*****END GYRO*****/
}
//  /*****ALTI*****/
//  count=0;
//  fname="alti_"+String(count)+".bin";
//  fname.toCharArray(b, fname.length());
//    while(in=myfs.open(b, FILE_READ)){
//  
//  
//  altiValues_t altiB;
//  fname = "alti_"+String(count)+".csv";
//  fname.toCharArray(b, fname.length());
//  if (!ex.open(b, O_WRONLY | O_CREAT)) {
//    //error("create Folder1/file1.txt failed");
//  }
//  digitalWrite(RED_LED,HIGH);
//  Serial.println("Copying alti from SPI to SD");
//  ex.print("tstp,t,p,alt\n");
//  while (in.available()) {
//    in.read((byte *)&altiB, sizeof(altiValues_t)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1  
//    ex.print(String(altiB.tstp)+", "+String(altiB.t)+", "+String(altiB.p)+", "+String(altiB.altitude)+"\n");
//  }
//  Serial.println("Altimeter measures are now on the sd card!");
//  digitalWrite(RED_LED,LOW);
//  in.close();
//  ex.close();
//  count++;
//  fname="alti_"+String(count)+".bin";
//  fname.toCharArray(b, fname.length());
//  }
//  /*****END ALTI*****/
//  
//  /*****ACC*****/
//  count=0;
//  fname="acc_"+String(count)+".bin";
//  fname.toCharArray(b, fname.length());
//    while(in=myfs.open(b, FILE_READ)){
//  
//  accValues_t accB;
//  fname = "acc_"+String(count)+".csv";
//  fname.toCharArray(b, fname.length());
//  if (!ex.open(b, O_WRONLY | O_CREAT)) {
//    //error("create Folder1/file1.txt failed");
//  }
//  digitalWrite(RED_LED,HIGH);
//  Serial.println("Copying acc from SPI to SD");
//  ex.print("tstp,x,y,z\n");
//  while (in.available()) {
//    in.read((byte *)&accB, sizeof(accValues_t)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//    ex.print(String(accB.tstp)+", "+String(accB.x)+", "+String(accB.y)+", "+String(accB.z)+"\n");
//  }
//    Serial.println("Accelerometer measures are now on the sd card!");
//  digitalWrite(RED_LED,LOW);
//  in.close();
//  ex.close();
//  count++;
//  fname="alti_"+String(count)+".bin";
//  fname.toCharArray(b, fname.length());
//  }
//  /*****END ACC*****/
//  return 1;
// 
//}
//
void Logger::setup(){
    pinMode(GREEN_LED,OUTPUT);
    pinMode(RED_LED,OUTPUT);
    
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
    
  }else{
    Serial.println("SD initialized.");

    if(!transferLogToSD()){
        Serial.println("Failed to copy files from flash to SD card");
        while (1) {
        // Error, so don't do anything more - stay stuck here
        }
    }
  }
  Serial.println("QUI");
  int count=0;
  char b[100];
  String fname="dump_"+String(count)+".bin";
  fname.toCharArray(b, fname.length());
  while(File f=myfs.open(b, FILE_READ)){
    f.close();
    count++;
    fname="dump_"+String(count)+".bin";
    fname.toCharArray(b, fname.length());
  }
  String dN="dump_"+String(count++);
  dN.toCharArray(_dumpFileName,dN.length());
  String sN="state_"+String(count++);
  sN.toCharArray(_stateFileName,sN.length());

}

void Logger::initDone(){
    digitalWrite(GREEN_LED,HIGH);
  }
  
void Logger::save(Buffers *buffers){
    File file = myfs.open(_dumpFileName, FILE_WRITE);
    file.write((byte *)&buffers, sizeof(*buffers)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    file.close();
    digitalWrite(RED_LED,LOW);
    
}

void Logger::save_state(State *state){
  File file = myfs.open(_stateFileName, FILE_WRITE);
  file.write((byte *)&state, sizeof(*state)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
  file.close();
  digitalWrite(RED_LED,LOW);
}

//void Logger::save(){
//      if(altiBuff_l >= (int)(0.7*BUFF_LEN)){
//    digitalWrite(RED_LED,HIGH);
//    altiFile = myfs.open(altiFileName, FILE_WRITE);
//    //detachInterrupt(digitalPinToInterrupt(BMP388_INT));
//    altiFile.write((byte *)&altiBuff, sizeof(altiValues_t)*altiBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//    altiBuff_l=0;
//    //Serial.println("Saving alti data");
//    altiFile.close();
//    digitalWrite(RED_LED,LOW);
//    //attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure,RISING);
//  }
//  
//  if(gyroBuff_l >= (int)(0.7*BUFF_LEN)){
//    digitalWrite(RED_LED,HIGH);
//    gyroFile = myfs.open(gyroFileName, FILE_WRITE);
//    //detachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO));
//    gyroFile.write((byte *)&gyroBuff, sizeof(gyroValues_t)*gyroBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//    gyroBuff_l=0;
//    //Serial.println("Saving gyro data");
//    gyroFile.close();
//    digitalWrite(RED_LED,LOW);
//    //attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure, RISING);
//  }
//  
//  if(accBuff_l >= (int)(0.7*BUFF_LEN)){
//    digitalWrite(RED_LED,HIGH);
//    accFile = myfs.open(accFileName, FILE_WRITE);
//    //detachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC));
//    accFile.write((byte *)&accBuff, sizeof(accValues_t)*accBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//    accBuff_l=0;
//    //Serial.println("Saving acc data");
//    accFile.close();
//    digitalWrite(RED_LED,LOW);
//    //attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure, RISING);
//  }
//  if(highAccBuff_l >= (int)(0.7*BUFF_LEN)){
//    digitalWrite(RED_LED,HIGH);
//    highAccFile = myfs.open(highAccFileName, FILE_WRITE);
//    highAccFile.write((byte *)&highAccBuff, sizeof(highAccValues_t)*highAccBuff_l); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
//    highAccBuff_l=0;
//    //Serial.println("Saving highAcc data");
//    accFile.close();
//    digitalWrite(RED_LED,LOW);
//  }
//}
//
//void Logger::altimeterMeasure(){
//  if(firstAl){
//    altiBuff[altiBuff_l] = altimeter.getMeasure(0.0,0.0);
//    firstAl=false;
//  }
//  else{
//    altiBuff[altiBuff_l] = altimeter.getMeasure(altiBuff[altiBuff_l-1].altitude, altiBuff[altiBuff_l-1].filtAlti);
//  }
//  f1=true;
//  Serial.println("Alti:" + String(altiBuff[altiBuff_l].altitude,2)+ " FAlti:" + String(altiBuff[altiBuff_l].filtAlti,2));
//  altiBuff_l++;
//}
//
//void Logger::accMeasure(){
//  if(firstA){
//    accBuff[accBuff_l] = imu.getAcc(0.0,0.0);
//    firstA=false;
//  }else{
//    accBuff[accBuff_l] = imu.getAcc(accBuff[accBuff_l-1].mod, accBuff[accBuff_l-1].filtAcc);
//  }
//  f2=true;
//  //Serial.println("Acc:"+String(accBuff[accBuff_l].mod, 2)+ " FAcc:" + String(accBuff[accBuff_l].filtAcc, 2));
//  accBuff_l++;
//}
//
//void Logger::highAccMeasure(){
//  highAccBuff[highAccBuff_l]=accelerometer.getAcc();
//  highAccBuff_l++;  
//}
//
//void Logger::gyroMeasure(){
//  if(firstG){
//    gyroBuff[gyroBuff_l] = imu.getGyro(0.0,0.0);
//    firstG=false;
//  }else{
//    gyroBuff[gyroBuff_l] = imu.getGyro(gyroBuff[gyroBuff_l-1].mod,gyroBuff[gyroBuff_l-1].filtGyro);
//  }
//  f3=true;
//  //Serial.println(String(gyroBuff[gyroBuff_l].tstp) + " " + String(gyroBuff[gyroBuff_l].mod, 2)+ " " + String(gyroBuff[gyroBuff_l].filtGyro, 2));
//  gyroBuff_l++;
//}
