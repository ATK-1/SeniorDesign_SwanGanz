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
#include "DAS.h"

//6 adc's
// ADC0.2 = PA25 - pressure 1 
// ADC0.3 = PA24 - pressure 1 
// ADC0.5 = PB24 - thermistor 
// ADC0.7 = PA22 - thermistor 
// ADC1.3 = PA17 - pressure 2 
// ADC1.5 = PA18 - pressure 2 
// UART_Tx = PA8
// UART_Rx = PA9
void Logic_Init(void){
    IOMUX->SECCFG.PINCM[PA8INDEX]  = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA9INDEX]  = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA17INDEX] = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA18INDEX] = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA22INDEX] = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA24INDEX] = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PA25INDEX] = (uint32_t) 0x00000081;
    IOMUX->SECCFG.PINCM[PB24INDEX] = (uint32_t) 0x00000081;
    GPIOA->DOE31_0 |= (1<<25) | (1<<24) | (1<<22) | (1<<18) | (1<<17) | (1<<9) | (1<<8);
    GPIOB->DOE31_0 |= (1<<24);
}

int main(void) {
    __disable_irq();
    Clock_Init80MHz(0);
    LaunchPad_Init();
    Logic_Init();
    DAS_Init();
    __enable_irq();
    while (1) {
    }
}
