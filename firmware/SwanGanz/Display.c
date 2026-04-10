#include <ti/devices/msp/msp.h>
#include <string.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"
#include "LUT.h"
#include "RA8875.h"

#define NUM_CHANNELS 6
Sema4_t LCD_Mutex;
static int input;



void DisplayInit() {
    if (!RA8875_begin(RA8875_800x480)) {
        while(1) {} // LCD Initialization failed
    }
    RA8875_displayOn(1);
    RA8875_setGPIOX(1);
    RA8875_PWM1config(1, 0);
    RA8875_PWM1out(255);
     
    RA8875_fillScreen(0xE7BF);
    

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


void displayConnected() {
    RA8875_textMode();
    RA8875_textSetCursor(10, 0);
    const char* connectedStr = "Connected";
    const char* unconnectedStr = "Not Connected";
    RA8875_textEnlarge(1);
    RA8875_textTransparent(RA8875_BLACK); //test if needed
    uint32_t connected = 0;
    if (connected) {
        RA8875_textWrite(connectedStr, strlen(connectedStr));
        RA8875_drawCircle(175, 18, 9, RA8875_BLACK);
        RA8875_fillCircle(175, 18, 8, RA8875_GREEN);
    }
    else {
        RA8875_textWrite(unconnectedStr, strlen(unconnectedStr));
        RA8875_drawCircle(235, 18, 9, RA8875_BLACK);
        RA8875_fillCircle(235, 18, 8, RA8875_RED);
    }
}
/*
    DisplayStartMenu - foreground thread
    Waits for an input in fifo
    Changes state of system
*/
void DisplayStartMenu() {
    displayConnected();
    RA8875_drawRoundRect(0, 40, 800, 200, 5, RA8875_BLACK);
    //RA8875_fillRoundRect(1, 41, 798, 198, 5 , (uint16_t)0xBFCE7E);

    // Injectate header
    const char* injectateStr = "Injectate";
    RA8875_textMode();
    RA8875_textEnlarge(2);
    RA8875_textSetCursor(7, 45);
    RA8875_textTransparent(RA8875_BLACK); //test if needed
    RA8875_textWrite(injectateStr, strlen(injectateStr));
    // Volume header
    const char* volumeHeaderStr = "Volume (mL)";
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(15, 100);
    RA8875_textWrite(volumeHeaderStr, strlen(volumeHeaderStr));
    // Volume value
    const char* volumeValStr = "10";
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(65, 135);
    RA8875_textWrite(volumeValStr, strlen(volumeValStr));
    // Temperature Header
    const char* tempStr = "0 C";
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

