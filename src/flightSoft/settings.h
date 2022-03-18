#ifndef Settings_H
#define Settings_H

//#define __DSO32__
//#define __ISM330__
#define __BMP388__
//#define __H3LIS331DL__
#define __LOGGING__
#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define BOARD_READY digitalWrite(GREEN_LED,HIGH)


#ifdef __BMP388__
#include "BMP388.h"
#endif
#if defined(__DSO32__) || defined(__ISM330__)
#include "DSO32.h"
#endif
#ifdef __H3LIS331DL__
#include "H3LIS331DL.h"
#endif


#endif
