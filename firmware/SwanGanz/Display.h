#ifndef __DISPLAY_H__
#define __DISPLAY_H__  1

 #include "OS.h"

#define SCREEN_W 800
#define SCREEN_H 480

#define UT_COLOR 0xBAA0
#define INJECTATE_COLOR 0x8DDE
#define BCKGRND_COLOR 0xE7BF

#define CORNER_ROUNDNESS 5
#define SPACING 5

Sema4_t LCD_Mutex;
CatheterVals_t newVals;

void DisplayInit();
void DisplayTemp();
void DisplayStartMenu();
void DisplayMeasuring();
void DisplayCurrentReadings();
void DisplayConnected();
void sendNewVals(uint32_t p1, uint32_t p2, uint32_t temp);


#endif