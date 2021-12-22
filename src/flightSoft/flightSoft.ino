#include "Logger.h"

#define RED_LED 8
#define GREEN_LED 28

Logger logger;

typedef enum {
  ON_PAD,
  ASCENT,
  DESCENT,
  LANDED
} flightState_t;

flightState_t state = ON_PAD;

void setup() {
  Serial.begin(512000);
  
  logger.init();

  pinMode(H3LIS331DL_CS, OUTPUT);
  pinMode(H3LIS331DL_INT, INPUT);
  pinMode(DSO32_CS, OUTPUT);
  pinMode(DSO32_INT_GYRO, INPUT);
  pinMode(DSO32_INT_ACC, INPUT);
  pinMode(BMP388_CS, OUTPUT);
  pinMode(BMP388_INT, INPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);

  digitalWrite(H3LIS331DL_CS, HIGH);
  digitalWrite(DSO32_CS, HIGH);
  digitalWrite(BMP388_CS, HIGH);
  Serial.println("Pins setted up.");
  
  SPI1.setMISO(39);
  SPI1.begin();
  
  SPI.begin();
  
  Serial.println("Sensors setted up.");
  //attachInterrupts
  
  attachInterrupt(digitalPinToInterrupt(H3LIS331DL_INT), highAccMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_ACC), accMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(DSO32_INT_GYRO), gyroMeasure, RISING);
  attachInterrupt(digitalPinToInterrupt(BMP388_INT), altimeterMeasure, RISING);
  Serial.println("Interrupts setted up.");
  Serial.println("---");
  
  digitalWrite(GREEN_LED,HIGH);
  
}

void loop() {
  logger.save();
  
  
   switch(state){
    case ON_PAD:
     // if(){  //launch condition met
        
     //   state=ASCENT;
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
      //Remember to save the remain of the buffers bf closing
      //Remember to close the file buffers
      break;
    }
  }

void accMeasure(){
  logger.accMeasure();
}

void gyroMeasure(){
  logger.gyroMeasure();
}

void altimeterMeasure(){
  logger.altimeterMeasure();
}

void highAccMeasure(){
  logger.highAccMeasure();
}
