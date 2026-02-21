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

// ADC0.2 = PA25 - pressure 1 
// ADC0.3 = PA24 - pressure 1 
// ADC0.5 = PB24 - thermistor 
// ADC0.7 = PA22 - thermistor 
// ADC1.3 = PA17 - pressure 2 
// ADC1.5 = PA18 - pressure 2 
//DOESN"T WORK
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
  
  // bits 30-28 =0  no shift
  // bits 26-24 =0  no averaging
  // bit 20 SAMPMODE=1 software triggers
  // bits 17-16 CONSEQ=0 ADC at start will be sampled once, 10 for repeated sampling
  // bit 8 SC=0 for stop, =1 to software start
  // bit 0 TRIGSRC=0 software trigger
  ADC0->ULLMEM.CTL1 = (1<<16);
  ADC1->ULLMEM.CTL1 = (1<<16);
  
  // bits 28-24 ENDADD (which  MEMCTL to end)
  // bits 20-16 STARTADD (which  MEMCTL to start)
  // bits 15-11 SAMPCNT (for DMA)
  // bit 10 FIFOEN=0 disable FIFO
  // bit 8  DMAEN=0 disable DMA
  // bits 2-1 RES=0 for 12 bit (=1 for 10bit,=2for 8-bit)
  // bit 0 DF=0 unsigned formant (1 for signed, left aligned)
  ADC0->ULLMEM.CTL2 = (3 << 24) | (0 << 16);
  ADC1->ULLMEM.CTL2 = (1 << 24) | (0 << 16);

  // bit 28 WINCOMP=0 disable window comparator
  // bit 24 TRIG trigger policy, =0 for auto next, =1 for next requires trigger
  // bit 20 BCSEN=0 disable burn out current
  // bit 16 = AVGEN =0 for no averaging
  // bit 12 = STIME=0 for SCOMP0
  // bits 9-8 VRSEL = 10 for internal VREF,(00 for VDDA)
  // bits 4-0 channel = 0 to 7 available
  ADC0->ULLMEM.MEMCTL[0] = (2<<8) | 7;
  ADC0->ULLMEM.MEMCTL[1] = (2<<8) | 3;
  ADC0->ULLMEM.MEMCTL[2] = (2<<8) | 5; 
  ADC0->ULLMEM.MEMCTL[3] = (1<<24) | (2<<8) | 2;
  
  ADC1->ULLMEM.MEMCTL[0] = (2<<8) | 3; 
  ADC1->ULLMEM.MEMCTL[1] = (1<<24) | (2<<8) | 5; 

  ADC0->ULLMEM.CTL0 |= 1;
  ADC1->ULLMEM.CTL0 |= 1;

  ADC0->ULLMEM.SCOMP0 = 0; // 8 sample clocks
  ADC1->ULLMEM.SCOMP0 = 0;
}


//THE BELOW FUNCTIONS WORK

void ADC0_Init(uint32_t channel, uint32_t reference) {
    // Reset ADC and VREF
    // RSTCLR
    //   bits 31-24 unlock key 0xB1
    //   bit 1 is Clear reset sticky bit
    //   bit 0 is reset ADC
  ADC0->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  if(reference == ADCVREF_INT){
    VREF->GPRCM.RSTCTL = 0xB1000003;
  }
    // Enable power ADC and VREF
    // PWREN
    //   bits 31-24 unlock key 0x26
    //   bit 0 is Enable Power
  ADC0->ULLMEM.GPRCM.PWREN = 0x26000001;
  if(reference == ADCVREF_INT){
    VREF->GPRCM.PWREN = 0x26000001;
  }
  Clock_Delay(24); // time for ADC and VREF to power up
  ADC0->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // ULPCLK
  // bits 31-24 key=0xA9
  // bit 5 CCONSTOP= 0 not continuous clock in stop mode
  // bit 4 CCORUN= 0 not continuous clock in run mode
  // bit 1-0 0=ULPCLK,1=SYSOSC,2=HFCLK
  ADC0->ULLMEM.CLKFREQ = 7; // 40 to 48 MHz
  ADC0->ULLMEM.CTL0 = 0x03010000;
  // bits 26-24 = 011 divide by 8
  // bit 16 PWRDN=1 for manual, =0 power down on completion, if no pending trigger
  // bit 0 ENC=0 disable (1 to 0 will end conversion)
  ADC0->ULLMEM.CTL1 = 0x00000000;
  // bits 30-28 =0  no shift
  // bits 26-24 =0  no averaging
  // bit 20 SAMPMODE=1 software triggers
  // bits 17-16 CONSEQ=0 ADC at start will be sampled once, 10 for repeated sampling
  // bit 8 SC=0 for stop, =1 to software start
  // bit 0 TRIGSRC=0 software trigger
  ADC0->ULLMEM.CTL2 = 0x00000000;
  // bits 28-24 ENDADD (which  MEMCTL to end)
  // bits 20-16 STARTADD (which  MEMCTL to start)
  // bits 15-11 SAMPCNT (for DMA)
  // bit 10 FIFOEN=0 disable FIFO
  // bit 8  DMAEN=0 disable DMA
  // bits 2-1 RES=0 for 12 bit (=1 for 10bit,=2for 8-bit)
  // bit 0 DF=0 unsigned formant (1 for signed, left aligned)
  ADC0->ULLMEM.MEMCTL[0] = reference+channel;
  // bit 28 WINCOMP=0 disable window comparator
  // bit 24 TRIG trigger policy, =0 for auto next, =1 for next requires trigger
  // bit 20 BCSEN=0 disable burn out current
  // bit 16 = AVGEN =0 for no averaging
  // bit 12 = STIME=0 for SCOMP0
  // bits 9-8 VRSEL = 10 for internal VREF,(00 for VDDA)
  // bits 4-0 channel = 0 to 7 available
  ADC0->ULLMEM.SCOMP0 = 0; // 8 sample clocks
//  ADC0->ULLMEM.GEN_EVENT.ICLR |= 0x0100; // clear flag MEMCTL[0]
//  ADC0->ULLMEM.GEN_EVENT.IMASK = 0; // no interrupt
  if(reference == ADCVREF_INT){
    VREF->CLKSEL = 0x00000008; // bus clock
    VREF->CLKDIV = 0; // divide by 1
    VREF->CTL0 = 0x0001;
  // bit 8 SHMODE = off
  // bit 7 BUFCONFIG=0 for 2.4 (=1 for 1.4)
  // bit 0 is enable
    VREF->CTL2 = 0;
  // bits 31-16 HCYCLE=0
    // bits 15-0 SHCYCLE=0
    while((VREF->CTL1&0x01)==0){}; // wait for VREF to be ready
  }
}


// sample 12-bit ADC
uint32_t ADC0_In(void) {
  ADC0->ULLMEM.CTL0 |= 0x00000001; // enable conversions
  ADC0->ULLMEM.CTL1 |= 0x00000100; // start ADC
  uint32_t volatile delay=ADC0->ULLMEM.STATUS; // time to let ADC start
  while((ADC0->ULLMEM.STATUS&0x01)==0x01){} // wait for completion
  return ADC0->ULLMEM.MEMRES[0];
}