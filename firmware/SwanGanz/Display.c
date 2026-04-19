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


static void displayConnected() {
    RA8875_textMode();
    RA8875_textSetCursor(10, 0);
    const char* connectedStr = "Connected";
    const char* unconnectedStr = "Not Connected";
    RA8875_textEnlarge(1);
    RA8875_textTransparent(RA8875_BLACK); 
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

#define SCREEN_W 800

#define ROUND_BOX_W 251
#define ROUND_BOX_H 150
#define CORNER_ROUNDNESS 5

#define INJECTATE_SECTION_Y 40
#define INJECTATE_SECTION_H 240
#define HEADER_X 7
#define INJECTATE_Y 45
#define VOLUME_BOX_X 10
#define BUTTON_BOXS_Y 120
#define BUTTONS_VAL_Y 175
#define V_HEADER_SIZE 170
#define TWO_DIG_SIZE 67
#define ONE_DIG_SIZE 34
#define SPACING 5
#define DPAD_SIDE 60
static void displayInjectate() {
    // Injectate header
    const char* injectateStr = "Injectate";
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textMode();
    RA8875_textEnlarge(2);
    RA8875_textSetCursor(HEADER_X, INJECTATE_Y);
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    // Volume 
    RA8875_drawRoundRect(VOLUME_BOX_X, BUTTON_BOXS_Y, ROUND_BOX_W, ROUND_BOX_H, CORNER_ROUNDNESS, RA8875_BLACK);

    const char* volumeHeaderStr = "Volume (mL)";
    uint32_t vHeaderStrX = VOLUME_BOX_X + ((ROUND_BOX_W - V_HEADER_SIZE) >> 1);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(vHeaderStrX, BUTTON_BOXS_Y + SPACING);                            
    RA8875_textWrite(volumeHeaderStr, strlen(volumeHeaderStr)); 

    const char* volumeValStr = "10";
    uint32_t volValStrX = VOLUME_BOX_X + ((ROUND_BOX_W - TWO_DIG_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(volValStrX, BUTTONS_VAL_Y);
    RA8875_textWrite(volumeValStr, strlen(volumeValStr)); 
   
    // Temperature 
    uint32_t tempBoxX = VOLUME_BOX_X + ROUND_BOX_W + SPACING;
    RA8875_drawRoundRect(tempBoxX, BUTTON_BOXS_Y, ROUND_BOX_W, ROUND_BOX_H, CORNER_ROUNDNESS, RA8875_BLACK);

    const char* tempHeaderStr = "Temperature (C)";
    const char* degreeStr = " o";
    uint32_t intermediateLen = strlen(tempHeaderStr) - 2;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(tempBoxX + SPACING, BUTTON_BOXS_Y + SPACING);
    RA8875_textWrite(tempHeaderStr, intermediateLen);
    RA8875_textEnlarge(0);
    RA8875_textSetCursor(471, BUTTON_BOXS_Y + SPACING); // 471 is where the parenthesis end
    RA8875_textWrite(degreeStr, 2);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(486, BUTTON_BOXS_Y + SPACING); // 486 is where the degree symbol ends
    RA8875_textWrite(tempHeaderStr + intermediateLen, 2);

    const char* tempValue = "0";
    uint32_t tempValueX = tempBoxX + ((ROUND_BOX_W - ONE_DIG_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(tempValueX, BUTTONS_VAL_Y);
    RA8875_textWrite(tempValue, 1);

    // Dpad of Rounded Squares with triangles inside

    // Right Dpad
    uint32_t halfBox = DPAD_SIDE >> 1;
    uint32_t R_DpadX = SCREEN_W - (SPACING * 5) - DPAD_SIDE;
    uint32_t R_TriX1 = R_DpadX + DPAD_SIDE - (SPACING * 2);
    uint32_t R_TriY1 = BUTTONS_VAL_Y + halfBox;
    uint32_t R_TriX2 = R_DpadX + halfBox;
    uint32_t R_TriY2 = BUTTONS_VAL_Y + (SPACING * 2);
    uint32_t R_TriX3 = R_TriX2;
    uint32_t R_TriY3 = BUTTONS_VAL_Y + DPAD_SIDE - (SPACING * 2);
    RA8875_fillRoundRect(R_DpadX, BUTTONS_VAL_Y, DPAD_SIDE, DPAD_SIDE, CORNER_ROUNDNESS, RA8875_BLACK); 
    RA8875_fillTriangle(R_TriX1, R_TriY1, R_TriX2, R_TriY2, R_TriX3, R_TriY3, RA8875_WHITE);
    
    // Left Dpad
    uint32_t L_DpadX = R_DpadX - (DPAD_SIDE * 2) - (SPACING * 2);
    uint32_t L_TriX1 = L_DpadX + (SPACING * 2);
    uint32_t L_TriY1 = BUTTONS_VAL_Y + halfBox;
    uint32_t L_TriX2 = L_DpadX + halfBox;
    uint32_t L_TriY2 = BUTTONS_VAL_Y + (SPACING * 2);
    uint32_t L_TriX3 = L_TriX2;
    uint32_t L_TriY3 = BUTTONS_VAL_Y + DPAD_SIDE - (SPACING * 2);
    RA8875_fillRoundRect(L_DpadX, BUTTONS_VAL_Y, DPAD_SIDE, DPAD_SIDE, CORNER_ROUNDNESS, RA8875_BLACK); // Left
    RA8875_fillTriangle(L_TriX1, L_TriY1, L_TriX2, L_TriY2, L_TriX3, L_TriY3, RA8875_WHITE);

    // Up Dpad
    uint32_t U_DpadX = L_DpadX + DPAD_SIDE + SPACING;
    uint32_t U_DpadY = BUTTONS_VAL_Y - SPACING - DPAD_SIDE;
    uint32_t U_TriX1 = U_DpadX + (DPAD_SIDE >> 1);
    uint32_t U_TriY1 = U_DpadY + (SPACING * 2);
    uint32_t U_TriX2 = U_DpadX + (SPACING * 2);
    uint32_t U_TriY2 = U_DpadY + (DPAD_SIDE >> 1);
    uint32_t U_TriX3 = U_DpadX + DPAD_SIDE - (SPACING * 2);
    uint32_t U_TriY3 = U_TriY2;
    RA8875_fillRoundRect(U_DpadX, U_DpadY, DPAD_SIDE, DPAD_SIDE, CORNER_ROUNDNESS, RA8875_BLACK); 
    RA8875_fillTriangle(U_TriX1, U_TriY1, U_TriX2, U_TriY2, U_TriX3, U_TriY3, RA8875_WHITE);
    // RA8875_fillRoundRect(650, 65, 60, 60, 5, RA8875_BLACK); // Top
    // RA8875_fillTriangle(680, 75, 660, 95, 700, 95, RA8875_WHITE);

    // Down Dpad
    uint32_t D_DpadX = U_DpadX;
    uint32_t D_DpadY = BUTTONS_VAL_Y + SPACING + DPAD_SIDE;
    uint32_t D_TriX1 = D_DpadX + (DPAD_SIDE >> 1);
    uint32_t D_TriY1 = D_DpadY + DPAD_SIDE - (SPACING * 2);
    uint32_t D_TriX2 = D_DpadX + (SPACING * 2);
    uint32_t D_TriY2 = D_DpadY + (DPAD_SIDE >> 1);
    uint32_t D_TriX3 = D_DpadX + DPAD_SIDE - (SPACING * 2);
    uint32_t D_TriY3 = D_TriY2;
    RA8875_fillRoundRect(D_DpadX, D_DpadY, DPAD_SIDE, DPAD_SIDE, CORNER_ROUNDNESS, RA8875_BLACK); 
    RA8875_fillTriangle(D_TriX1, D_TriY1, D_TriX2, D_TriY2, D_TriX3, D_TriY3, RA8875_WHITE);
    // RA8875_fillRoundRect(650, 195, 60, 60, 5, RA8875_BLACK); // Down
    // RA8875_fillTriangle(680, 245, 660, 225, 700, 225, RA8875_WHITE);

}

#define READING_BOXES_Y 330
#define READING_VAL_Y 380

#define FOUR_DIG_DEC_SIZE 162
static void displayCurrentReadings() {
    // Current ADC Readings
    const char* currentReadingsStr = "Current Readings";
    uint32_t currentReadingsStrY = BUTTON_BOXS_Y + ROUND_BOX_H + SPACING;
    RA8875_textMode();
    RA8875_textSetCursor(HEADER_X, currentReadingsStrY);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(currentReadingsStr, strlen(currentReadingsStr));
    
    // Current pressure 1
    const char* pres1Str = "Pressure 1";
    uint32_t pressureStrPxSize = 165;
    uint32_t pres1StrX = (SPACING * 2) + ((ROUND_BOX_W - pressureStrPxSize) >> 1);
    //RA8875_drawRoundRect(10, 330, 251, 150, 5, RA8875_BLACK);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres1StrX, READING_BOXES_Y);
    // RA8875_textSetCursor(10 + 43, 330);
    RA8875_textWrite(pres1Str, strlen(pres1Str)); // Size of string is 165 pixels

    const char* exStr1 = "12.34";
    uint32_t exStr1X = (SPACING * 2) + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    uint32_t exStr1Y = READING_VAL_Y;
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(exStr1X, exStr1Y);
    // RA8875_textSetCursor(10 + 45, 380);
    RA8875_textWrite(exStr1, strlen(exStr1));
    
    // Current temperature
    const char* tempHeaderStr = "Temperature (C)";
    const char* degreeStr = " o";
    uint32_t intermediateLen = strlen(tempHeaderStr) - 2;
    uint32_t tempBoxX = VOLUME_BOX_X + ROUND_BOX_W + SPACING;
    RA8875_textEnlarge(1);
    //RA8875_drawRoundRect(266, 330, 251, 150, 5, RA8875_BLACK); //just for centering
    RA8875_textSetCursor(tempBoxX, READING_BOXES_Y);
    // RA8875_textSetCursor(271, 330);
    RA8875_textWrite(tempHeaderStr, intermediateLen);
    RA8875_textEnlarge(0);
    RA8875_textSetCursor(471, READING_VAL_Y);
    //RA8875_textSetCursor(471, 330);
    RA8875_textWrite(degreeStr, 2);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(486, READING_BOXES_Y);
    // RA8875_textSetCursor(486, 330);
    RA8875_textWrite(tempHeaderStr + intermediateLen, 2);

    const char* exStr2 = "56.78";
    uint32_t exStr2X = tempBoxX + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(exStr2X, READING_VAL_Y);
    // RA8875_textSetCursor(266 + 45, 380);
    RA8875_textWrite(exStr2, strlen(exStr2)); // Width of four digit number with decimal is 162 pixes including 6 pixels to either side of the first and last dig

    // Current pressure 2
    const char* pres2Str = "Pressure 2";
    uint32_t pres2BoxX = tempBoxX + ROUND_BOX_W + SPACING;

    //RA8875_drawRoundRect(522, 330, 251, 150, 5, RA8875_BLACK);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres2BoxX, READING_BOXES_Y);
    //RA8875_textSetCursor(522 + 43, 330);
    RA8875_textWrite(pres2Str, strlen(pres2Str)); // Size of string is 165 pixels

    const char* exStr3 = "90.12";
    uint32_t exStr3X = pres2BoxX + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(exStr3X, READING_VAL_Y);
    //RA8875_textSetCursor(522 + 45, 380);
    RA8875_textWrite(exStr3, strlen(exStr3));
}
/*
    DisplayStartMenu - foreground thread
    Waits for an input in fifo
    Changes state of system
*/
void DisplayStartMenu() {
    displayConnected();

    //RA8875_drawRoundRect(0, 40, 800, 240, 5, RA8875_BLACK);
    RA8875_drawLine(0, 40, 800, 40, RA8875_BLACK);

    displayInjectate();
    displayCurrentReadings();
    
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

