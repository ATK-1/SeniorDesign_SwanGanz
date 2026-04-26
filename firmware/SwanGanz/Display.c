#include <ti/devices/msp/msp.h>
#include <string.h>
#include "OS.h"
#include "Display.h"
#include "Conversions.h"
#include "RA8875.h"
#include "DAS.h"
#include "Data.h"

#define NUM_CHANNELS 6


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
    OS_InitSemaphore(&CurrReadingsKilled, 0);
    OS_InitSemaphore(&newVals.ready, 0);
}

void DisplayResults() {
    OS_bWait(&LCD_Mutex);

    uint32_t headerRectX = SPACING;
    uint32_t headerRectY = SPACING;
    uint32_t headerRectW = SCREEN_W - SPACING * 2;
    uint32_t headerRectH = 80;
    RA8875_fillRoundRect(headerRectX, headerRectY, headerRectW, headerRectH, CORNER_ROUNDNESS, UT_COLOR);
    
    const char* resultsHeaderStr = "Thermodilution Results";
    const uint32_t resultsHeaderStrW = 525;
    const uint32_t resultsHeaderStrH = 33;
    const uint32_t resultsHeaderStrX = headerRectX + ((headerRectW - resultsHeaderStrW) >> 1);
    const uint32_t resultsHeaderStrY = ((headerRectH - resultsHeaderStrH) >> 1) - SPACING;
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textSetCursor(resultsHeaderStrX, resultsHeaderStrY);
    RA8875_textWrite(resultsHeaderStr, strlen(resultsHeaderStr));

    const char* flowStr = "Flow";
    const uint32_t flowStrW = 50;
    const uint32_t flowStrH = 50;
    const uint32_t flowStrX = 0;
    const uint32_t flowStrY = 100;
    RA8875_textSetCursor(flowStrX, flowStrY);
    RA8875_textWrite(flowStr, strlen(flowStr));
    
    char flowValStr[7] = "000000";
    int64_t flowVal = getFlowRate();
    uint32_t negative1 = !(flowVal & 0xFFFFFFFF00000000) && (flowVal & 0x80000000);
    uint32_t negative2 = (flowVal & 0x8000000000000000) >> 32;
    uint32_t x = flowStrX;
    if (negative1 || negative2) {
        RA8875_textSetCursor(flowStrX, flowStrY + 50);
        RA8875_textWrite("-", 1);
        x += 50;
        if (negative1) {
            flowVal |= 0xFFFFFFFF00000000;
        }
        flowVal = ~flowVal + 1;
    }
    int64ToString(flowVal, flowValStr);
    RA8875_textSetCursor(x, flowStrY + 50);
    RA8875_textWrite(flowValStr, strlen(flowValStr));

    

    const char* initTempStr = "Initial temp of water";
    const uint32_t initTempX = flowStrX;
    const uint32_t initTempY = flowStrY + 100;
    RA8875_textSetCursor(initTempX, initTempY);
    RA8875_textWrite(initTempStr, strlen(initTempStr));

    char initTemp[7] = "------";
    FiveDigUIntToFixedStr(getInitialTemp(), initTemp);
    initTemp[3] = initTemp[2];
    initTemp[2] = '.';
    RA8875_textSetCursor(initTempX, initTempY + 50);
    RA8875_textWrite(initTemp, strlen(initTemp));

    const char* AOCStr = "Area under of curve:";
    const uint32_t AOCStrX = flowStrX;
    const uint32_t AOCStrY = flowStrY + 200;
    RA8875_textSetCursor(AOCStrX, AOCStrY);
    RA8875_textWrite(AOCStr, strlen(AOCStr));

    char AOCValStr[7] = "000000";
    int64_t AOCVal = getAOC();
    negative1 = !(AOCVal & 0xFFFFFFFF00000000) && (AOCVal & 0x80000000);
    negative2 = (AOCVal & 0x8000000000000000) >> 32;
    x = AOCStrX;
    if (negative1 || negative2) {
        RA8875_textSetCursor(x, AOCStrY + 75);
        RA8875_textWrite("-", 1);
        x += 50;
        if (negative1) {
            AOCVal |= 0xFFFFFFFF00000000;
        }
        AOCVal = ~AOCVal + 1;
    }
    int64ToString(AOCVal, AOCValStr);
    RA8875_textSetCursor(x, AOCStrY + 75);
    RA8875_textWrite(AOCValStr, strlen(AOCValStr));


    RA8875_fillRoundRect(SCREEN_W - 260, SCREEN_H - 60, 250, 50, 5, RA8875_RED);
    OS_bSignal(&LCD_Mutex);
    while (1) {
        uint32_t input = Fifo_Get(INPUT);
        if (input == RESET_BUTTON) {
            RA8875_fillScreen(BCKGRND_COLOR);
            __disable_irq();
            DASInit();
            OS_AddThread(&DisplayCurrentReadings, 2);
            OS_AddThread(&DisplayStartMenu, 2);
            OS_AddThread(&DisplayConnected, 2);
            OS_AddThread(&InitReadings, 1);
            OS_SetPerioidcSchedule(0);
            OS_Kill();
        }
    }
}
#define MEASURING_TIME_MS 10000
#define INJECTATE_TIME_MS 8000
void DisplayMeasuring() {
    OS_bWait(&LCD_Mutex);

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
    const uint32_t progressStrY = headerRectY + headerRectH + (SPACING * 4);
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
    const uint32_t injecetateStrY = progressBarY + progressBarH + (SPACING * 4);
    const uint32_t injecetateStrH = 31;
    RA8875_textSetCursor(injectateStrX, injecetateStrY);
    RA8875_textTransparent(RA8875_BLACK);
    RA8875_textWrite(injectateStr, strlen(injectateStr));

    const uint32_t InjBarW = progressBarW >> 1;
    const uint32_t InjBarH = progressBarH;
    const uint32_t InjBarX = injectateStrX;
    const uint32_t InjBarY = injecetateStrY + injecetateStrH + SPACING;
    RA8875_fillRect(InjBarX, InjBarY, InjBarW, InjBarH, 0xbdf7);
    OS_bSignal(&LCD_Mutex);
    
    uint32_t currProgX = progressX + 3;
    uint32_t endProgX = progressX + progressBarW - 3;
    uint32_t msPerProgLine = MEASURING_TIME_MS / (endProgX - currProgX);

    uint32_t currInjX = InjBarX + 3;
    uint32_t endInjX = InjBarX + InjBarW - 3;
    uint32_t msPerInjLine = INJECTATE_TIME_MS / (endInjX - currProgX);

    uint32_t prevProgTime = OS_MsTime();
    uint32_t prevInjTime = prevProgTime;
    while (1) {
        uint32_t currTime = OS_MsTime();
        int32_t diffProgTime = currTime - prevProgTime;
        int32_t diffInjTime = currTime - prevInjTime;

        while (diffProgTime >= msPerProgLine && currProgX < endProgX) {
            OS_bWait(&LCD_Mutex);
            RA8875_drawLine(currProgX, progressBarY + 2, currProgX, progressBarY + progressBarH - 3, RA8875_BLACK);
            OS_bSignal(&LCD_Mutex);
            currProgX++;
            diffProgTime -= msPerProgLine;
            prevProgTime += msPerProgLine;
        }

        while (diffInjTime >= msPerInjLine && currInjX < endInjX) {
            OS_bWait(&LCD_Mutex);
            RA8875_drawLine(currInjX, InjBarY + 2, currInjX, InjBarY + InjBarH - 3, INJECTATE_COLOR);
            OS_bSignal(&LCD_Mutex);
            currInjX++;
            diffInjTime -= msPerInjLine;
            prevInjTime += msPerInjLine;
        }
        if ((currProgX < endProgX) || (currInjX < endInjX)) {
            OS_Sleep(50);
        }
        else {
            OS_bWait(&LCD_Mutex);
            RA8875_fillScreen(BCKGRND_COLOR);
            OS_bSignal(&LCD_Mutex);
            killTransfer();
            KillCurrentReadings();
            OS_AddThread(&DisplayResults, 2);
            signalAllDataFifos();
            OS_Kill();
        }        
    }
}
