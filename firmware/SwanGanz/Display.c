#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"
#include "LUT.h"
#include "../inc/RTOS_UART.h"


#define NUM_CHANNELS 6
Sema4_t LCD_Mutex;
 uint32_t input;
 uint32_t initTemp;
 uint16_t tempVals[8000];
 uint32_t tempIdx;


void DisplayInit() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    OS_InitSemaphore(&LCD_Mutex, 1);
}

//**************Low pass Digital filter**************
int32_t Size1;    // Size-point average, Size=2 to FILTERMAX
int32_t x1[30];   // one copy of data in MACQ
uint32_t I1;       // index to oldest
int32_t LPFSum1;  // sum of the last Size samples
int32_t sigma1;
int32_t snr1;      // signal to noise ratio
static void LPF_Init(int32_t initial, int32_t size) { 
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
static int32_t LPF_Calc(int32_t newdata) {
  if(I1 == 0){
    I1 = Size1-1;              // wrap
  } else{
    I1--;                     // make room for data
  }
  LPFSum1 = LPFSum1+newdata-x1[I1];   // subtract oldest, add newest
  x1[I1] = newdata;     // save new data
  return LPFSum1/Size1;
}

static void DisplayAll() {
    char* messages[] = {
        "P1-Low: ",
        "P1-Hi:  ",
        "T-Low:  ",
        "T-Hi:   ",
        "P2-Low: ",
        "P2-Hi:  ",
    };
    OS_bWait(&LCD_Mutex);
    ST7735_FillScreen(ST7735_BLACK);
    for (int i = 0; i < NUM_CHANNELS; i++) {
        ST7735_SetCursor(0, (i + 1) * 2);
        ST7735_OutString(messages[i]);
    }
    OS_bSignal(&LCD_Mutex);
    LPF_Init(initTemp, 30);

    while (1) {
        uint32_t data[NUM_CHANNELS];
        for (int i = 0; i < NUM_CHANNELS; i++) {
            data[i] = Fifo_Get(i);
        }
        uint32_t temp = TempLUT[data[2]];
        if (tempIdx < 8000) {
            tempVals[tempIdx] = LPF_Calc(temp);
            tempIdx++;
            UART_OutUDec(temp);
            UART_OutChar(',');
        }  
        if (tempIdx == 8000) {
            UART_OutString("\n\n\n\n\r");
            UART_OutString("------------LPF Values-----------:\n\r");
            for (int i = 0; i < 1000; i++) {};
            for (int i = 0; i < 8000; i++) {
                UART_OutUDec(tempVals[i]);
                UART_OutChar(',');
                for (uint32_t i = 0; i < 4000; i++) {};
            }
            tempIdx++;
        }
    }
}


/*
    DisplayTemp - foreground thread
    Waits for sampled data in FIFO and displays in on LCD
*/
void DisplayTemp() {
    uint32_t temp;
    while (1) {
        temp = Fifo_Get(THERM_LOW_FIFO);
        
		OS_bWait(&LCD_Mutex);
        ST7735_SetCursor(0, 3);
        ST7735_OutString("Current Temperature: ");
        ST7735_SetCursor(0, 4);
		ST7735_OutUDec(TempLUT[temp]);
        OS_bSignal(&LCD_Mutex);

        if (input) {
            initTemp = TempLUT[temp];
            UART_OutString("Init temp: ");
            UART_OutUDec(initTemp);
            UART_OutString("\r\n\n");
            Fifo_Init(THERM_LOW_FIFO);
            OS_AddThread(&DisplayAll, 1);
			OS_SetPerioidcSchedule(1);
            OS_Kill();
        }
    }
}


/*
    DisplayStartMenu - foreground thread
    Waits for an input in fifo
    Changes state of system
*/
void DisplayStartMenu() {
    while (1) {
        ST7735_SetCursor(0, 0);
        OS_bWait(&LCD_Mutex);
        ST7735_OutString("Press any button to ");
        ST7735_SetCursor(0, 1);
        ST7735_OutString("begin calculation");
        OS_bSignal(&LCD_Mutex);
        input = Fifo_Get(INPUT_FIFO); 
        OS_Kill();
    }
}

