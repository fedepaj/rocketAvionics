#include "Logger.h"

void Logger::setup(){
    pinMode(GREEN_LED,OUTPUT);
    pinMode(RED_LED,OUTPUT);
    
    // see if the Flash is present and can be initialized:
  if (!myfs.begin()) {
    DEBUG("Error starting QSPI FLASH");
    //error();
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  DEBUG("LittleFS initialized.");

  if (!sd.begin(SD_CONFIG)) {
    DEBUG("SD initialization failed!");
  }else{
    DEBUG("SD initialized.");
  }
}

void Logger::ready(){
  digitalWrite(GREEN_LED,HIGH);
  }

void Logger::cdToNewFolder(){
  int c=0;
  String folderName = "Experiment_";
  while (sd.exists(folderName+String(c))) c++;
  if (!sd.mkdir(folderName+String(c))) {
    DEBUG("Folder cration failed!");
    while(1){}
  }
  if (!sd.chdir(folderName+String(c))) {
    DEBUG("Chdir failed.");
    while(1){}
  }

  Serial.println("We are now in folder "+folderName+String(c));
}

template <typename T> int Logger::transferLogToSD(File entry){
  T v;
  File32 ex;
  if (!ex.open(String(entry.name()+String(".csv")).c_str(), O_WRONLY | O_CREAT)) {
    return 1;
  }
  while (entry.available()) {
    entry.read((byte *)&v, sizeof(T)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1  
    ex.print(String(v.toString().c_str()));
  }
  // Here we need to delete the file
  entry.close();
  ex.close();
}

int Logger::transferLogsToSD(){
  File dir = myfs.open("/");
  cdToNewFolder();
  while(File entry = dir.openNextFile()) {
    if(String(entry.name()[6]).equals(String("states"))){
      transferLogToSD<State>(entry);
    }
    
    #if defined(__DSO32__) || defined(__ISM330__)
    else if(String(entry.name()[3]).equals(String("acc"))){
      transferLogToSD<imu_values>(entry);
    }
    
    else if(String(entry.name()[4]).equals(String("gyro"))){
      transferLogToSD<imu_values>(entry);
    }
    #endif
    
    #ifdef __BMP388__
    else if(String(entry.name()[4]).equals(String("alti"))){
      transferLogToSD<altiValues_t>(entry);
    }
    #endif
  }
}

String Logger::currDump(String filename, int *prec_dump){
    if(*prec_dump==-1){
      const char* fname=String(filename+String(*prec_dump++)+String(".bin")).c_str();
      while(File f=myfs.open(fname, FILE_READ)){
        f.close();
        fname=String(filename+String(*prec_dump++)+String(".bin")).c_str();
      }
      return String(filename+String(*prec_dump));
    }else{
      return String(filename+String(*prec_dump++));
    }
}

template <typename T> void Logger::save(T *q, int len, String filename){
    File file = myfs.open(filename.c_str(), FILE_WRITE);
    file.write((byte *)&q, sizeof(T)*len); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    DEBUG("Saving data");
    file.close();
    digitalWrite(RED_LED,LOW);
    
}

void Logger::save_states(State *states, int len){
  save<State>(states, len, currDump(f_states_name,&f_states_cd));
}

#if defined(__DSO32__) || defined(__ISM330__)
void Logger::save_acc(imu_values *v, int len){
  save<imu_values>(v,len, currDump(f_imu_acc_name,&f_imu_acc_cd));
}

void Logger::save_gyro(imu_values *v, int len){
  save<imu_values>(v,len, currDump(f_imu_gyro_name,&f_imu_gyro_cd));
}
#endif

#ifdef __BMP388__
void Logger::save_alti(altiValues_t *v, int len){
  save<altiValues_t>(v,len, currDump(f_alti_name,&f_alti_cd));
}
#endif
