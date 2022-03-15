#include "flightSoft.h"


void setup_sensors(){

    #if defined(__DSO32__) || defined(__ISM330__)
    imu.setup();
    DEBUG("Imu setted up.");
    #endif
    
    #ifdef __BMP388__
    altimeter.setup();
    DEBUG("Altimeter setted up.");
    #endif

    #ifdef __H3LIS331DL__
    hf_acc.setup();
    DEBUG("HF imu setted up.");
    #endif
}

void setup() {

  #ifdef __DEBUG__
  Serial.begin(512000);
  #endif
  //SPI.begin();
  #ifndef __DSO32__
  SPI.begin();
  #endif
//  attachInterrupts
  setup_sensors();
  DEBUG("Sensors setted up.");
  #ifdef LOGGING
  logger.setup(String("ciao"));
  if(!logger.transferLogToSD<imu_values>(String("ciao"))){
        Serial.println("Failed to copy files from flash to SD card");
        while (1) {
        // Error, so don't do anything more - stay stuck here
        }
    }
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
  #ifdef __LOGGING__
  logger.ready()
  #endif
  
}

void loop() {
//  logger.save();
  
//   switch(state.state){
//    case ON_PAD:
//     // if(){  //launch condition met
//        
//      state.state=ASCENT;
//      state.tstp = millis();
//     // }
//      break;
//    case ASCENT:
//    
//    //if(){  //apogee condition met
//        
//      //  state=DESCENT;
//     // }
//      break;
//    case DESCENT:
//   // if(){  //landing condition met
//        
//     //   state=LANDED;
//     // }
//      //break;
//    case LANDED:
//      logger.save(&buffers);
//      //Remember to save the remain of the buffers bf closing
//      //Remember to close the file buffers
//      break;
//    }
  }
  
#ifdef __BMP388__
void altimeterCB(){
  if(aq_len<=QSIZE){
    if(aq_len==0){
      altiValues_t v = {};
      v = altimeter.measure(v);
      aq[aq_len]=v;
      aq_len++;
    }else{
      altiValues_t v = altimeter.measure(aq[aq_len]);
      aq[aq_len]=v;\
      aq_len++;
      DEBUG("ALTI: " + String(v.toString().c_str())+", "+String(aq_len));
    }
  }
}
#endif

#if defined(__DSO32__) || defined(__ISM330__)
void gyroCB(){
  if(gq_len<=QSIZE){
    if(gq_len==0){
      imu_values v = {};
      v = imu.measureGyro(v);
      gq[gq_len]=v;
      gq_len++;
    }else{
      imu_values v = imu.measureGyro(gq[gq_len]);
      gq[gq_len]=v;\
      gq_len++;
      DEBUG("GYRO: " + String(v.toString().c_str())+", "+String(gq_len));
      DEBUG(QSIZE);
    }
  }
}

void accCB(){
  if(acq_len<=QSIZE){
    if(acq_len==0){
      imu_values v = {};
      v = imu.measureAcc(v);
      acq[acq_len]=v;
      acq_len++;
    }else{
      imu_values v = imu.measureAcc(acq[acq_len]);
      acq[acq_len]=v;\
      acq_len++;
      DEBUG("ACC: " + String(v.toString().c_str())+", "+String(acq_len));
    }
  }
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
