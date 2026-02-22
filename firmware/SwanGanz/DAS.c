#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"

#define BUFF_SIZE 1000

uint32_t P1A[BUFF_SIZE];
uint32_t P1B[BUFF_SIZE]; 
uint32_t P2A[BUFF_SIZE];
uint32_t P2B[BUFF_SIZE];
uint32_t THERM1[BUFF_SIZE];
uint32_t THERM2[BUFF_SIZE];
uint32_t buffIdx;
uint64_t Therm1Watch;
uint64_t temp;
uint32_t watchA1;
uint32_t watchA2;
uint32_t watchA3;
uint32_t watchB0;
uint32_t watchB1;


void DAS_Init() {
    ADC_Init(); 
    //ADC0_Init(7, ADCVREF_VDDA);
    TimerG0_IntArm(40000, 5, 1);
}


void TIMG0_IRQHandler(void) {

    TIMG0->CPU_INT.ICLR = 1;
    // Therm1Watch = ADC0_In(); // does work

    // ADC0->ULLMEM.CTL0 |= 1;
    // ADC0->ULLMEM.CTL1 |= (1<<8); 
    
    // volatile uint32_t d0 = ADC0->ULLMEM.STATUS;
    // while (ADC0->ULLMEM.STATUS & 0x01);   // wait until done

    // watchA3 = ADC0->ULLMEM.MEMRES[0];
    // watchA1 = ADC0->ULLMEM.MEMRES[1];
    // watchA2 = ADC0->ULLMEM.MEMRES[2];
    // Therm1Watch = ADC0->ULLMEM.MEMRES[3]; 
    // temp = (((Therm1Watch * Therm1Watch * 0x1D) + ((61154 << 14) - (Therm1Watch * 0x50802))) >> 14);

    ADC1->ULLMEM.CTL0 |= 1;
    ADC1->ULLMEM.CTL1 |= (1<<8);
    
    volatile uint32_t d1 = ADC1->ULLMEM.STATUS;
    while (ADC1->ULLMEM.STATUS & 0x01);
    
    watchB0 = ADC1->ULLMEM.MEMRES[0];
    watchB1 = ADC1->ULLMEM.MEMRES[1];
   
   
    // THERM2[buffIdx]    = ADC0->ULLMEM.MEMRES[0];
    // P1B[buffIdx]    = ADC0->ULLMEM.MEMRES[1];
    // THERM1[buffIdx] = ADC0->ULLMEM.MEMRES[2];
    // P1A[buffIdx] = ADC0->ULLMEM.MEMRES[3]; //TESTBUFF
   
    // P2A[buffIdx]    = ADC1->ULLMEM.MEMRES[0];
    // P2B[buffIdx]    = ADC1->ULLMEM.MEMRES[1];

    // if (buffIdx < BUFF_SIZE) {
    //     THERM2[buffIdx]    = ADC0->ULLMEM.MEMRES[0];
    //     P1B[buffIdx]    = ADC0->ULLMEM.MEMRES[1];
    //     THERM1[buffIdx] = ADC0->ULLMEM.MEMRES[2];
    //     P1A[buffIdx] = ADC0->ULLMEM.MEMRES[3]; //TESTBUFF

    //     P2A[buffIdx]    = ADC1->ULLMEM.MEMRES[0];
    //     P2B[buffIdx]    = ADC1->ULLMEM.MEMRES[1];
    //     ashtonsVar = THERM2[buffIdx];
    //     buffIdx++;
    // }
    // else {
    //     ashtonsVar = ADC0->ULLMEM.MEMRES[0];
    // }
}
