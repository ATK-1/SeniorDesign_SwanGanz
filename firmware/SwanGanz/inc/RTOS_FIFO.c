// RTOS_FIFO.c
// Runs on any Microcontroller
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The FIFO
// uses index implementation.
// Daniel Valvano
// June 20, 2025

/* 
 Copyright 2025 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <ti/devices/msp/msp.h>
#include "../inc/RTOS_UART.h"
#include "../inc/RTOS_FIFO.h"
#include "OS.h"
// Two-index implementation of the transmit FIFO
// can hold 0 to TXFIFOSIZE-1 elements
uint32_t volatile TxPutI; // where to put next
uint32_t volatile TxGetI; // where to get next
//Sema4_t TxFifoEmpty;
Sema4_t TxFifoFull;

long StartCritical(void);
void EndCritical(long);
long sr;

Sema4_t RxFifoEmpty;
//Sema4_t RxFifoFull;
char static TxFifo[TXFIFOSIZE];

void TxFifo_Init(void){
  TxPutI = TxGetI = 0; // empty
  //OS_InitSemaphore(&TxFifoEmpty, 0);
  OS_InitSemaphore(&TxFifoFull, TXFIFOSIZE - 1);

  OS_InitSemaphore(&RxFifoEmpty, 0);
  //OS_InitSemaphore(&RxFifoFull, RXFIFOSIZE - 1);
}
int TxFifo_Put(char data){
  //OS_bWait(&TxFifoFull);
  StartCritical();
  uint32_t newPutI = (TxPutI+1)&(TXFIFOSIZE-1);
  TxFifo[TxPutI] = data;          // save in Fifo
  TxPutI = newPutI;               // next place to put
  //OS_Signal(&TxFifoEmpty);
  EndCritical(sr);
  return 1;
}
char TxFifo_Get(void){char data;
  if (TxGetI == TxPutI) {
    return 0;
  }
  //TxFifoEmpty.value -= 1;
  data = TxFifo[TxGetI];              // retrieve data
  TxGetI = (TxGetI+1)&(TXFIFOSIZE-1); // next place to get
  //OS_bSignal(&TxFifoFull);
  return data;
}

uint32_t TxFifo_Size(void){
  return (TxPutI-TxGetI)&(TXFIFOSIZE-1);
}

// Two-index implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements
uint32_t volatile RxPutI; // where to put next
uint32_t volatile RxGetI; // where to get next
char static RxFifo[RXFIFOSIZE];

void RxFifo_Init(void){
  RxPutI = RxGetI = 0;  // empty
}
int RxFifo_Put(char data){
  uint32_t newPutI = (RxPutI+1)&(RXFIFOSIZE-1);
  if (newPutI == RxGetI) {
    return 0;
  }
  //RxFifoFull.value -= 1;
  RxFifo[RxPutI] = data;          // save in Fifo
  RxPutI = newPutI;               // next place to put
  //OS_bSignal(&RxFifoEmpty);
  return 1;
}
char RxFifo_Get(void){
  char data;
  //OS_bWait(&RxFifoEmpty);
  StartCritical();
  data = RxFifo[RxGetI];              // retrieve data
  RxGetI = (RxGetI+1)&(RXFIFOSIZE-1); // next place to get
  //OS_Signal(&RxFifoFull);
  EndCritical(sr);
  return data;
}

uint32_t RxFifo_Size(void){
  return (RxPutI-RxGetI)&(RXFIFOSIZE-1);
}