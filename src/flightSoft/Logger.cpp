#include "Logger.h"

void Logger::init(){
    DEBUG("IN SETUP");
    if (!sd.begin(SD_CONFIG)) {
      DEBUG("SD initialization failed!");
      sd_ok=false;
    }else{
      DEBUG("SD initialized.");
      delay(100);
      sd_ok=true;
      currFolderNumber();
    }
}

void Logger::currFolderNumber(){
  if(!sd_ok) return;
  int c=0;
  String folderName = "Experiment_";
  while (sd.exists(folderName+String(c))) c++;
  if (!sd.mkdir(folderName+String(c))) {
    DEBUG("Folder creation failed!");
    while(1){}
  }
  if (!sd.chdir(folderName+String(c))) {
    DEBUG("Chdir failed.");
  }
  folder_count=c;
  DEBUG("We are now in folder " + folderName + String(c));
}

template <typename T> void Logger::save(T q[], int len, String filename){
     File32 ex;
      if (!ex.open(String(String(filename)+String(file_count)+String(".csv")).c_str(), O_WRONLY | O_CREAT)) {
        return;
      }
      for(int i=0;i<len;i++){
        ex.print(String(q[i].toString().c_str()));
      }
      DEBUG(String(String(filename)+String(file_count)+String(".csv")));
      ex.close();

}

void Logger::save_states(State states[], int len){
  file_count++;
  save<State>(states, len, statesFileName);
}

#if defined(__DSO32__) || defined(__ISM330__)
void Logger::save_acc(imu_values v[], int len){
  save<imu_values>(v, len, accFileName);
}

void Logger::save_gyro(imu_values v[], int len){
  save<imu_values>(v, len, gyroFileName);
}
#endif

#ifdef __BMP388__
void Logger::save_alti(altiValues_t v[], int len){
  save<altiValues_t>(v, len, altiFileName);
}
#endif
