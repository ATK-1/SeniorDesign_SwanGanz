/*
TSC2046IPWR.h
Written By Ashton Knecht, Nathan DeLaRosa
Resisistve Touch Screen Interface
Written for MSPM0+
*/


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


typedef struct {
    uint32_t xpos;
    uint32_t ypos;
} TSC2046Pos_t;


void TSC2046IPWR_Init();


//---------TSC2046_OutByte------------
// Output: 8-bit read from TSC2046
// Input: 8-bit data to be transmitted to TSC2046
// Transmits a SPI byte to TSC2046 with a read return byte
void TSC2046IPWR_OutByte(uint8_t data);


//---------TSC2046_OutByte------------
// Output: None
// Input: 8-bit data to be transmitted to TSC2046
// Transmits a SPI byte to TSC2046 without a return
uint8_t TSC2046IPWR_OutReadByte(uint8_t data);

//---------TSC2046IPWR_ReadRawPosition------------
// Output: TSC2046Pos_t - Raw 12-bit ADC values for both x and y directions
// Input: None
// Reads X and Y position returning the raw 12-bit adc value of both through the TSC2046Pos_t struct
TSC2046Pos_t TSC2046IPWR_ReadRawPosition();

// ---------- TSC2046IWR_PollTouch ----------
// Checks for a screen touch -- assumes screen is in PD = 00 mode
// Outputs - 1 => Screen Touch, 0 => No Screen Touch
uint32_t TSC2046IPWR_PollTouch();



// ---------- TSC2046IWR_GetX ----------
// Outputs: ADC value for screen touch in the X direction
// Inputs: None
// Transmits the read x byte and 2 null bytes to recieve x position of a touch
uint32_t TSC2046IPWR_GetX();


// ---------- TSC2046IWR_GetY ----------
// Outputs: ADC value for screen touch in the X direction
// Inputs: None
// Transmits the read y byte and 2 null bytes to recieve y position of a touch
uint32_t TSC2046IPWR_GetY();