/*!
 * @file     TSC2056IPWR.c
 *
 * @mainpage TSC2056IPWR Resistive Touch Screen Driver
 *
 * @author   Ashton Knecht, Nathan DeLaRosa
 *
 *
 *
 * @section author Author
 *
 * Written by Ashton Knecht, Nathan DeLaRosa
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "TSC2046IPWR.h"


/*
    Pin Allocation -
    SCLK - PA11
    CS0 - PA8
    PICO - PA9
    POCI - PA10
    BSY - PB4
    PENIRQ - PB5

    SPI Channel - SPI0
    Frame Requirments - 
        DCLK - Steady State Low => SPO = 0
        Data Captured on First DCLK edge transition (Rising Edge) => SPH = 0
*/


/*
    TSC2046 Command Structure
    Bit 7   :   (S)       Start Bit - Must be high for a control byte
    Bit 6-4 :   (A2-A0)   Channel Select Bits - Chooses multiplexer inputs, touch driver switches and refenerce inputs
    Bit 3   :   (MODE)    1 = 12-bit, 0 = 8-bit
    Bit 2   :   (SER/DFR) 1 = Single Ended (Not wanted), 0 = Differential
    Bit 1-0 :   (PD1-PD0) Power Down Mode Select

*/

// TSC2046_READ_X -> 0b_1_101_1_0_00
#define TSC2046_READ_X ((uint8_t) 0b11011000)

// TSC2046_READ_Y -> -b_1_001_1_0_00
#define TSC2046_READ_Y ((uint8_t) 0b10011000)

// TSC2046_Blank -> No high start bit thus TSC knows this is not a control byte
#define TSC2046_BLANK ((uint8_t) 0b00000000)


// CS Macros
#define TSC2046_CS_LOW()     (GPIOA->DOUTCLR31_0 = (1<<8))   // PA8 low
#define TSC2046_CS_HIGH()    (GPIOA->DOUTSET31_0 = (1<<8))   // PA8 high


// Static Helper Functions
static void TSC2046_SpiInit();
static void TSC2046_CS_Init();
static void TSC2046_SPI_Reset();


void TSC2046IPWR_Init() {
    TSC2046_SpiInit();
    // PENIRQ Pin
    IOMUX->SECCFG.PINCM[PB5INDEX]  = 0x00040081;
    // BUSY Pin (Not used but good to have)
    IOMUX->SECCFG.PINCM[PB4INDEX]  = 0x00040081;
}


// Derived from Dr. Valvano's SPI.c
static void TSC2046_SpiInit() {
    uint32_t busfreq =  Clock_Freq();

    // assumes GPIOA and GPIOB are reset and powered previously
    // RSTCLR to SPI0 peripherals
    //   bits 31-24 unlock key 0xB1
    //   bit 1 is Clear reset sticky bit
    //   bit 0 is reset gpio port
    SPI0->GPRCM.RSTCTL = 0xB1000003;

    // Enable power to SPI0 peripherals
    // PWREN
    //   bits 31-24 unlock key 0x26
    //   bit 0 is Enable Power
    SPI0->GPRCM.PWREN = 0x26000001;

    Clock_Delay(24); // time for gpio to power up

    SPI0->CLKSEL = 8; // SYSCLK
    // bit 3 SYSCLK
    // bit 2 MFCLK
    // bit 1 LFCLK
    //SPI0->CLKDIV = 0; // divide by 1
  
    // bits 2-0 n (0 to 7), divide by n+1
    //Set the bit rate clock divider to generate the serial output clock
    //     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
    //     8,000,000 = (16,000,000)/((0 + 1) * 2)
    //     8,000,000 = (32,000,000)/((1 + 1) * 2)
    //     6,666,667 = (40,000,000)/((2 + 1) * 2)
    //    10,000,000 = (40,000,000)/((1 + 1) * 2)
    //     8,000,000 = (80,000,000)/((4 + 1) * 2)
    //     8,000,000 = (Clock_Freq)/((m + 1) * 2)
    //     m = (Clock_Freq/16000000) - 1
    // if(busfreq <= 16000000) {
    //   SPI0->CLKCTL = 0; // frequency= busfreq/2
    // }
    // else if(busfreq == 40000000) {
    //   SPI0->CLKCTL = 1; // frequency= 10MHz
    // // SPI0->CLKCTL = 2; // frequency= 6.66MHz
    // }
    // else{
    //   SPI0->CLKCTL = busfreq/16000000 -1; // 8 MHz
    // }

    SPI0->CLKCTL = 399; //1 MHz


    // bit 14 CSCLR=0 not cleared
    // bits 13-12 CSSEL=0 CS0
    // bit 9 SPH = 0 => Output on DCK first edge transistion (Rising Edge)
    // bit 8 SPO = 0 => DCK steady state LOW
    // bit 7 PACKEN = 0 => No packing
    // bits 6-5 FRF = 01 (4 wire)
    // bits 4-0 n=7, data size is n+1 (8bit data)
    SPI0->CTL0 = (0 << 14) | (0 << 12) | (0 << 9) | (0 << 8) | (1 << 5) | (7 << 0);

    SPI0->CTL1 = 0x0015;
    // bits 29-24 RXTIMEOUT=0
    // bits 23-16 REPEATX=0 disabled
    // bits 15-12 CDMODE=0 manual
    // bit 11 CDENABLE=0 CS3
    // bit 7-5 =0 no parity
    // bit 4=1 MSB first
    // bit 3=0 POD (not used, not peripheral)
    // bit 2=1 CP controller mode
    // bit 1=0 LBM disable loop back
    // bit 0=1 enable SPI
    TSC2046_CS_Init();
    // configure PA11 PA9 PA10 as alternate SPI0 function
    // TODO: change this back to PA11
    IOMUX->SECCFG.PINCM[PA11INDEX]  = 0x00000083;  // SPI0 SCLK 
    IOMUX->SECCFG.PINCM[PA9INDEX]  = 0x00000083;  // SPI0 PICO
    IOMUX->SECCFG.PINCM[PA10INDEX]  = 0x00040083;  // SPI0 POCI
    // IOMUX->SECCFG.PINCM[PB15INDEX] = 0x00000081;  // GPIO output, LCD !RST
    // GPIOB->DOE31_0 |= (1<<15);    // PB15 is LCD !RST
    // GPIOB->DOUTSET31_0 = (1<<15); // !RST = 1, RS=1

    TSC2046Pos_t initialPos = TSC2046IPWR_ReadRawPosition();
}


