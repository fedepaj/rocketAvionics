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

#include "data.h"

#ifdef LOGGING
#include "Logger.h"
#endif

#ifdef LOGGING
Logger logger;
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

//EXTMEM struct Buffers buffers;
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

EXTMEM State state = {};
