#include <ti/devices/msp/msp.h>
#include "../inc/ADC.h"
#include "../inc/Timer.h"

#define BUFF_SIZE 3000

static uint32_t P1A[BUFF_SIZE];
static uint32_t P1B[BUFF_SIZE]; 
static uint32_t P2A[BUFF_SIZE];
static uint32_t P2B[BUFF_SIZE];
static uint32_t THERM1[BUFF_SIZE];
static uint32_t THERM2[BUFF_SIZE];
static uint32_t buffIdx;

void DAS_Init() {
    ADC_Init();
    TimerG0_IntArm(40000, 10, 1);
}


void TIMG0_IRQHandler(void) {
    TIMG7->CPU_INT.ICLR = 1;

    ADC0->ULLMEM.CTL1 |= (1<<8); 
    ADC1->ULLMEM.CTL1 |= (1<<8);
    
    volatile uint32_t d0 = ADC0->ULLMEM.STATUS;
    volatile uint32_t d1 = ADC1->ULLMEM.STATUS;
    while (ADC0->ULLMEM.STATUS & 0x01);
    while (ADC1->ULLMEM.STATUS & 0x01);

    P1A[buffIdx]    = ADC0->ULLMEM.MEMRES[0];
    P1B[buffIdx]    = ADC0->ULLMEM.MEMRES[1];
    THERM1[buffIdx] = ADC0->ULLMEM.MEMRES[2];
    THERM2[buffIdx] = ADC0->ULLMEM.MEMRES[3];
    P2A[buffIdx]    = ADC1->ULLMEM.MEMRES[0];
    P2B[buffIdx]    = ADC1->ULLMEM.MEMRES[1];

    if (++buffIdx >= BUFF_SIZE) {
        TIMG0->COUNTERREGS.CTRCTL = 0;
    }
}
