#ifndef Logger_H
#define Logger_H

#define RED_LED 8
#define GREEN_LED 28

#include <SD.h>
#include <SdFat.h>
#include <LittleFS.h>
#include <iostream>
#include <string>

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO

struct Buffers{
  #ifdef BMP388_ON
  std::vector<altiValues_t> altimeter {};
  #endif
  #if defined(DSO32_ON) || defined(ISM330_ON)
  std::vector<imu_values> acc {};
  std::vector<imu_values> gyro {};
  #endif
  #ifdef H3LIS331DL_ON
  std::vector<hf_imu_values> hf_acc {};
  #endif

//  std::string toString()
//  {
//    return "title = " + title + ", year = " + to_string(year);
//  }
};

typedef enum {
  ON_PAD,
  ASCENT,
  DESCENT,
  LANDED
} flightState_t;

struct State{
  unsigned long tstp = 0;
  flightState_t state = ON_PAD ;
  
//  string toString()
//  {
//    return "title = " + title + ", year = " + to_string(year);
//  }
};

class Logger{
    public:
        void setup();
        void save(Buffers *buffers);
        void save_state(State *state);
        void initDone();
        int transferLogToSD();
    private:
        LittleFS_QPINAND myfs;
        SdFat32 sd;
        
        File dump;
        File state;
        char _dumpFileName[100]; 
        char _stateFileName[100];

};
#endif
