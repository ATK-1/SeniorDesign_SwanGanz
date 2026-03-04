#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"

Sema4_t LCD_Mutex;
static int input;

void DisplayInit() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    OS_InitSemaphore(&LCD_Mutex, 1);
}

static void DisplayAll() {
    char* messages[] = {
        "P1A: ",
        "P1B: ",
        "T-Low: ",
        "T-Hi: ",
        "P2A: ",
        "P2B: ",
    };
    ST7735_FillScreen(ST7735_BLACK);
    while (1) {
        uint32_t data[6];
        for (int i = 0; i < 6; i++) {
            data[i] = Fifo_Get(i);
        }
        for (int i = 0; i < 6; i++) {
            OS_bWait(&LCD_Mutex);
            ST7735_SetCursor(0, i + 1);
            ST7735_OutString(messages[i]);
            ST7735_OutUDec(data[i]);
            OS_bSignal(&LCD_Mutex);
        }
    }
}

/*
    DisplayTemp - foreground thread
    Waits for sampled data in FIFO and displays in on LCD
*/
void DisplayTemp() {
    while (1) {
        uint32_t temp = Fifo_Get(THERM_LOW_FIFO);
        OS_bWait(&LCD_Mutex);
        ST7735_SetCursor(0, 1);
        ST7735_OutString("Current Temperature: ");
        ST7735_SetCursor(0, 2);
        ST7735_OutUDec(temp);
        OS_bSignal(&LCD_Mutex);
        if (input) {
            OS_AddThread(&DisplayAll, 1);
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
        ST7735_OutString("Press any button to begin calculation");
        OS_bSignal(&LCD_Mutex);
        input = Fifo_Get(INPUT_FIFO); 
        OS_Kill();
    }
}

