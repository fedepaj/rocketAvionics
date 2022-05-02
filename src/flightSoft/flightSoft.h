#include "settings.h"
#include "commons.h"

#ifdef __LOGGING__
#include "Logger.h"
#endif

#ifdef __BMP388__
BMP388 altimeter;
#endif

#if defined(__DSO32__) || defined(__ISM330__)
DSO32 imu;
#endif

#ifdef __H3LIS331DL__
H3LIS331DL hf_acc;
#endif

#ifdef __LOGGING__
Logger logger;
#endif

State states[10];
volatile int sq_len=0;

#ifdef __BMP388__
EXTMEM altiValues_t aq[QSIZE];
volatile int aq_len=0;
#endif

#if defined(__DSO32__) || defined(__ISM330__)
EXTMEM imu_values gq[QSIZE];
volatile int gq_len=0;


EXTMEM imu_values acq[QSIZE];
volatile int acq_len=0;

#endif

#ifdef __H3LIS331DL__
EXTMEM hf_imu_values hf_acq[QSIZE];
volatile int hf_acq_len=0;
#endif

State stateS = {};
FligthState state = ON_PAD ;
LedState ledState;
