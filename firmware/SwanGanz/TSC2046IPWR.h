/*
TSC2046IPWR.h
Written By Ashton Knecht, Nathan DeLaRosa
Resisistve Touch Screen Interface
Written for MSPM0+
*/

typedef struct {
    uint32_t xpos;
    uint32_t ypos;
} TSC2046Pos_t;


void TSC2046IPWR_Init();


void TSC2046IPWR_OutByte(uint8_t data);


uint8_t TSC2046IPWR_OutReadByte(uint8_t data);


TSC2046Pos_t TSC2046IPWR_ReadRawPosition();

// ---------- TSC2046IWR_PollTouch ----------
// Checks for a screen touch -- assumes screen is in PD = 00 mode
// Outputs - 1 => Screen Touch, 0 => No Screen Touch
uint32_t TSC2046IPWR_PollTouch();