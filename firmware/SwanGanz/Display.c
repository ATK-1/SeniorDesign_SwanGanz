#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"
#include "LUT.h"
#include "../inc/RTOS_UART.h"


#define NUM_CHANNELS 6
Sema4_t LCD_Mutex;
static uint32_t input;
static uint32_t initTemp;
static uint32_t tempVals[4000];
static uint32_t tempIdx;


void DisplayInit() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    OS_InitSemaphore(&LCD_Mutex, 1);
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

    while (1) {
        uint32_t data[NUM_CHANNELS];
        for (int i = 0; i < NUM_CHANNELS; i++) {
            data[i] = Fifo_Get(i);
        }
        uint32_t temp = TempLUT[data[2]];
        if (tempIdx < 4000) {
            tempVals[tempIdx] = temp;
            tempIdx++;
            UART_OutUDec(temp);
            UART_OutChar(',');
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

