#ifndef Commons_H
#define Commons_H

#define RED_LED 8
#define GREEN_LED 28

#include <string>
#include <sstream>

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__); Serial.flush()
#else
#define DEBUG(...)
#endif

#define LED00(...) {\
  digitalWrite(RED_LED,LOW);\
  digitalWrite(GREEN_LED,LOW);\
}

#define LED01(...) {\
  digitalWrite(GREEN_LED,HIGH);\
  digitalWrite(RED_LED,LOW);\
}

#define LED11(...) {\
  digitalWrite(RED_LED,HIGH);\
  digitalWrite(GREEN_LED,HIGH);\
}

#define LED10(...) {\
  digitalWrite(GREEN_LED,LOW);\
  digitalWrite(RED_LED,HIGH);\
}

#include "settings.h"
#ifdef __BMP388__
#include "BMP388.h"
#endif

#if defined(__DSO32__) || defined(__ISM330__)
#include "DSO32.h"
#endif

#ifdef __H3LIS331DL__
#include "H3LIS331DL.h"
#endif

enum LedState {LC0, LC1, LC2, LC3};

enum FligthState {ON_PAD=0, ASCENT,DESCENT ,LANDED};

struct State{
  unsigned long tstp = 0;
  FligthState state = ON_PAD ;
  std::string toString(){
    std::string states[] =
      {
        "ON_PAD",
        "ASCENT",
        "DESCENT",
        "LANDED"
      };
    std::ostringstream ss;
    ss << tstp << "," << states[state];
    std::string s(ss.str());
    return s;  
    }
};

#endif
