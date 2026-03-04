#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"
#include "OS.h"
#include "DAS.h"


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
void DAS() {
    uint32_t sampleBuffer[6];
    ADC_In(sampleBuffer);
    for (int i = 0; i < 6; i++) {
        Fifo_Put(i, sampleBuffer[i]);
    }
}

// for now we're just polling for any button and will send a 1 if anything is pressed
static uint32_t PrevMode;
static uint32_t PrevEnter;
void InputPolling() {
    uint32_t mode  = !(GPIOB->DIN31_0 & (1<<12));
    uint32_t enter = !(GPIOB->DIN31_0 & (1<<13));

    if ((PrevMode && !mode) || (PrevEnter && !enter)) {
        Fifo_Put(INPUT_FIFO, 1);
    }
    PrevMode = mode;
    PrevEnter = enter;
}

/*
    HeartBeat - Periodic Thread
    Toggle working LED on PA15
*/
void HeartBeat() {
    GPIOA->DOUTTGL31_0 = (1<<15); 
}