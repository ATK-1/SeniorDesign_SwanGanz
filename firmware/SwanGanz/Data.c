#include <ti/devices/msp/msp.h>
#include <string.h>
#include "OS.h"
#include "../inc/RTOS_UART.h"
#include "DAS.h"
#include "../inc/ADC.h"
#include "Display.h"
#include "LUT.h"

#define NUM_CHANNELS 6
#define K 14

int32_t injectateVol;
int32_t injectateTemp;
int32_t initialTemp;
int32_t initialVol;
int64_t accumlator;
int64_t areaUnderCurve; 

uint64_t getFlowRate() {
  int64_t numerator = injectateVol * (initialTemp - injectateTemp);

  if (areaUnderCurve == 0) {
    return 0;
  }

  return (int64_t)(numerator / areaUnderCurve);
}

uint32_t getInitialTemp() {
  return initialTemp;
}

uint32_t getAOC() {
  return areaUnderCurve;
}
//**************Low pass Digital filter**************
int32_t Size0;    // Size-point average, Size=2 to FILTERMAX
int32_t x0[32];   // one copy of data in MACQ
uint32_t I0;       // index to oldest
int32_t LPFSum0;  // sum of the last Size samples
int32_t sigma0;
int32_t snr0;      // signal to noise ratio
static void LPF_Init0(int32_t initial, int32_t size) { 
  int i;
  Size0 = size;
  I0 = Size0-1;
  LPFSum0 = Size0*initial; // prime MACQ with initial data
  for(i=0; i<Size0; i++){
    x0[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
static int32_t LPF_Calc0(int32_t newdata) {
  if(I0 == 0){
    I0 = Size0-1;              // wrap
  } else{
    I0--;                     // make room for data
  }
  LPFSum0 = LPFSum0+newdata-x0[I0];   // subtract oldest, add newest
  x0[I0] = newdata;     // save new data
  return LPFSum0 >> 5;
}

//**************Low pass Digital filter**************
int32_t Size1;    // Size-point average, Size=2 to FILTERMAX
int32_t x1[32];   // one copy of data in MACQ
uint32_t I1;       // index to oldest
int32_t LPFSum1;  // sum of the last Size samples
int32_t sigma1;
int32_t snr1;      // signal to noise ratio
static void LPF_Init1(int32_t initial, int32_t size) { 
  int i;
  Size1 = size;
  I1 = Size1-1;
  LPFSum1 = Size1*initial; // prime MACQ with initial data
  for(i=0; i<Size1; i++){
    x1[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
static int32_t LPF_Calc1(int32_t newdata) {
  if(I1 == 0){
    I1 = Size1-1;              // wrap
  } else{
    I1--;                     // make room for data
  }
  LPFSum1 = (LPFSum1-x1[I1])+newdata;   // subtract oldest, add newest
  x1[I1] = newdata;     // save new data
  return LPFSum1 >> 5;
}

//**************Low pass Digital filter**************
int32_t Size2;    // Size-point average, Size=2 to FILTERMAX
int32_t x2[32];   // one copy of data in MACQ
uint32_t I2;       // index to oldest
int32_t LPFSum2;  // sum of the last Size samples
int32_t sigma2;
int32_t snr2;      // signal to noise ratio
static void LPF_Init2(int32_t initial, int32_t size) { 
  int i;
  Size2 = size;
  I2 = Size2-1;
  LPFSum2 = Size2*initial; // prime MACQ with initial data
  for(i=0; i<Size2; i++){
    x2[i] = initial;
  }
}
// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
static int32_t LPF_Calc2(int32_t newdata) {
  if(I2 == 0){
    I2 = Size2-1;              // wrap
  } else{
    I2--;                     // make room for data
  }
  LPFSum2 = LPFSum2+newdata-x2[I2];   // subtract oldest, add newest
  x2[I2] = newdata;     // save new data
  return LPFSum2 >> 5;
}

static uint32_t transferKill;
static uint32_t InitialsKill;
static uint32_t readings;


uint32_t transmissions;
void TransferData() {
    while (1) {
        uint32_t data[NUM_CHANNELS];
        for (int i = 0; i < NUM_CHANNELS; i++) {
            data[i] = Fifo_Get(i);
        }

        if (transferKill) {
            transferKill = 0;
            areaUnderCurve = accumlator / 400;
            OS_Kill();
        }

        uint32_t pres1 = PresLG_Lut[data[PRESSURE1_LOW]];
        uint32_t pres2 = PresLG_Lut[data[PRESSURE1_HI]];
        uint32_t temp = LPF_Calc2(TempHG_LUT[data[THERM_HI]]);

  

        UART_OutChar(0xFA);
        UART_OutU16((uint16_t)pres1); //p1
        UART_OutU16((uint16_t)pres2); //p2
        UART_OutU16((uint16_t)(temp / 10)); //therm

      
        int32_t diff = (initialTemp - temp);
        accumlator += diff;

        readings++;
        if (readings == 250) {
          sendNewVals(pres1, pres2, temp / 10);
          readings = 0;
        }
    }
}

void InitReadings() {
    LPF_Init0(0, 32);
    LPF_Init1(0, 32);
    LPF_Init2(0, 32);

    while (1) {
        uint32_t data[NUM_CHANNELS];
        for (int i = 0; i < NUM_CHANNELS; i++) {
            data[i] = Fifo_Get(i);
        }
    
        if (InitialsKill) {
            readings = 0;
            InitialsKill = 0;
            initialTemp = LPF_Calc2(TempHG_LUT[data[THERM_HI]]);
            OS_AddThread(&TransferData, 1);
            OS_AddThread(&DisplayMeasuring, 2);
            OS_AddThread(&DisplayCurrentReadings, 2);
            // OS_SetPerioidcSchedule(1);
            OS_Kill();
        }

        uint32_t pres1 = PresLG_Lut[data[PRESSURE1_LOW]];
        uint32_t pres2 = PresLG_Lut[data[PRESSURE1_HI]];
        uint32_t temp = LPF_Calc2(TempHG_LUT[data[THERM_HI]]);


        readings++;
        if (readings == 250) {
          sendNewVals(pres1, pres2, temp / 10);
          readings = 0;
        }
    }
}

void killTransfer() {
    transferKill = 1;
}

void startTransfer(uint32_t injectTemp, uint32_t injectVol) {
    InitialsKill = 1;
    injectateVol = injectVol;
    injectateTemp = injectTemp;
}