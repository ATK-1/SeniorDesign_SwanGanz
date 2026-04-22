#include <ti/devices/msp/msp.h>
#include <string.h>
#include "OS.h"
#include "Display.h"
//#include "LUT.h"
#include "RA8875.h"
#include "DAS.h"
#include "DataTransfer.h"

#define NUM_CHANNELS 6

#define SCREEN_W 800
#define SCREEN_H 480

#define ROUND_BOX_W 251
#define ROUND_BOX_H 150
#define CORNER_ROUNDNESS 5
#define SPACING 5

#define UT_COLOR 0xBAA0
#define INJECTATE_COLOR 0x8DDE
#define BCKGRND_COLOR 0xE7BF

#define CURSOR_V_X1 102
#define CURSOR_V_X0 (102 + ONE_DIG_SIZE)
#define CURSOR_T_X 374
#define CURSOR_Y (BUTTONS_VAL_Y + 65)
#define CURSOR_H 10

Sema4_t LCD_Mutex;
static int input;
uint32_t Started;
uint32_t OtherButton;



void DisplayInit() {
    if (!RA8875_begin(RA8875_800x480)) {
        while(1) {} // LCD Initialization failed
    }
    RA8875_displayOn(1);
    RA8875_setGPIOX(1);
    RA8875_PWM1config(1, 0);
    RA8875_PWM1out(255);
     
    RA8875_fillScreen(BCKGRND_COLOR);
    

    OS_InitSemaphore(&LCD_Mutex, 1);
}


