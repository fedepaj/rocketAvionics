#include "flightSoft.h"

Logger logger;
#ifdef BMP388_ON
BMP388 altimeter;
#endif
#if defined(DSO32_ON) || defined(ISM330_ON)
DSO32 imu;
#endif
#ifdef H3LIS331DL_ON
H3LIS331DL hf_acc;
#endif


EXTMEM struct Buffers buffers;

EXTMEM State state = {};

void setup_sensors(){
    #ifdef BMP388_ON
    altimeter.setup();
    #endif
    #if defined(DSO32_ON) || defined(ISM330_ON)
    imu.setup();
    #endif
    #ifdef H3LIS331DL_ON
    hf_acc.setup();
    #endif
}

void setup() {
  
  Serial.begin(512000);



//  logger.init();
  Serial.println("Sensors setted up.");
//  attachInterrupts
  setup_sensors();
  logger.setup();
  
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterCB, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accCB, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroCB, RISING);
  attachInterrupt(digitalPinToInterrupt(H3LIS331DL_INT), hf_accCB, RISING);
  Serial.println("Interrupts setted up.");
  Serial.println("---");
  
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

void altimeterCB(){
  if(sizeof(buffers.altimeter)==0){
    altiValues_t v = {};
    buffers.altimeter.push_back(v);
  }
  buffers.altimeter.push_back(altimeter.measure(buffers.altimeter.back()));
}

void gyroCB(){
  if(sizeof(buffers.gyro)==0){
    imu_values v = {};
    buffers.gyro.push_back(v);
  }
  buffers.gyro.push_back(imu.measureGyro(buffers.gyro.back()));
}

void accCB(){
  if(sizeof(buffers.acc)==0){
    imu_values v = {};
    buffers.acc.push_back(v);
  }
  buffers.acc.push_back(imu.measureAcc(buffers.acc.back()));
}

void hf_accCB(){
  if(sizeof(buffers.hf_acc)==0){
    hf_imu_values v = {};
    buffers.hf_acc.push_back(v);
  }
  buffers.hf_acc.push_back(hf_acc.measure(buffers.hf_acc.back()));
}
