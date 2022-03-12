#include "flightSoft.h"

#ifdef LOGGING
Logger logger;
#endif
#ifdef __BMP388__
BMP388 altimeter;
#endif

#if defined(__DSO32__) || defined(__ISM330__)
DSO32 imu;
#endif

#ifdef __H3LIS331DL__
H3LIS331DL hf_acc;
#endif

EXTMEM struct Buffers buffers;

EXTMEM State state = {};

void setup_sensors(){

    #ifdef __BMP388__
    altimeter.setup();
    DEBUG("Altimeter setted up.");
    #endif
    
    #if defined(__DSO32__) || defined(__ISM330__)
    imu.setup();
    DEBUG("Imu setted up.");
    #endif
    
    #ifdef __H3LIS331DL__
    hf_acc.setup();
    DEBUG("HF imu setted up.");
    #endif
}

void setup() {
  
  Serial.begin(512000);

  SPI.begin();

//  logger.init();
  
//  attachInterrupts
  setup_sensors();
  DEBUG("Sensors setted up.");
  #ifdef LOGGING
  logger.setup();
  #endif
  #ifdef __BMP388__
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterCB, RISING);
  #endif
  #if defined(__DSO32__) || defined(__ISM330__)
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accCB, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroCB, RISING);
  #endif
  #ifdef __H3LIS331DL__
  attachInterrupt(digitalPinToInterrupt(H3LIS331DL_INT), hf_accCB, RISING);
  #endif
  DEBUG("Interrupts setted up.");
  DEBUG("---");
  
  digitalWrite(GREEN_LED,HIGH);
  
}

void loop() {
//  logger.save();
  
  
   switch(state.state){
    case ON_PAD:
     // if(){  //launch condition met
        
      state.state=ASCENT;
      state.tstp = millis();
     // }
      break;
    case ASCENT:
    
    //if(){  //apogee condition met
        
      //  state=DESCENT;
     // }
      break;
    case DESCENT:
   // if(){  //landing condition met
        
     //   state=LANDED;
     // }
      //break;
    case LANDED:
      logger.save(&buffers);
      //Remember to save the remain of the buffers bf closing
      //Remember to close the file buffers
      break;
    }
  }
#ifdef __BMP388__
void altimeterCB(){
  if(buffers.altimeter.size()==0){
    altiValues_t v = {};
    buffers.altimeter.push_back(v);
  }
  altiValues_t v = altimeter.measure(buffers.altimeter.back());
  //DEBUG("ALTI: " + String(v.toString().c_str()));
  buffers.altimeter.push_back(v);
}
#endif

#if defined(__DSO32__) || defined(__ISM330__)
void gyroCB(){
  if(buffers.gyro.size()==0){
    imu_values v = {};
    buffers.gyro.push_back(v);
  }
  imu_values v = imu.measureGyro(buffers.gyro.back());
  DEBUG("GYRO: " + String(v.toString().c_str()));
  buffers.gyro.push_back(v);
}

void accCB(){
  if(buffers.acc.size()==0){
    imu_values v = {};
    buffers.acc.push_back(v);
  }
  imu_values v = imu.measureAcc(buffers.acc.back());
  DEBUG("ACC: " + String(v.toString().c_str()));
  buffers.acc.push_back(v);
}
#endif

#ifdef __H3LIS331DL__
void hf_accCB(){
  if(buffers.hf_acc.size()==0){
    hf_imu_values v = {};
    buffers.hf_acc.push_back(v);
  }
  hf_imu_values v = hf_acc.measure(buffers.hf_acc.back());
  DEBUG("HF_ACC: " + String(v.toString().c_str()));
  buffers.hf_acc.push_back(v);
}
#endif