// CS0 - PA8 - Negative Logic
static void TSC2046_CS_Init() {
  IOMUX->SECCFG.PINCM[PA8INDEX]  = (uint32_t) 0x00000081;
  GPIOA->DOE31_0 |= (1 << 8);
  GPIOA->DOUTSET31_0 = (1 << 8);
}


//---------TSC2046_OutByte------------
// Output: None
// Input: 8-bit data to be transmitted to TSC2046
// Transmits a SPI byte to TSC2046 without a return
void TSC2046IPWR_OutByte(uint8_t data) {
    uint8_t response;
    // TSC2046_CS_LOW();
    while((SPI0->STAT&0x10) == 0x10){}; // spin if SPI busy
    SPI0->TXDATA = data;
    // while((SPI1->STAT&0x04) == 0x04){}; // spin SPI RxFifo empty
    while((SPI0->STAT&0x10) == 0x10) {}; // spin if SPI busy
    // TSC2046_CS_HIGH();
    response = SPI0->RXDATA; // has no meaning, flush
}


//---------TSC2046_OutByte------------
// Output: 8-bit read from TSC2046
// Input: 8-bit data to be transmitted to TSC2046
// Transmits a SPI byte to TSC2046 with a read return byte
uint8_t TSC2046IPWR_OutReadByte(uint8_t data) {
    // TSC2046_CS_LOW();
    while((SPI0->STAT&0x10) == 0x10){}; // spin if SPI busy
    SPI0->TXDATA = data;
    // while((SPI1->STAT&0x04) == 0x04){}; // spin SPI RxFifo empty
    while((SPI0->STAT&0x10) == 0x10) {}; // spin if SPI busy
    // TSC2046_CS_HIGH();
    return SPI0->RXDATA; // has no meaning, flush
}


//---------TSC2046IPWR_ReadRawPosition------------
// Output: TSC2046Pos_t - Raw 12-bit ADC values for both x and y directions
// Input: None
// Reads X and Y position returning the raw 12-bit adc value of both through the TSC2046Pos_t struct
TSC2046Pos_t TSC2046IPWR_ReadRawPosition() {
    TSC2046Pos_t result;
    result.xpos = 0;
    result.ypos = 0;
    TSC2046_CS_LOW();
    // Send Control Byte to specify x read
    TSC2046IPWR_OutByte(TSC2046_READ_X);                                        // Send X control byte
    result.xpos = ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) << 5;      // Read high 7 X bits
    result.xpos |= ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_READ_Y)) >> 3;    // Read low 5  X bits and send Y control byte
    result.ypos = ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) << 5;      // Read high 7 Y bits
    result.ypos |= ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) >> 3;     // Read low 5 Y bits

    TSC2046_CS_HIGH();
    return result;
}



// ---------- TSC2046IWR_PollTouch ----------
// Outputs: 1 => Screen Touch, 0 => No Screen Touch
// Inputs: None
// Checks for a screen touch -- assumes screen is in PD = 00 mode (Polling on PENIRQ)
uint32_t TSC2046IPWR_PollTouch() {
    // Check PENIRQ pin (PB5) -> Negative Logic
    return !(GPIOB->DIN31_0 & 0x0020);
}

// ---------- TSC2046IWR_GetX ----------
// Outputs: ADC value for screen touch in the X direction
// Inputs: None
// Transmits the read x byte and 2 null bytes to recieve x position of a touch
uint32_t TSC2046IPWR_GetX() {
    uint16_t x = 0;
    TSC2046_CS_LOW();
    // Send Control Byte to specify x read
    TSC2046IPWR_OutByte(TSC2046_READ_X);   
    x = ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) << 7;      // Read high 7 X bits
    x |= ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) >> 3;    // Read low 5  X bits 
    TSC2046_CS_HIGH();

    return x;
}


// ---------- TSC2046IWR_GetY ----------
// Outputs: ADC value for screen touch in the X direction
// Inputs: None
// Transmits the read y byte and 2 null bytes to recieve y position of a touch
uint32_t TSC2046IPWR_GetY() {
    uint32_t y = 0;
    TSC2046_CS_LOW();
    // Send Control Byte to specify y read
    TSC2046IPWR_OutByte(TSC2046_READ_Y);                             // Send X control byte
    y = ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) << 5;     // Read high 7 X bits
    y |= ((uint32_t)TSC2046IPWR_OutReadByte(TSC2046_BLANK)) >> 3;    // Read low 5  X bits 
    TSC2046_CS_HIGH();
    return y;
}