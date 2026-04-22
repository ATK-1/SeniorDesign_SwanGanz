
#ifndef __DISPLAY_H__
#define __DISPLAY_H__  1

void DisplayInit();
void DisplayTemp();
void DisplayStartMenu();
void DisplayMeasuring();
void DisplayCurrentReadings();
void sendNewVals(uint32_t p1, uint32_t p2, uint32_t temp);



#endif