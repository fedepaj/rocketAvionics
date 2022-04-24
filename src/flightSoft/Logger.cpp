#include "Logger.h"

void printSpaces(int num) {
  for (int i=0; i < num; i++) {
    Serial.print(" ");
  }
}

void printDirectory(File dir, int numSpaces) {
   while(true) {
     File entry = dir.openNextFile();
     if (! entry) {
       //Serial.println("** no more files **");
       break;
     }
     printSpaces(numSpaces);
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numSpaces+2);
     } else {
       // files have sizes, directories do not
       printSpaces(36 - numSpaces - strlen(entry.name()));
       Serial.print("  ");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void printDirectory(FS &fs) {
  Serial.println("Directory\n---------");
  printDirectory(fs.open("/"), 0);
  Serial.println();
}

void listFiles(FS &fs)
{
  Serial.println("---------------");
  printDirectory(fs);
  Serial.printf("Bytes Used: %llu, Bytes Total:%llu\n", fs.usedSize(), fs.totalSize());
}

int Logger::format(){
  File dir = myfs.open("/");
  while(File entry = dir.openNextFile()) {
    const char* name = entry.name();
    entry.close();
    myfs.remove(name);
    
  }
  return 0;
}

void Logger::currFileNumber(){
    listFiles(myfs);
    char b[100];
    String fname="gyro_"+String(file_count)+".bin";
    DEBUG("FNAME: "+fname);
    fname.toCharArray(b, fname.length()+1);
    while(File f=myfs.open(b, FILE_READ)){
      f.close();
      file_count++;
      fname="gyro_"+String(file_count)+".bin";
      fname.toCharArray(b, fname.length()+1);
      DEBUG("FNAME: "+fname);
    }
    DEBUG("COUNT: " + String(file_count));
}

void Logger::setup(){
    DEBUG("IN SETUP");
    pinMode(GREEN_LED,OUTPUT);
    pinMode(RED_LED,OUTPUT);
    
    // see if the Flash is present and can be initialized:
    if (!myfs.begin()) {
      DEBUG("Error starting QSPI FLASH");
      while(1) {blink();}
    }
    //format();
    currFileNumber();
    DEBUG("LittleFS initialized.");
    delay(500);
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

void Logger::ready(){
  digitalWrite(GREEN_LED,HIGH);
}


void Logger::done(){
  digitalWrite(RED_LED,HIGH);
}

void Logger::blink(){
  digitalWrite(RED_LED,HIGH);
  delay(1000);
  digitalWrite(RED_LED,LOW);
  delay(1000);
}

void Logger::currFolderNumber(){
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
  }
  folder_count=c;
  DEBUG("We are now in folder " + folderName + String(c));
}

template <typename T> int Logger::transferLogToSD(File entry){
  if(!sd_ok) return 1;
  T v;
  File32 ex;
  if (!ex.open(String(entry.name()+String(".csv")).c_str(), O_WRONLY | O_CREAT)) {
    return 1;
  }
  while (entry.available()) {
    entry.read(&v, sizeof(T)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1  
    ex.print(String(v.toString().c_str()));
  }
  // Here we delete the file
  entry.close();
  delay(100);
  ex.close();
  delay(100);
  return 0;
}

int Logger::transferLogsToSD(){
  listFiles(myfs);
  if(!sd_ok) return 1;
  File dir = myfs.open("/");
  while(File entry = dir.openNextFile()) {
    delay(100);
    DEBUG("IN FS ROOT");
    std::string s = entry.name();
    DEBUG(String(s.c_str()));
    if(s.rfind("states", 0) == 0){
      transferLogToSD<State>(entry);
    }
    #if defined(__DSO32__) || defined(__ISM330__)
    else if(s.rfind("acc", 0) == 0){
      transferLogToSD<imu_values>(entry);
    }
    else if(s.rfind("gyro", 0) == 0){
      transferLogToSD<imu_values>(entry);
    }
    #endif
    
    #ifdef __BMP388__
    else if(s.rfind("alti", 0) == 0){
      transferLogToSD<altiValues_t>(entry);
    }
    #endif
    myfs.remove(entry.name());
    delay(100);
    entry.close();
    delay(100);
    
  }
  return 0;
}

template <typename T> void Logger::save(T q[], int len, String filename){
    char buff[100];
    DEBUG("QUI1");
    String fname = String(filename + String(file_count)+".bin");
    DEBUG("QUI2");
    fname.toCharArray(buff,fname.length()+1);
    DEBUG("QUI3");
    
    File file = myfs.open(buff, FILE_WRITE);
    DEBUG("QUI4");
    delay(100);
    for(int i=0;i<len;i++){
      file.write(&q[i], sizeof(T)); //https://gist.github.com/CelliesProjects/7fab9013517583b3a0922c0f153606a1
    }
    delay(100);
    DEBUG("QUI5");
    file.close();
    delay(100);
    DEBUG("QUI6");

}

void Logger::save_states(State states[], int len){
  save<State>(states, len, statesFileName);
  DEBUG("Saving states");
}

#if defined(__DSO32__) || defined(__ISM330__)
void Logger::save_acc(imu_values v[], int len){
  save<imu_values>(v, len, accFileName);
  DEBUG("Saving acc data");
}

void Logger::save_gyro(imu_values v[], int len){
  save<imu_values>(v, len, gyroFileName);
  DEBUG("Saving gyro data");
}
#endif

#ifdef __BMP388__
void Logger::save_alti(altiValues_t v[], int len){
  save<altiValues_t>(v, len, altiFileName);
  DEBUG("Saving altimeter data");
}
#endif