#define MEASURING_TIME_MS 40000
void displayMeasuring() {
    RA8875_fillScreen(BCKGRND_COLOR);

    uint32_t headerRectX = SPACING;
    uint32_t headerRectY = SPACING;
    uint32_t headerRectW = SCREEN_W - SPACING * 2;
    uint32_t headerRectH = 80;
    RA8875_fillRoundRect(headerRectX, headerRectY, headerRectW, headerRectH, CORNER_ROUNDNESS, UT_COLOR);
    
    const char* thermoHeaderStr = "Thermodilution Calculation";
    const uint32_t thermoHeaderStrW = 620;
    const uint32_t thermoHeaderStrH = 33;
    const uint32_t thermoHeaderStrX = headerRectX + ((headerRectW - thermoHeaderStrW) >> 1);
    const uint32_t thermoHeaderStrY = ((headerRectH - thermoHeaderStrH) >> 1) - SPACING;
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textSetCursor(thermoHeaderStrX, thermoHeaderStrY);
    RA8875_textWrite(thermoHeaderStr, strlen(thermoHeaderStr));

    const char* progressStr = "Progress...";
    const uint32_t progressX = SPACING * 5;
    const uint32_t progressStrY = headerRectY + headerRectH + (SPACING * 15);
    const uint32_t progressStrH = 31;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(progressX, progressStrY);
    RA8875_textWrite(progressStr, strlen(progressStr));

    const uint32_t progressBarY = progressStrY + progressStrH + SPACING;
    const uint32_t progressBarW = SCREEN_W - (progressX * 2);
    const uint32_t progressBarH = 35;
    RA8875_fillRect(progressX, progressBarY, progressBarW, progressBarH, 0xbdf7);
    
    const char* injectateStr = "Injectate";
    const uint32_t injectateStrX = (SCREEN_W - (progressBarW >> 1)) >> 1;
    const uint32_t injecetateStrY = progressBarY + progressBarH + (SPACING * 15);
    const uint32_t injecetateStrH = 31;
    RA8875_textSetCursor(injectateStrX, injecetateStrY);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    const uint32_t InjBarW = progressBarW >> 1;
    const uint32_t InjBarH = progressBarH << 1;
    const uint32_t InjBarX = injectateStrX;
    const uint32_t InjBarY = injecetateStrY + injecetateStrH + SPACING;
    RA8875_fillRect(InjBarX, InjBarY, InjBarW, InjBarH, 0xbdf7);

    uint32_t currProgX = progressX + 3;
    uint32_t endProgX = progressX + progressBarW - 3;
    uint32_t msPerProgLine = MEASURING_TIME_MS / (endProgX - currProgX);

    uint32_t currInjX = InjBarX + 3;
    uint32_t endInjX = InjBarX + InjBarW - 3;
    uint32_t msPerInjLine = 8000 / (endInjX - currProgX);

    uint32_t prevProgTime = OS_MsTime();
    uint32_t prevInjTime = prevProgTime;
    while (1) {
        uint32_t currTime = OS_MsTime();
        int32_t diffProgTime = currTime - prevProgTime;
        int32_t diffInjTime = currTime - prevInjTime;

        while (diffProgTime >= msPerProgLine && currProgX < endProgX) {
            RA8875_drawLine(currProgX, progressBarY + 2, currProgX, progressBarY + progressBarH - 3, RA8875_BLACK);
            currProgX++;
            diffProgTime -= msPerProgLine;
            prevProgTime += msPerProgLine;
        }

        while (diffInjTime >= msPerInjLine && currInjX < endInjX) {
            RA8875_drawLine(currInjX, InjBarY + 2, currInjX, InjBarY + InjBarH - 3, INJECTATE_COLOR);
            currInjX++;
            diffInjTime -= msPerInjLine;
            prevInjTime += msPerInjLine;
        }
        if ((currProgX < endProgX) || (currInjX < endInjX)) {
            OS_Sleep(50);
        }
        else {
            killTransfer();
            signalAllDataFifos();
            OS_Kill();
        }        
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
#define DPAD_SIDE 60

// Dpad of Rounded Squares with triangles inside
static void displayDpad() {
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

static void coverDpad() {
    uint32_t R_DpadX = SCREEN_W - (SPACING * 5) - DPAD_SIDE;
    uint32_t L_DpadX = R_DpadX - (DPAD_SIDE * 2) - (SPACING * 2);
    uint32_t U_DpadY = BUTTONS_VAL_Y - SPACING - DPAD_SIDE;
    uint32_t D_DpadY = BUTTONS_VAL_Y + SPACING + DPAD_SIDE;
    uint32_t size = (DPAD_SIDE * 3) + (SPACING * 3);
    RA8875_fillRect(L_DpadX, U_DpadY, size, size, BCKGRND_COLOR);
}
static void displayInjectate() {
    // Injectate header
    const char* injectateStr = "Injectate";
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textMode();
    RA8875_textEnlarge(2);
    RA8875_textSetCursor(HEADER_X, INJECTATE_Y);
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    uint32_t startButtonX = (SPACING * 2) + ROUND_BOX_W + SPACING;
    uint32_t startButtonY = INJECTATE_SECTION_Y + (SPACING * 3);
    RA8875_fillRoundRect(startButtonX, INJECTATE_SECTION_Y + (SPACING * 3), ROUND_BOX_W, 50, CORNER_ROUNDNESS, 0x35ee);

    const char* startStr = "START";
    uint32_t startStrSize = 80;
    uint32_t startStrX = startButtonX + ((ROUND_BOX_W - startStrSize) >> 1);
    uint32_t startStrY = startButtonY + (SPACING + 2);
    //RA8875_drawRect(startButtonX, startButtonY, 80, 50, RA8875_BLACK);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(startStrX, startStrY);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(startStr, strlen(startStr));



    // Volume 
    RA8875_fillRoundRect(VOLUME_BOX_X, BUTTON_BOXS_Y, ROUND_BOX_W, ROUND_BOX_H, CORNER_ROUNDNESS, INJECTATE_COLOR);
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
    RA8875_fillRoundRect(tempBoxX, BUTTON_BOXS_Y, ROUND_BOX_W, ROUND_BOX_H, CORNER_ROUNDNESS, INJECTATE_COLOR);
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

    //displayDpad();

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
    RA8875_textSetCursor(471, READING_BOXES_Y);
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
    uint32_t pres2StrX = pres2BoxX + ((ROUND_BOX_W - pressureStrPxSize) >> 1);

    //RA8875_drawRoundRect(522, 330, 251, 150, 5, RA8875_BLACK);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres2StrX, READING_BOXES_Y);
    //RA8875_textSetCursor(522 + 43, 330);
    RA8875_textWrite(pres2Str, strlen(pres2Str)); // Size of string is 165 pixels

    const char* exStr3 = "90.12";
    uint32_t exStr3X = pres2BoxX + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(exStr3X, READING_VAL_Y);
    //RA8875_textSetCursor(522 + 45, 380);
    RA8875_textWrite(exStr3, strlen(exStr3));
}

static enum BUTTON mode = NULL_INPUT;
static uint32_t highlighted_dig = 0;
static uint32_t currentVol = 10;
static uint32_t currentTemp = 0;
static char volStr[3] = "10";
static char tempStr[2] = "0";

static void displayNavigation(enum BUTTON input) {
    if (mode == VOLUME_BUTTON) {
        if (highlighted_dig == 1 && input == RIGHT_BUTTON) {
            RA8875_fillRect(CURSOR_V_X1, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, INJECTATE_COLOR);
            RA8875_fillRect(CURSOR_V_X0, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);
            highlighted_dig = 0;
        }
        else if (highlighted_dig == 0 && input == LEFT_BUTTON) {
            RA8875_fillRect(CURSOR_V_X0, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, INJECTATE_COLOR);
            RA8875_fillRect(CURSOR_V_X1, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);
            highlighted_dig = 1;
        }
        else if (input == UP_BUTTON) {
            if (highlighted_dig == 1) {
                if (volStr[0] == '9') {
                    return;
                }
                else {
                    currentVol += 10;
                    volStr[0] = volStr[0] + 1;
                }                    
                RA8875_textSetCursor(CURSOR_V_X1, BUTTONS_VAL_Y);
            }
            else {
                if (volStr[1] >= '9') {
                    return;
                }
                else {
                    currentVol += 1;
                    volStr[1] = volStr[1] + 1;
                }
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_V_X1, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(volStr, 2); 
        
        }
        else if (input == DOWN_BUTTON) {
            if (highlighted_dig == 1) {
                if (currentVol <= 10) {
                    currentVol = 1;
                    volStr[0] = '0';
                    volStr[1] = '1';
                }
                else {
                    currentVol -= 10;
                    volStr[0] = volStr[0] - 1;
                }
            }
            else {
                if (volStr[0] == 0 && volStr[1] == '1') {
                    return;
                }
                else if (volStr[0] != 0 && volStr[1] == '0') {
                    return;
                }
                else {
                    currentVol -= 1;
                    volStr[1] = volStr[1] - 1;
                }
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_V_X1, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(volStr, 2); 
        }
        else {
            uint32_t x = highlighted_dig ? CURSOR_V_X1 : CURSOR_V_X0;
            RA8875_fillRect(x, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, INJECTATE_COLOR);
            coverDpad();
            mode = NULL_INPUT;
        }
    }
    else if (mode == TEMP_BUTTON) {
        if (input == UP_BUTTON) {
            if (currentTemp == 9) {
                return;
            }
            else {
                currentTemp += 1;
                tempStr[0] = tempStr[0] + 1;
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_T_X, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(tempStr, 1);
        }
        else if (input == DOWN_BUTTON) {
            if (currentTemp == 0) {
                return;
            }
            else {
                currentTemp -= 1;
                tempStr[0] = tempStr[0] - 1;
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_T_X, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(tempStr, 1);
        }
        else {
            RA8875_fillRect(CURSOR_T_X, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, INJECTATE_COLOR);
            coverDpad();
            mode = NULL_INPUT;
        }
    }
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
        uint32_t input = Fifo_Get(INPUT_FIFO);
        if (mode) {
            displayNavigation(input);
        }
        else if (input == START_BUTTON) {
            OS_SetPerioidcSchedule(1);
            // Stop initial readings data transfer
            killInitReadings();
            //signalAllDataFifos();

            //start uart data transfer
            
            OS_Kill();
        }
        else if (input == VOLUME_BUTTON) { 
            displayDpad();
            mode = VOLUME_BUTTON;
            highlighted_dig = 0;
            RA8875_fillRect(CURSOR_V_X0, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);

        }
        else if (input == TEMP_BUTTON) {
            displayDpad();
            mode = TEMP_BUTTON;
            RA8875_fillRect(CURSOR_T_X, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);
        }
    }
}



