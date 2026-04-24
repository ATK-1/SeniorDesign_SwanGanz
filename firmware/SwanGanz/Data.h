#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__  1


void TransferData();
void InitReadings();
void killTransfer();
void startTransfer(uint32_t injectTemp, uint32_t injectVol);

uint32_t getFlowRate();

uint32_t getInitialTemp();
uint32_t getAOC();
#endif