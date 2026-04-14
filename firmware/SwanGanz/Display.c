#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "OS.h"
#include "Display.h"
#include "LUT.h"
#include "RA8875.h"
#include "../inc/RTOS_UART.h"

#define NUM_CHANNELS 6
Sema4_t LCD_Mutex;
static int input;


void DisplayInit() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    OS_InitSemaphore(&LCD_Mutex, 1);
}

// void DisplayInit() {
//     if (!RA8875_begin(RA8875_800x480)) {
//         while(1) {
//             // writeCommand(0x35);
//         }
//     }
//     displayOn(1);
//     GPIOX(1);
//     PWM1config(1, 0);
//     PWM1out(255);

//     OS_InitSemaphore(&LCD_Mutex, 1);
// }

int32_t transmissions;
static void DisplayAll() {
    
    ST7735_DrawString(0, 2, "calculating", ST7735_BLUE);

    while (1) {
        uint32_t data[NUM_CHANNELS];
        for (int i = 0; i < NUM_CHANNELS; i++) {
            data[i] = Fifo_Get(i);
        }
        if (transmissions < 2000) {
            UART_OutString("DATA");
            UART_OutUDec(data[0]); //p1
            UART_OutUDec(data[4]); //p2
            UART_OutUDec(data[2]); //therm
        }
    }
}


/*
    DisplayTemp - foreground thread
    Waits for sampled data in FIFO and displays in on LCD
*/
void DisplayTemp() {
    DisplayStartMenu();
    while (1) {
        if (Fifo_Get(INPUT_FIFO)) {
            Fifo_Init(PRESSURE_1A_FIFO);
            Fifo_Init(PRESSURE_1B_FIFO);
            Fifo_Init(THERM_LOW_FIFO);
            Fifo_Init(THERM_HI_FIFO);
            Fifo_Init(PRESSURE_2A_FIFO);
            Fifo_Init(PRESSURE_2B_FIFO);
            Fifo_Init(INPUT_FIFO);
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
    ST7735_DrawString(0, 0, "start", ST7735_BLUE);
}

