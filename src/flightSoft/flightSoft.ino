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
  setup_sensors();
  DEBUG("Sensors setted up.");
  #ifdef LOGGING
  logger.setup();
  logger.transferLogsToSD();
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
  
   switch(state){
      case ON_PAD:
        delay(5000);
        state=ASCENT;
        stateS.state=state;
        stateS.tstp = millis();
        states[sq_len]=stateS;
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
      #ifdef __LOGGING__
      logger.save_states(states,sq_len);
      logger.save_acc(acq,acq_len);
      logger.save_gyro(gq,gq_len);
      logger.save_alti(aq,aq_len);
      logger.transferLogsToSD();
      #endif
      //Remember to save the remain of the buffers bf closing
      //Remember to close the file buffers
      break;
    }
  }
  
//TODO: test this
// This could reduce the callback code of 1/4 sice we would need only to pass the rigth parameters in the callbacks
//template <typename T> T callback(T q[], int *len, T (*f)(T)){
//  T ve = {};
//    if(len==0){
//      T ve = {};
//      v = (*f)(ve);
//if(len<=QSIZE){
//      q[len]=v;
//      len++;
//}
//return v;
//    }else{
//      T v = (*f)(q[len]);
//if(len<=QSIZE){
//      q[len]=v;
//      len++;
//}
//      DEBUG("ALTI: " + String(v.toString().c_str())+", "+String(len));
//return v;
//    }
//  }
//  }
#ifdef __BMP388__
void altimeterCB(){
    if(aq_len==0){
      altiValues_t v = {};
      v = altimeter.measure(v);
      if(aq_len<=QSIZE){
        aq[aq_len]=v;
        aq_len++;
      }
      altimeter_last_500.push(v);
    }else{
      altiValues_t v = altimeter.measure(aq[aq_len]);
      if(aq_len<=QSIZE){
        aq[aq_len]=v;
        aq_len++;
      }
      altimeter_last_500.push(v);
      DEBUG("ALTI: " + String(v.toString().c_str())+", "+String(aq_len));
    }
}
#endif

#if defined(__DSO32__) || defined(__ISM330__)
void gyroCB(){
  
    if(gq_len==0){
      imu_values v = {};
      v = imu.measureGyro(v);
      if(gq_len<=QSIZE){
        gq[gq_len]=v;
        gq_len++;
      }
      gyro_last_500.push(v);
    }else{
      imu_values v = imu.measureGyro(gq[gq_len]);
      if(gq_len<=QSIZE){
        gq[gq_len]=v;
        gq_len++;
      }
      gyro_last_500.push(v);
      DEBUG("GYRO: " + String(v.toString().c_str())+", "+String(gq_len));
      DEBUG(QSIZE);
    }
}

void accCB(){
  if(acq_len<=QSIZE){
    if(acq_len==0){
      imu_values v = {};
      v = imu.measureAcc(v);
      if(gq_len<=QSIZE){
        acq[acq_len]=v;
        acq_len++;
      }
      acc_last_500.push(v);
    }else{
      imu_values v = imu.measureAcc(acq[acq_len]);
      if(gq_len<=QSIZE){
        acq[acq_len]=v;
        acq_len++;
      }
      acc_last_500.push(v);
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
