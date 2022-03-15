#ifndef Data_H
#define Data_H

//#define CIRCULAR_BUFFER_INT_SAFE
#include <CircularBuffer.h>

//#include "vect.h"

#define QSIZE 100000 // Define for buffer

typedef enum {
  ON_PAD,
  ASCENT,
  DESCENT,
  LANDED
} flightState_t;

struct State{
  unsigned long tstp = 0;
  flightState_t state = ON_PAD ;
};

#endif
