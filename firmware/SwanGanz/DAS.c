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
    for (int i = 0; i < 7; i++) {
        Fifo_Init(i);
    }
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
        Fifo_Put(INPUT, button);
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