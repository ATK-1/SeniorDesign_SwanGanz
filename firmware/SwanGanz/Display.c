#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"
#include "LUT.h"
#include "RA8875.h"

#define NUM_CHANNELS 6
Sema4_t LCD_Mutex;
static int input;


void DisplayInit0() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    OS_InitSemaphore(&LCD_Mutex, 1);
}

void DisplayInit() {
    if (!RA8875_begin(RA8875_800x480)) {
        while(1) {
            // writeCommand(0x35);
        }
    }
    fillScreen(RA8875_MAGENTA);
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
        for (int i = 0; i < NUM_CHANNELS; i++) {
            ST7735_FillRect(78 , (i + 1) * 20, 11, 7, ST7735_BLACK);
            ST7735_SetCursor(10, (i + 1) * 2);
			if (i == 2) {
				ST7735_OutUDec(TempLUT[data[i]]);
			}
			else {
            	ST7735_OutUDec(data[i]);
			}
        }
    }
}


/*
    DisplayTemp - foreground thread
    Waits for sampled data in FIFO and displays in on LCD
*/
void DisplayTemp() {
    while (1) {
        // uint32_t temp = Fifo_Get(THERM_LOW_FIFO);
        
		// OS_bWait(&LCD_Mutex);
        // ST7735_SetCursor(0, 3);
        // ST7735_OutString("Current Temperature: ");
        // ST7735_SetCursor(0, 4);
		// ST7735_OutUDec(TempLUT[temp]);
        // OS_bSignal(&LCD_Mutex);

        // if (input) {
        //     OS_AddThread(&DisplayAll, 1);
		// 	OS_SetPerioidcSchedule(1);
        //     OS_Kill();
        // }
    }
}


/*
    DisplayStartMenu - foreground thread
    Waits for an input in fifo
    Changes state of system
*/
void DisplayStartMenu() {
    while (1) {
        // ST7735_SetCursor(0, 0);
        // OS_bWait(&LCD_Mutex);
        // ST7735_OutString("Press any button to ");
        // ST7735_SetCursor(0, 1);
        // ST7735_OutString("begin calculation");
        // OS_bSignal(&LCD_Mutex);
        // input = Fifo_Get(INPUT_FIFO); 
        // OS_Kill();
    }
}

