#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"
#include "OS.h"
#include "DAS.h"
#include "../inc/RTOS_UART.h"
#include "TSC2046IPWR.h"
#include "TouchControl.h"
//#include "LUT.h"


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
static uint32_t PrevPoll;
void InputPolling() {
    uint32_t newPoll = TSC2046IPWR_PollTouch();
    if ((!PrevPoll) && newPoll) {
        TSC2046Pos_t pos = TSC2046IPWR_ReadRawPosition();
        enum BUTTON button = ButtonSelect(pos);
        Fifo_Put(INPUT_FIFO, button);
    }
    PrevPoll = newPoll;

}

/*
    HeartBeat - Periodic Thread
    Toggle working LED on PA18
*/
void HeartBeat() {
    GPIOB->DOUTTGL31_0 = (1<<19); 
}


uint32_t ADC_tempLG;
uint32_t ADC_tempHG;
uint32_t temporLg;
uint32_t temporHg;
uint32_t ADC_P1LG;
uint32_t ADC_P1HG;
uint32_t ADC_P2HG;
uint32_t ADC_P2LG;

uint32_t LowGainTemp;
uint32_t HighGainTemp;

void TestDas() {
    while(1) {
    //     for (uint32_t i = 0; i < 32; i ++) {
    //         temporLg += Fifo_Get(THERM_LOW_FIFO);
    //         temporHg += Fifo_Get(THERM_HI_FIFO);
    //         ADC_P1LG   = Fifo_Get(PRESSURE_1A_FIFO);
    //         ADC_P1HG   = Fifo_Get(PRESSURE_1B_FIFO);
    //         ADC_P2LG   = Fifo_Get(PRESSURE_2A_FIFO);
    //         ADC_P2HG   = Fifo_Get(PRESSURE_2B_FIFO);
    //     }
    //     ADC_tempLG = (temporLg >> 5);
    //     ADC_tempHG = (temporHg >> 5);

    //     LowGainTemp = TempLG_LUT[ADC_tempLG];
    //     HighGainTemp = TempHG_LUT[ADC_tempHG];
    //     temporLg = 0;
    //     temporHg = 0;
    }
}