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
    int count=0;
  char b[100];
  String fname="gyro_"+String(count)+".bin";
  fname.toCharArray(b, fname.length());
  while(File f=myfs.open(b, FILE_READ)){
    f.close();
    count++;
    fname="acc_"+String(count)+".bin";
    fname.toCharArray(b, fname.length());
  }
    String gN="gyro_"+String(count); 
    gN.toCharArray(gyroFileName,gN.length());
    String aN="alti_"+String(count);
    aN.toCharArray(altiFileName,aN.length());
    String acN="acc_"+String(count);
    acN.toCharArray(accFileName,acN.length());
    String hAcN="highAcc_"+String(count);
    acN.toCharArray(highAccFileName,acN.length());
  }
  DEBUG("LittleFS initialized.");

  if (!sd.begin(SD_CONFIG)) {
    DEBUG("SD initialization failed!");
    sd_ok=false;
  }else{
    DEBUG("SD initialized.");
    sd_ok=true;
  }
}

void Logger::ready(){
  digitalWrite(GREEN_LED,HIGH);
  }


void Logger::done(){
  digitalWrite(RED_LED,HIGH);
  }

void Logger::cdToNewFolder(){
  if(!sd_ok) return;
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
  if(!sd_ok) return 1;
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
  return 0;
}

int Logger::transferLogsToSD(){
  File dir = myfs.open("/");
  cdToNewFolder();
  while(File entry = dir.openNextFile()) {

    std::string s = entry.name();
    if(s.rfind("state", 0) == 0){

      transferLogToSD<State>(entry);
    }
    
    #if defined(__DSO32__) || defined(__ISM330__)
    else if(s.rfind("imu_acc", 0) == 0){

      transferLogToSD<imu_values>(entry);
    }
    
    else if(s.rfind("imu_gyro", 0) == 0){

      transferLogToSD<imu_values>(entry);
    }
    #endif
    
    #ifdef __BMP388__
    else if(s.rfind("alti", 0) == 0){
      
      transferLogToSD<altiValues_t>(entry);
    }
    #endif
    myfs.remove(file.name());
  }
}

template <typename T> void Logger::save(T *q, int len, char filename[],File fileb){
    DEBUG("QUI1");
    fileb = myfs.open(filename, FILE_WRITE);
    //detachInterrupt(digitalPinToInterrupt(BMP388_INT));
    fileb.write((byte *)&q, sizeof(T)*len); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    len=0;
    Serial.println("Saving alti data");
    altiFile.close();

}

void Logger::save_states(State *states, int len){

  //save<State>(states, len, );
}

#if defined(__DSO32__) || defined(__ISM330__)
void Logger::save_acc(imu_values *v, int len){
  save<imu_values>(v,len,gyroFileName);
}

void Logger::save_gyro(imu_values *v, int len){
  save<imu_values>(v,len,accFileName);
}
#endif

#ifdef __BMP388__
void Logger::save_alti(altiValues_t *v, int len){
  save<altiValues_t>(v,len, altiFileName,altiFile);
}
#endif
