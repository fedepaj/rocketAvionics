#include "flightSoft.h"
#include "TeensyTimerTool.h"
//using namespace TeensyTimerTool;

//PeriodicTimer cLogicTimer;

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
  pinMode(0, INPUT_PULLUP);
  Serial.begin(512000);
  removeBeforeFlight();
  #ifdef __DEBUG__
  Serial.begin(512000);
  #endif
  //SPI.begin();
  
  
  
  #ifdef __LOGGING__
  
  //logger.transferLogsToSD();
  #endif
  SPI.begin();
  delay(100);
  setup_sensors();
  
  DEBUG("Sensors setted up.");
  
  //cLogicTimer.begin(cLogicCallBack, 20ms);
  
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
  logger.setup();
  logger.ready();
  #endif
  
  delay(120000);
  detachInterrupt(digitalPinToInterrupt(BMP388_INT));
  detachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC));
  detachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO));
  #ifdef __LOGGING__
  DEBUG("Beginnning dump");
  logger.save_states(states, sq_len);
  DEBUG("Dumped states");
  #if defined(__DSO32__) || defined(__ISM330__)
  logger.save_acc(acq, acq_len);
  DEBUG("Dumped acc");
  logger.save_gyro(gq, gq_len);
  #endif
  DEBUG("Dumped gyro");
  logger.save_alti(aq, aq_len);
  DEBUG("Dumped alti");
  logger.transferLogsToSD();
  #endif
  #ifdef __LOGGING__
  logger.blink();
  #endif
  DEBUG("FINE");

}

void loop() {
//  switch(state){
//    case LANDED:
//      #ifdef __LOGGING__
//      DEBUG("Beginnning dump");
//      logger.save_states(states, sq_len);
//      DEBUG("Dumped states");
//      #if defined(__DSO32__) || defined(__ISM330__)
//      logger.save_acc(acq, acq_len);
//      DEBUG("Dumped acc");
//      logger.save_gyro(gq, gq_len);
//      #endif
//      DEBUG("Dumped gyro");
//      //logger.save_alti(aq, aq_len);
//      DEBUG("Dumped alti");
//      logger.transferLogsToSD();
//      #endif
//      #ifdef __LOGGING__
//      logger.blink();
//      #endif
//      for(int i=0;i<1000000;i++){}
//      break;
//  }
}
//
//void cLogicCallBack(){
//  /*TEST
//  Serial.print("AccelFiltrata:"); Serial.print(acc_last_500.last().filt);Serial.print(", ");
//  Serial.print("Accel:"); Serial.print(acc_last_500.last().mod);
//  Serial.println();
//  */
//  switch(state){
//    case ON_PAD:
//      state=LANDED;
////      if(acc_last_500.last().filt >= 50){
////        state=ASCENT;
////        stateS.state=state;
////        stateS.tstp = millis();
////        states[sq_len++]=stateS;
////      }
////      //Serial.println("on pad");
////      break;
////    case ASCENT:
////      if(altimeter_last_500.last().filtVel <= -1.0){
////        state=DESCENT;
////        stateS.state=state;
////        stateS.tstp = millis();
////        states[sq_len++]=stateS;
////      }
//      //Serial.println("ASCENT");
////      break;
////    case DESCENT:
////      if(altimeter_last_500.last().filtVel >= -0.5 && altimeter_last_500.last().filtVel <= 0.5){
////        state=LANDED;
////        stateS.state=state;
////        stateS.tstp = millis();
////        states[sq_len++]=stateS;
////      }
////      //Serial.println("descent");
////      break;
//    case LANDED:
//      //non fa niente
//      //Serial.println("LANDED");
//      break;
//    }
//  
//}  

#ifdef __BMP388__
void altimeterCB(){
  altiValues_t v = altimeter.measure();
  if(aq_len<=QSIZE){
    aq[aq_len]=v;
    aq_len++;
  }
  //DEBUG("ALTI: " + String(v.toString().c_str())+", "+String(aq_len));
}

#endif

#if defined(__DSO32__) || defined(__ISM330__)
void gyroCB(){
  imu_values v = imu.measureGyro();
  if(gq_len<=QSIZE){
    gq[gq_len]=v;
    gq_len++;
  }
  //DEBUG("GYRO: " + String(v.toString().c_str())+", "+String(gq_len));
}

void accCB(){
  imu_values v = imu.measureAcc();
  if(gq_len<=QSIZE){
    acq[acq_len]=v;
    acq_len++;
  }
  //DEBUG("ACC: " + String(v.toString().c_str())+", "+String(acq_len));
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

void removeBeforeFlight(){
  while(!digitalRead(0)){
    DEBUG("WAITING");
    delay(100);
  }
}
