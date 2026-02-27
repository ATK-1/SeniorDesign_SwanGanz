#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"


uint64_t Therm1Watch;
uint64_t temp;
uint32_t watchPA25;
uint32_t watchPA24;
uint32_t LowGainTherm;
uint32_t HiGainTherm;
uint32_t watchPA17;
uint32_t watchPA18;

void DAS_Init() {
    ADC_Init(); 
    TimerG0_IntArm(40000, 5, 1); // 200 Hz
}


void TIMG0_IRQHandler(void) {
    TIMG0->CPU_INT.ICLR = 1;

    ADC0->ULLMEM.CTL0 |= 1;
    ADC0->ULLMEM.CTL1 |= (1<<8); 
    ADC1->ULLMEM.CTL0 |= 1;
    ADC1->ULLMEM.CTL1 |= (1<<8);
    
    volatile uint32_t delay = ADC0->ULLMEM.STATUS;
    while (ADC0->ULLMEM.STATUS & 0x01);

    watchPA25 = ADC0->ULLMEM.MEMRES[0];
    watchPA24 = ADC0->ULLMEM.MEMRES[1];
    LowGainTherm = ADC0->ULLMEM.MEMRES[2];
    HiGainTherm = ADC0->ULLMEM.MEMRES[3]; 
    temp = (((Therm1Watch * Therm1Watch * 0x1D) + ((61154 << 14) - (Therm1Watch * 0x50802))) >> 14); // Not correct yet

    // Test to see if we need these two lines 
    delay= ADC1->ULLMEM.STATUS;
    while (ADC1->ULLMEM.STATUS & 0x01);
    
    watchPA17 = ADC1->ULLMEM.MEMRES[0];
    watchPA18 = ADC1->ULLMEM.MEMRES[1];
}
