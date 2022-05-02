#include "flightSoft.h"
#include "TeensyTimerTool.h"

TeensyTimerTool::PeriodicTimer cLogicTimer;
TeensyTimerTool::PeriodicTimer ledsTimer;

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
  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  LED00();
  delay(100);
  //Serial.begin(512000);
  removeBeforeFlight();
  #ifdef __DEBUG__
  Serial.begin(512000);
  #endif
  //SPI.begin();
  
  
  
  #ifdef __LOGGING__
  
  //logger.transferLogsToSD();
  #endif
  #ifdef __LOGGING__
  logger.setup();
  #endif
  delay(500);
  SPI.begin();
  delay(100);
  setup_sensors();
  
  DEBUG("Sensors setted up.");
  //ledsTimer.begin(ledsCallBack, 1000ms);
  cLogicTimer.begin(cLogicCallBack, 20ms);

  
  
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
  
}

void loop() {
  #ifdef __LOGGING__
  delay(10000);
  //detachInterrupt(digitalPinToInterrupt(BMP388_INT));
  //detachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC));
  //detachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO));
  DEBUG("Beginnning dump");
  logger.save_states(states, sq_len);
  sq_len=0;
  DEBUG("Dumped states");
  #if defined(__DSO32__) || defined(__ISM330__)
  logger.save_acc(acq, acq_len);
  acq_len=0;
  DEBUG("Dumped acc");
  logger.save_gyro(gq, gq_len);
  gq_len=0;
  DEBUG("Dumped gyro");
  #endif
  #ifdef __BMP388__
  logger.save_alti(aq, aq_len);
  aq_len=0;
  DEBUG("Dumped alti");
  logger.transferLogsToSD();
  logger.createFiles();
  #endif
  #if defined(__DSO32__) || defined(__ISM330__)
  //attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accCB, RISING);
  //attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroCB, RISING);
  #endif
  #endif
}

void ledsCallBack(){
   switch(ledState){
    case LC0:
      LED00();
      ledState=LC1;
      break;
    case LC1:
      LED01();
      ledState=LC2;
      break;
    case LC2:
      LED11();
      ledState=LC3;
      break;
    case LC3:
      LED10();
      ledState=LC0;
      break;
    }
}

void cLogicCallBack(){
  /*TEST
  Serial.print("AccelFiltrata:"); Serial.print(acc_last_500.last().filt);Serial.print(", ");
  Serial.print("Accel:"); Serial.print(acc_last_500.last().mod);
  Serial.println();
  */
//  switch(state){
//    case ON_PAD:
//      if(imu.currAcc.filt >= 50){
//        state=ASCENT;
//        stateS.state=state;
//        stateS.tstp = millis();
//        states[sq_len++]=stateS;
//      }
//      break;
//    case ASCENT:
//      if(altimeter.curr.filtVel <= -1.0){
//        state=DESCENT;
//        stateS.state=state;
//        stateS.tstp = millis();
//        states[sq_len++]=stateS;
//      }
//      Serial.println("ASCENT");
//      break;
//    case DESCENT:
//      if(altimeter.curr.filtVel >= -0.5 && altimeter.curr.filtVel <= 0.5){
//        state=LANDED;
//        stateS.state=state;
//        stateS.tstp = millis();
//        states[sq_len++]=stateS;
//      }
//      //Serial.println("descent");
//      break;
//    case LANDED:
//      //non fa niente
//      //Serial.println("LANDED");
//      break;
//    }
  
}  

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
