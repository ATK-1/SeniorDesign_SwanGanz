/* ADC.c
 * Jonathan Valvano
 * December 9, 2025
 * Derived from adc12_single_conversion_vref_internal_LP_MSPM0G3507_nortos_ticlang
 *              adc12_single_conversion_LP_MSPM0G3507_nortos_ticlang
 * PB24_ADC0.5 thermistor input, V_temperature
 * connect J27.1 to J27.2
 */

// ****note to students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz

#include <ti/devices/msp/msp.h>
#include <stdint.h>
#include "../inc/ADC.h"
#include "../inc/Clock.h"

// ADC1.1 = PA16 - pressure 1 LG
// ADC1.2 = PA17 - pressure 1 HG
// ADC0.0 = PA27 - thermistor LG
// ADC0.1 = PA26 - thermistor HG 
// ADC1.4 = PB17 - pressure 2 LG
// ADC1.5 = PB18 - pressure 2 HG
void ADC_Init() {
  // Reset ADC and VREF
  // RSTCLR
  // bits 31-24 unlock key 0xB1 to allow write access to this register 
  // bit 1 is Clear reset sticky bit
  // bit 0 is reset ADC
  ADC0->ULLMEM.GPRCM.RSTCTL = (0xB1 << 24) | (0x03);
  ADC1->ULLMEM.GPRCM.RSTCTL = (0xB1 << 24) | (0x03);

  // Enable power ADC and VREF
  // PWREN
  // bits 31-24 unlock key 0x26 to allow write access to this register 
  // bit 0 is Enable Power
  ADC0->ULLMEM.GPRCM.PWREN = (0x26 << 24) | (0x01);
  ADC1->ULLMEM.GPRCM.PWREN = (0x26 << 24) | (0x01);
  Clock_Delay(24); // time for ADC and VREF to power up

  // bits 31-24 unlock key 0xA9 to allow write access to this register 
  // bit 5 CCONSTOP = 0 not continuous clock in stop mode
  // bit 4 CCORUN = 0 not continuous clock in run mode
  // bit 1-0, 0 = ULPCLK, 1 = SYSOSC, 2 = HFCLK
  ADC0->ULLMEM.GPRCM.CLKCFG = (0xA9 << 24); // ULPCLK
  ADC1->ULLMEM.GPRCM.CLKCFG = (0xA9 << 24);

  ADC0->ULLMEM.CLKFREQ = 0x7; // >40 to 48 MHz
  ADC1->ULLMEM.CLKFREQ = 0x7;

  // bits 26-24 Sample clock divider: 3 = divide clock source by 8
  // bit 16 Power down: 1 = manual, 0 = power down on completion, if no pending trigger
  // bit 0 Enable conversion: 0 = disable (1 to 0 will end conversion)
  ADC0->ULLMEM.CTL0 = (3 << 24) | (1 << 16);
  ADC1->ULLMEM.CTL0 = (3 << 24) | (1 << 16);
  
  // bits 30-28 = 5  Right Shift result by 5 bits
  // bits 26-24 = 5  Hardware Average 32 Samples
  // bit 20 SAMPMODE=1 software triggers
  // bits 17-16 CONSEQ=0 ADC at start will be sampled once, 10 for repeated sampling
  // bit 8 SC=0 for stop, =1 to software start
  // bit 0 TRIGSRC=0 software trigger
  ADC0->ULLMEM.CTL1 = (5 << 28) | (5 << 24) | (1 << 16); // 32 sample HARDWARE AVERAGING
  ADC1->ULLMEM.CTL1 = (5 << 28) | (5 << 24) | (1 << 16); // 32 sample HARDWARE AVERAGING
  
  // bits 28-24 ENDADD (which  MEMCTL to end)
  // bits 20-16 STARTADD (which  MEMCTL to start)
  // bits 15-11 SAMPCNT (for DMA)
  // bit 10 FIFOEN=0 disable FIFO
  // bit 8  DMAEN=0 disable DMA
  // bits 2-1 RES=0 for 12 bit (=1 for 10bit,=2for 8-bit)
  // bit 0 DF=0 unsigned formant (1 for signed, left aligned)
  ADC0->ULLMEM.CTL2 = (1 << 24) | (0 << 16);
  ADC1->ULLMEM.CTL2 = (3 << 24) | (0 << 16);

  // bit 28 WINCOMP=0 disable window comparator
  // bit 24 TRIG trigger policy, =0 for auto next, =1 for next requires trigger
  // bit 20 BCSEN=0 disable burn out current
  // bit 16 = AVGEN =0 for no averaging
  // bit 12 = STIME=0 for SCOMP0
  // bits 9-8 VRSEL = 10 for internal VREF,(00 for VDDA)
  // bits 4-0 channel = 0 to 7 available
  ADC0->ULLMEM.MEMCTL[0] =  (1<<16) | 0;             // ADC0.0 = PA27 - thermistor LG
  ADC0->ULLMEM.MEMCTL[1] =  (1<<16) | (1<<24) | 1;   // ADC0.1 = PA26 - thermistor HG 
  ADC1->ULLMEM.MEMCTL[0] =  (1<<16) | 1;             // ADC1.1 = PA16 - pressure 1 LG
  ADC1->ULLMEM.MEMCTL[1] =  (1<<16) | 2;             // ADC1.2 = PA17 - pressure 1 HG        
  ADC1->ULLMEM.MEMCTL[2] =  (1<<16) | 4;             // ADC1.4 = PB17 - pressure 2 LG
  ADC1->ULLMEM.MEMCTL[3] =  (1<<16) | (1<<24) | 5;   // ADC1.5 = PB18 - pressure 2 HG
  

  ADC0->ULLMEM.SCOMP0 = 0; // 8 sample clocks
  ADC1->ULLMEM.SCOMP0 = 0;
}

void ADC_In(uint32_t* sampleBuffer) {
  TIMG0->CPU_INT.ICLR = 1;

  ADC0->ULLMEM.CTL0 |= 1;
  ADC0->ULLMEM.CTL1 |= (1<<8); 
  ADC1->ULLMEM.CTL0 |= 1;
  ADC1->ULLMEM.CTL1 |= (1<<8);
  
  volatile uint32_t delay = ADC0->ULLMEM.STATUS;
  while (ADC1->ULLMEM.STATUS & 0x01);

  sampleBuffer[2] = ADC0->ULLMEM.MEMRES[0];   // ADC0.0 = PA27 - thermistor LG
  sampleBuffer[3] = ADC0->ULLMEM.MEMRES[1];   // ADC0.1 = PA26 - thermistor HG 
  
  sampleBuffer[0] = ADC1->ULLMEM.MEMRES[0];   // ADC1.1 = PA16 - pressure 1 LG
  sampleBuffer[1] = ADC1->ULLMEM.MEMRES[1];   // ADC1.2 = PA17 - pressure 1 HG  
  sampleBuffer[4] = ADC1->ULLMEM.MEMRES[2];   // ADC1.4 = PB17 - pressure 2 LG
  sampleBuffer[5] = ADC1->ULLMEM.MEMRES[3];   // ADC1.5 = PB18 - pressure 2 HG
}