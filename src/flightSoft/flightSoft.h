#define DSO32_ON
//#define ISM330_ON
#define BMP388_ON
#define H3LIS331DL_ON
#define LOGGING
//#define DEBUG


#define BOARD_READY digitalWrite(GREEN_LED,HIGH)
#define QSIZE 1000 // Define for buffer

#include <vector>


#ifdef BMP388_ON
#include "BMP388.h"
#endif
#if defined(DSO32_ON) || defined(ISM330_ON)
#include "DSO32.h"
#endif
#ifdef H3LIS331DL_ON
#include "H3LIS331DL.h"
#endif



#ifdef LOGGING
#include "Logger.h"
#endif
