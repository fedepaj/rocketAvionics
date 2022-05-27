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
H3LIS331DL hf_imu;
#endif

#ifdef __LOGGING__
Logger logger;
#endif

#ifdef __TELEMETRY__
#include "SerialTransfer.h"
SerialTransfer myTransfer;

typedef struct{
  char z = '$';
  float y = 0.1;
}Msg_uno;
  
typedef struct{
  int a[15];
  char b = '@';
}Msg_due;

Msg_uno msg_tipo_uno;
Msg_due msg_tipo_due;
#endif

State states[10];
volatile int sq_len=0;

#ifdef __BMP388__
EXTMEM altiValues_t f_aq[QSIZE];
EXTMEM altiValues_t s_aq[QSIZE];
volatile int f_aq_len=0;
volatile int s_aq_len=0;
int curr_aq=0;
altiValues_t* aq = f_aq;
volatile int* aq_len = &f_aq_len;

#endif

#if defined(__DSO32__) || defined(__ISM330__)
EXTMEM imu_values f_gq[QSIZE];
EXTMEM imu_values s_gq[QSIZE];
volatile int f_gq_len=0;
volatile int s_gq_len=0;
int curr_gq=0;
imu_values* gq = f_gq;
volatile int* gq_len = &f_gq_len;

EXTMEM imu_values f_acq[QSIZE];
EXTMEM imu_values s_acq[QSIZE];
volatile int f_acq_len=0;
volatile int s_acq_len=0;
int curr_acq=0;
imu_values* acq = f_acq;
volatile int* acq_len = &f_acq_len;
#endif

#ifdef __H3LIS331DL__
EXTMEM hf_imu_values f_hf_acq[QSIZE];
EXTMEM hf_imu_values s_hf_acq[QSIZE];
volatile int f_hf_acq_len=0;
volatile int s_hf_acq_len=0;
int curr_hf_acq=0;
hf_imu_values* hf_acq = f_hf_acq;
volatile int* hf_acq_len = &f_hf_acq_len;
#endif

State stateS = {};
FligthState state = ON_PAD ;
LedState ledState;
void resetCause(uint32_t resetStatusReg);
int lastMillis;
