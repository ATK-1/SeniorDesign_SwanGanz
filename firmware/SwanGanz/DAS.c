#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"
#include "OS.h"
#include "DAS.h"
#include "../inc/RTOS_UART.h"


void DASInit() {
    ADC_Init();
    // Obviously a for loop would be better but i kinda like explicity using the enum
    Fifo_Init(PRESSURE_1A_FIFO);
    Fifo_Init(PRESSURE_1B_FIFO);
    Fifo_Init(THERM_LOW_FIFO);
    Fifo_Init(THERM_HI_FIFO);
    Fifo_Init(PRESSURE_2A_FIFO);
    Fifo_Init(PRESSURE_2B_FIFO);
    Fifo_Init(INPUT_FIFO);
}

/*
    Samples ADC and puts sampled data into designated FIFO
*/
void DasStartMenu() {
    GPIOB->DOUTTGL31_0 = (1<<14);   // Toggle Debug 1
    uint32_t sampleBuffer[6];
    ADC_In(sampleBuffer);

    GPIOB->DOUTTGL31_0 = (1<<14);   // Toggle Debug 1
    Fifo_Put(THERM_LOW_FIFO, sampleBuffer[THERM_LOW]);

    GPIOB->DOUTTGL31_0 = (1<<14);   // Toggle Debug 1
}

void DasAllSamples() {
    GPIOB->DOUTTGL31_0 = (1<<15);   // Toggle Debug 2
    uint32_t sampleBuffer[6];
    ADC_In(sampleBuffer);
    
    GPIOB->DOUTTGL31_0 = (1<<15);   // Toggle Debug 2
    //maybe make function to do this all at once
    for (int i = 0; i < 6; i++) {
        Fifo_Put(i, sampleBuffer[i]);
    }
    GPIOB->DOUTTGL31_0 = (1<<15);   // Toggle Debug 2
}

// for now we're just polling for any button and will send a 1 if anything is pressed
static uint32_t PrevMode;
static uint32_t PrevEnter;
void InputPolling() {
    // uint32_t mode  = !(GPIOB->DIN31_0 & (1<<12));
    // uint32_t enter = !(GPIOB->DIN31_0 & (1<<13));

<<<<<<< Updated upstream
    // if (PrevMode && !mode) {
    //    // UART_OutString("mode\n");
    //     Fifo_Put(INPUT_FIFO, MODE);
    // }
    // if (PrevEnter && !enter) {
    //   //  UART_OutString("Enter\n");
    //     Fifo_Put(INPUT_FIFO, ENTER);
    // }
    // PrevMode = mode;
    // PrevEnter = enter;
=======
    if (PrevMode && !mode) {
       // UART_OutString("mode\n");
        Fifo_Put(INPUT_FIFO, VOLUME_BUTTON);
    }
    if (PrevEnter && !enter) {
      //  UART_OutString("Enter\n");
        Fifo_Put(INPUT_FIFO, TEMP_BUTTON);
    }
    PrevMode = mode;
    PrevEnter = enter;
>>>>>>> Stashed changes
}

/*
    HeartBeat - Periodic Thread
    Toggle working LED on PA18
*/
void HeartBeat() {
    GPIOB->DOUTTGL31_0 = (1<<19); 
}