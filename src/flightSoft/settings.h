#ifndef Settings_H
#define Settings_H

#define __DSO32__
//#define __ISM330__
#define __BMP388__
//#define __H3LIS331DL__
//#define LOGGING
#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define BOARD_READY digitalWrite(GREEN_LED,HIGH)


#endif
