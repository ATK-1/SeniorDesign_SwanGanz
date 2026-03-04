#include <stdint.h>
#include <stdio.h>
#include "OS.h"

long StartCritical(void);
void EndCritical(long);

static fifo_t Fifos[6];

// ******** Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
void Fifo_Init(enum FIFO fifoNum) {
    fifo_t* fifo = &Fifos[fifoNum];
    fifo->getI = 0;
    fifo->putI = 0;
    OS_InitSemaphore(fifo->empty, 0);
}

// ******** Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int Fifo_Put(enum FIFO fifoNum, uint32_t data) {
    fifo_t* fifo = &Fifos[fifoNum];
    uint32_t newPutI = (fifo->putI + 1) & (FIFO_CAPACITY - 1);
    if (newPutI == fifo->putI) {
        return 0;
    }
    fifo->data[fifo->putI] = data;          // save in Fifo
    fifo->putI = newPutI;               // next place to put
    OS_bSignal(fifo->empty);
    return 1;
}

// ******** Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
uint32_t Fifo_Get(enum FIFO fifoNum) {
    fifo_t* fifo = &Fifos[fifoNum];
    uint32_t data;
    OS_bWait(fifo->empty);
    long status = StartCritical();
    data = fifo->data[fifo->getI];                    
    fifo->getI = (fifo->getI + 1) & (FIFO_CAPACITY - 1); 
    EndCritical(status);
    return data;
}