/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/ADC.h"
#include "../inc/SPI.h"
#include "Display.h"
#include "DAS.h"
#include "OS.h"

#define TIME_1MS    80000      

// MODE button PB12
// ENTER button PB13

// ADC0.2 = PA25 - pressure 1 low gain 
// ADC0.3 = PA24 - pressure 1 high gain 
// ADC0.5 = PB24 - thermistor 
// ADC0.7 = PA22 - thermistor TEST1
// ADC1.3 = PA17 - pressure 2 low gain  
// ADC1.5 = PA18 - pressure 2 high gain
// UART_Tx = PA8
// UART_Rx = PA9
void Logic_Init() {
    // Inputs - debug pins and LED
    IOMUX->SECCFG.PINCM[PA10INDEX] = (uint32_t) 0x00000081; // Debug Pin 1
    IOMUX->SECCFG.PINCM[PA11INDEX] = (uint32_t) 0x00000081; // Debug Pin 2
    IOMUX->SECCFG.PINCM[PA12INDEX] = (uint32_t) 0x00000081; // Debug Pin 3
    IOMUX->SECCFG.PINCM[PA13INDEX] = (uint32_t) 0x00000081; // Debug Pin 4
    IOMUX->SECCFG.PINCM[PA15INDEX] = (uint32_t) 0x00000081; // Working LED
    IOMUX->SECCFG.PINCM[PA16INDEX] = (uint32_t) 0x00000081; // Error LED
    IOMUX->SECCFG.PINCM[PB17INDEX] = (uint32_t) 0x00000081; // Debug Pin 5
    IOMUX->SECCFG.PINCM[PB18INDEX] = (uint32_t) 0x00000081; 
    IOMUX->SECCFG.PINCM[PB19INDEX] = (uint32_t) 0x00000081; 
    IOMUX->SECCFG.PINCM[PB20INDEX] = (uint32_t) 0x00000081; // Debug Pin 8
    GPIOA->DOE31_0 |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 15) | (1 << 16);
    GPIOB->DOE31_0 |= (0x0F << 17);

    // Outputs- buttons
    IOMUX->SECCFG.PINCM[PB12INDEX] = (uint32_t) 0x00040081;
    IOMUX->SECCFG.PINCM[PB13INDEX] = (uint32_t) 0x00040081;
}

static void IdleThread() {
    while (1) {
        //__asm("WFI");
    }
}

int main(void) {
    __disable_irq();
    Clock_Init80MHz(1);
    LaunchPad_Init();
    Logic_Init();
    OS_Init();
    DASInit();

    OS_SetPerioidcSchedule(0);
    OS_AddThread(&DisplayTemp, 1);
    OS_AddThread(&IdleThread, 2);
    
    
    OS_Launch(TIME_1MS);
}

