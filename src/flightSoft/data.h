#ifndef Data_H
#define Data_H

#define QSIZE 100000 // Define for buffer



#include <string>
#include <sstream>

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
