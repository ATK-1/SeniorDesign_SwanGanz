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
    RA8875_drawRoundRect(0, 40, 800, 240, 5, RA8875_BLACK);

    // Injectate header
    const char* injectateStr = "Injectate";
    RA8875_textMode();
    RA8875_textEnlarge(2);
    RA8875_textSetCursor(7, 45);
    RA8875_textTransparent(RA8875_BLACK); //test if needed
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    // Volume 
    RA8875_drawRect(10, 120, 251, 150, RA8875_BLACK);
    // Volume header
    const char* volumeHeaderStr = "Volume (mL)";
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(50, 125);                            
    RA8875_textWrite(volumeHeaderStr, strlen(volumeHeaderStr)); // Width of headerStr is 170 pixels
    // Volume value
    const char* volumeValStr = "10";
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(92, 175);
    RA8875_textWrite(volumeValStr, strlen(volumeValStr)); // width of two digit value is 67 pixels 

    // Temperature 
    RA8875_drawRect(266, 120, 251, 150, RA8875_BLACK);
    // Temperature Header
    const char* tempHeaderStr = "Temperature (C)";
    const char* degreeStr = " o";
    uint32_t intermediateLen = strlen(tempHeaderStr) - 2;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(271, 125);
    RA8875_textWrite(tempHeaderStr, intermediateLen);
    RA8875_textEnlarge(0);
    RA8875_textSetCursor(471, 125);
    RA8875_textWrite(degreeStr, 2);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(486, 125);
    RA8875_textWrite(tempHeaderStr + intermediateLen, 2);
    // Temperature Value
    const char* tempValue = "0";
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(374, 175);
    RA8875_textWrite(tempValue, 1);

    // Rounded Squares
    RA8875_fillRoundRect(650, 65, 60, 60, 5, RA8875_BLACK); // Top
    RA8875_fillTriangle(680, 75, 660, 95, 700, 95, RA8875_WHITE);

    RA8875_fillRoundRect(715, 130, 60, 60, 5, RA8875_BLACK); // Right
    RA8875_fillTriangle(765, 160, 745, 140, 745, 180, RA8875_WHITE);

    RA8875_fillRoundRect(585, 130, 60, 60, 5, RA8875_BLACK); // Left
    RA8875_fillTriangle(595, 160, 615, 140, 615, 180, RA8875_WHITE);

    RA8875_fillRoundRect(650, 195, 60, 60, 5, RA8875_BLACK); // Down
    RA8875_fillTriangle(680, 245, 660, 225, 700, 225, RA8875_WHITE);
    
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

