#include <ti/devices/msp/msp.h>
#include <string.h>
#include "OS.h"
#include "Display.h"
#include "RA8875.h"
#include "Conversions.h"
#include "DAS.h"
#include "Data.h"

#define ROUND_BOX_W 251
#define ROUND_BOX_H 150

#define CURSOR_V_X1 102
#define CURSOR_V_X0 (102 + ONE_DIG_SIZE)
#define CURSOR_T_X 374
#define CURSOR_Y (BUTTONS_VAL_Y + 65)
#define CURSOR_H 10

static int killCurrentReadings;
static int killConnected;

void DisplayConnected() {
    const char* connectedStr = "Connected";
    const char* unconnectedStr = "Not Connected";
    uint32_t lastCheckedStatus = 0;
    OS_bWait(&LCD_Mutex);
    RA8875_textMode();
    RA8875_textSetCursor(10, 0);
    RA8875_textEnlarge(1);
    RA8875_textTransparent(RA8875_BLACK); 
    RA8875_textWrite(unconnectedStr, strlen(unconnectedStr));
    RA8875_drawCircle(235, 18, 9, RA8875_BLACK);
    RA8875_fillCircle(235, 18, 8, RA8875_RED);
    OS_bSignal(&LCD_Mutex);
    while (1) {
        uint32_t connectionStatus = GPIOA->DIN31_0 & 0x080;
        if (connectionStatus && !lastCheckedStatus) {
            OS_bWait(&LCD_Mutex);
            RA8875_fillRect(0, 0, 300, 39, BCKGRND_COLOR);
            RA8875_textSetCursor(10, 0);
            RA8875_textEnlarge(1);
            RA8875_textTransparent(RA8875_BLACK); 
            RA8875_textWrite(connectedStr, strlen(connectedStr));
            RA8875_drawCircle(175, 18, 9, RA8875_BLACK);
            RA8875_fillCircle(175, 18, 8, RA8875_GREEN);
            OS_bSignal(&LCD_Mutex);
        }
        else if (!connectionStatus && lastCheckedStatus) {
            OS_bWait(&LCD_Mutex);
            RA8875_fillRect(0, 0, 200, 39, BCKGRND_COLOR);
            RA8875_textSetCursor(10, 0);
            RA8875_textEnlarge(1);
            RA8875_textTransparent(RA8875_BLACK); 
            RA8875_textWrite(unconnectedStr, strlen(unconnectedStr));
            RA8875_drawCircle(235, 18, 9, RA8875_BLACK);
            RA8875_fillCircle(235, 18, 8, RA8875_RED);
            OS_bSignal(&LCD_Mutex);
        }
        lastCheckedStatus = connectionStatus;
        OS_Sleep(600);
        if (killConnected) {
            killConnected = 0;
            OS_Kill();
        }
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
#define START_BUTTON_H 50
// Dpad of Rounded Squares with triangles inside
static void DisplayDpad() {
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

static void CoverDpad() {
    uint32_t R_DpadX = SCREEN_W - (SPACING * 5) - DPAD_SIDE;
    uint32_t L_DpadX = R_DpadX - (DPAD_SIDE * 2) - (SPACING * 2);
    uint32_t U_DpadY = BUTTONS_VAL_Y - SPACING - DPAD_SIDE;
    uint32_t D_DpadY = BUTTONS_VAL_Y + SPACING + DPAD_SIDE;
    uint32_t size = (DPAD_SIDE * 3) + (SPACING * 3);
    RA8875_fillRect(L_DpadX, U_DpadY, size, size, BCKGRND_COLOR);
}
static void DisplayInjectate() {
    // Injectate header
    const char* injectateStr = "Injectate";
    RA8875_textMode();
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textEnlarge(2);
    RA8875_textSetCursor(HEADER_X, INJECTATE_Y);
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    uint32_t startButtonX = (SPACING * 2) + ROUND_BOX_W + SPACING;
    uint32_t startButtonY = INJECTATE_SECTION_Y + (SPACING * 3);
    RA8875_fillRoundRect(startButtonX, INJECTATE_SECTION_Y + (SPACING * 3), ROUND_BOX_W, START_BUTTON_H, CORNER_ROUNDNESS, 0x35ee);

    const char* startStr = "START";
    uint32_t startStrSize = 80;
    uint32_t startStrX = startButtonX + ((ROUND_BOX_W - startStrSize) >> 1);
    uint32_t startStrY = startButtonY + (SPACING + 2);
    //RA8875_drawRect(startButtonX, startButtonY, 80, 50, RA8875_BLACK);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(startStrX, startStrY);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(startStr, strlen(startStr));



    // // Volume 
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
   
    // // Temperature 
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
#define READING_VAL_Y 402

#define FOUR_DIG_DEC_SIZE 162
#define FIVE_DIG_DEC_SIZE (FOUR_DIG_DEC_SIZE + 20)

void DisplayCurrentReadings() {
    OS_bWait(&LCD_Mutex);
    // Current ADC Readings
    const char* currentReadingsStr = "Current Readings";
    uint32_t currentReadingsStrY = BUTTON_BOXS_Y + ROUND_BOX_H + SPACING;
    RA8875_textMode();
    RA8875_textSetCursor(HEADER_X, currentReadingsStrY);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(currentReadingsStr, strlen(currentReadingsStr));
    
    // Current pressure 1
    const uint32_t pres1BoxX = (SPACING * 2);
    const char* pres1Str = "Pressure 1";
    uint32_t pressureStrPxSize = 165;
    uint32_t pres1StrX = pres1BoxX + ((ROUND_BOX_W - pressureStrPxSize) >> 1);
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres1StrX, READING_BOXES_Y);
    RA8875_textWrite(pres1Str, strlen(pres1Str)); // Size of string is 165 pixels

    const char* presUnitStr = "(mmHg)";
    const uint32_t presUnitsW = 85;
    uint32_t pres1UnitStrX = (ROUND_BOX_W - 85) >> 1;
    uint32_t presUnitStrY = READING_BOXES_Y + 30;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres1UnitStrX, presUnitStrY);
    RA8875_textWrite(presUnitStr, strlen(presUnitStr)); 

    char pres1Val5DigStr[7] = "---.--";
    char pres1Val4DigStr[6] = "--.--";
    uint32_t pres1Val5DigStrX = (SPACING * 2) + ((ROUND_BOX_W - FIVE_DIG_DEC_SIZE) >> 1);
    uint32_t pres1Val4DigStrX = (SPACING * 2) + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    uint32_t pres1ValStrY = READING_VAL_Y;
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(pres1Val5DigStrX, pres1ValStrY);
    RA8875_textWrite(pres1Val5DigStr, strlen(pres1Val5DigStr));
    //RA8875_drawRoundRect(pres1BoxX, READING_BOXES_Y, ROUND_BOX_W, ROUND_BOX_H, 5, RA8875_BLACK);
    
    // Current temperature
    const char* tempHeaderStr = "Temperature (C)";
    const char* degreeStr = " o";
    uint32_t intermediateLen = strlen(tempHeaderStr) - 2;
    uint32_t tempBoxX = pres1BoxX + ROUND_BOX_W + SPACING;
    uint32_t tempHeaderStrX = tempBoxX + SPACING;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(tempHeaderStrX, READING_BOXES_Y);
    RA8875_textWrite(tempHeaderStr, intermediateLen);

    RA8875_textEnlarge(0);
    RA8875_textSetCursor(tempHeaderStrX + 200, READING_BOXES_Y);
    RA8875_textWrite(degreeStr, 2);

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(tempHeaderStrX + 210, READING_BOXES_Y);
    RA8875_textSetCursor(tempHeaderStrX + 210 + SPACING, READING_BOXES_Y);
    RA8875_textWrite(tempHeaderStr + intermediateLen, 2);


    char tempValStr[6] = "--.--";
    uint32_t tempValStrx = tempBoxX + ((ROUND_BOX_W - FOUR_DIG_DEC_SIZE) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(tempValStrx, READING_VAL_Y);
    RA8875_textWrite(tempValStr, strlen(tempValStr)); // Width of four digit number with decimal is 162 pixes including 6 pixels to either side of the first and last dig

    //RA8875_drawRoundRect(tempBoxX, READING_BOXES_Y, ROUND_BOX_W, ROUND_BOX_H, 5, RA8875_BLACK);


    // Current pressure 2
    const char* pres2Str = "Pressure 2";
    uint32_t pres2BoxX = tempBoxX + ROUND_BOX_W + SPACING;
    uint32_t pres2StrX = pres2BoxX + ((ROUND_BOX_W - pressureStrPxSize) >> 1);

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres2StrX, READING_BOXES_Y);
    RA8875_textWrite(pres2Str, strlen(pres2Str)); // Size of string is 165 pixels

    uint32_t pres2UnitStrX = pres2BoxX + ((ROUND_BOX_W - presUnitsW) >> 1);
    uint32_t pres2UnitStrY = READING_BOXES_Y + 30;
    RA8875_textEnlarge(1);
    RA8875_textSetCursor(pres2UnitStrX, presUnitStrY);
    RA8875_textWrite(presUnitStr, strlen(presUnitStr)); 
    
    char pres2Val5DigStr[7] = "---.--";
    char pres2Val4DigStr[6] = "--.--";
    uint32_t pres2Val5DigStrX = pres2BoxX + ((ROUND_BOX_W - (FIVE_DIG_DEC_SIZE)) >> 1);
    uint32_t pres2Val4DigStrX = pres2BoxX + ((ROUND_BOX_W - (FOUR_DIG_DEC_SIZE)) >> 1);
    RA8875_textEnlarge(3);
    RA8875_textSetCursor(pres2Val5DigStrX, READING_VAL_Y);
    RA8875_textWrite(pres2Val5DigStr, strlen(pres2Val5DigStr));

    //RA8875_drawRoundRect(pres2BoxX, READING_BOXES_Y, ROUND_BOX_W, ROUND_BOX_H,5, RA8875_BLACK);
    
    OS_bSignal(&LCD_Mutex);
    while (1) {
        OS_bWait(&newVals.ready);
        if (killCurrentReadings) {
            killCurrentReadings = 0;
            OS_Kill();
        }

        OS_bWait(&LCD_Mutex);
        RA8875_textEnlarge(3);

        if (newVals.p1 > 9999) {
            RA8875_textColor(RA8875_BLACK, BCKGRND_COLOR);
            FiveDigUIntToFixedStr(newVals.p1, pres1Val5DigStr);
            RA8875_textSetCursor(pres1Val5DigStrX, READING_VAL_Y);
            RA8875_textWrite(pres1Val5DigStr, strlen(pres1Val5DigStr));
        }
        else {
            RA8875_fillRect(pres1Val5DigStrX, pres1ValStrY, FIVE_DIG_DEC_SIZE + 10, 60, BCKGRND_COLOR);
            RA8875_textTransparent(RA8875_BLACK);
            FourDigUIntToFixedStr(newVals.p1, pres1Val4DigStr);
            RA8875_textSetCursor(pres1Val4DigStrX, READING_VAL_Y);
            RA8875_textWrite(pres1Val4DigStr, strlen(pres1Val4DigStr));
        }
        
        if (newVals.p2 > 9999) {
            RA8875_textColor(RA8875_BLACK, BCKGRND_COLOR);
            FiveDigUIntToFixedStr(newVals.p2, pres2Val5DigStr);
            RA8875_textSetCursor(pres2Val5DigStrX, READING_VAL_Y);
            RA8875_textWrite(pres2Val5DigStr, strlen(pres2Val5DigStr));
        }
        else {
            RA8875_fillRect(pres2Val5DigStrX, READING_VAL_Y, FIVE_DIG_DEC_SIZE + 10, 60, BCKGRND_COLOR);
            RA8875_textTransparent(RA8875_BLACK);
            FourDigUIntToFixedStr(newVals.p2, pres2Val4DigStr);
            RA8875_textSetCursor(pres2Val4DigStrX, READING_VAL_Y);
            RA8875_textWrite(pres2Val4DigStr, strlen(pres2Val4DigStr));
        }

        RA8875_textColor(RA8875_BLACK, BCKGRND_COLOR);
        FourDigUIntToFixedStr(newVals.temp, tempValStr);
        RA8875_textSetCursor(tempValStrx, READING_VAL_Y);
        RA8875_textWrite(tempValStr, strlen(tempValStr));

    
        OS_bSignal(&LCD_Mutex);
    }
}

void sendNewVals(uint32_t p1, uint32_t p2, uint32_t temp) {
    newVals.p1 = p1;
    newVals.p2 = p2;
    newVals.temp = temp;
    OS_bSignal(&newVals.ready);
}

static enum BUTTON mode = NULL_INPUT;
static uint32_t highlighted_dig = 0;
static uint32_t InjectVol = 10;
static uint32_t InjectTemp = 0;
static char volStr[3] = "10";
static char tempStr[2] = "0";

static void DisplayNavigation(enum BUTTON input) {
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
                    InjectVol += 10;
                    volStr[0] = volStr[0] + 1;
                }                    
                RA8875_textSetCursor(CURSOR_V_X1, BUTTONS_VAL_Y);
            }
            else {
                if (volStr[1] >= '9') {
                    return;
                }
                else {
                    InjectVol += 1;
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
                if (InjectVol <= 10) {
                    InjectVol = 1;
                    volStr[0] = '0';
                    volStr[1] = '1';
                }
                else {
                    InjectVol -= 10;
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
                    InjectVol -= 1;
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
            CoverDpad();
            mode = NULL_INPUT;
        }
    }
    else if (mode == TEMP_BUTTON) {
        if (input == UP_BUTTON) {
            if (InjectTemp == 9) {
                return;
            }
            else {
                InjectTemp += 1;
                tempStr[0] = tempStr[0] + 1;
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_T_X, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(tempStr, 1);
        }
        else if (input == DOWN_BUTTON) {
            if (InjectTemp == 0) {
                return;
            }
            else {
                InjectTemp -= 1;
                tempStr[0] = tempStr[0] - 1;
            }
            RA8875_textEnlarge(3);
            RA8875_textSetCursor(CURSOR_T_X, BUTTONS_VAL_Y);
            RA8875_textColor(RA8875_BLACK, INJECTATE_COLOR);
            RA8875_textWrite(tempStr, 1);
        }
        else {
            RA8875_fillRect(CURSOR_T_X, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, INJECTATE_COLOR);
            CoverDpad();
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
    OS_bWait(&LCD_Mutex);
    RA8875_drawLine(0, 40, 800, 40, RA8875_BLACK);

    DisplayInjectate();
    OS_bSignal(&LCD_Mutex);
    

    while (1) {
        uint32_t input = Fifo_Get(INPUT);
        OS_bWait(&LCD_Mutex);
        if (mode) {
            DisplayNavigation(input);
        }
        else if (input == START_BUTTON) {
            OS_SetPerioidcSchedule(1);
            killCurrentReadings = 1;
            killConnected = 1;
            OS_bSignal(&newVals.ready);

            // Stop initial readings data transfer to
            startTransfer(InjectTemp, InjectVol);
            //signalAllDataFifos();

            //start uart data transfer
            OS_bSignal(&LCD_Mutex);
            OS_Kill();
        }
        else if (input == VOLUME_BUTTON) { 
            DisplayDpad();
            mode = VOLUME_BUTTON;
            highlighted_dig = 0;
            RA8875_fillRect(CURSOR_V_X0, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);

        }
        else if (input == TEMP_BUTTON) {
            DisplayDpad();
            mode = TEMP_BUTTON;
            RA8875_fillRect(CURSOR_T_X, CURSOR_Y, ONE_DIG_SIZE, CURSOR_H, RA8875_BLACK);
        }
        OS_bSignal(&LCD_Mutex);
    }
}