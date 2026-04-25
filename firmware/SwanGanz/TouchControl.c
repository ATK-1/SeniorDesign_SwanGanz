#include <stdio.h>
#include <stdlib.h>
#include <ti/devices/msp/msp.h>
#include "TSC2046IPWR.h"
#include "OS.h"
#include "DAS.h"


static uint32_t XRegion0(uint32_t y);
static uint32_t XRegion1(uint32_t y);
static uint32_t XRegion2(uint32_t y);
static uint32_t XRegion3(uint32_t y);
static uint32_t XRegion4(uint32_t y);
static uint32_t XRegion5(uint32_t y);


uint32_t DPadEnabled = 1;
// void TouchControl() {
//     while(1) { 
//         uint32_t newPoll = TSC2046IPWR_PollTouch();
//         if ((!PolledTouch) && newPoll) {
//             TSC2046Pos_t pos = TSC2046IPWR_ReadRawPosition();
//             XPos = pos.xpos;
//             YPos = pos.ypos;
//             TouchPresses++;
//             ButtonSelect(pos);
//         }
//         PolledTouch = newPoll;
//         OS_Sleep(2);
//     }
// }



// Volume Reading X
#define X_REGION_0_MIN  256
#define X_REGION_0_MAX  1360

// Stat and Temp X
#define X_REGION_1_MIN 1400
#define X_REGION_1_MAX 2600

// Left Arrow X
#define X_REGION_2_MIN 2850
#define X_REGION_2_MAX 3200

// Up and Down Arrow X
#define X_REGION_3_MIN 3220
#define X_REGION_3_MAX 3470

// Right Arrow X
#define X_REGION_4_MIN 3480
#define X_REGION_4_MAX 3800

// Reset X
#define X_REGION_5_MIN 2670
#define X_REGION_5_MAX 3860

// Start Y
#define Y_REGION_0_MIN 700
#define Y_REGION_0_MAX 1150

// Volume and Temp Y
#define Y_REGION_1_MIN 1200
#define Y_REGION_1_MAX 2400

// Up Arrow Y
#define Y_REGION_2_MIN 1120
#define Y_REGION_2_MAX 1630

// Left and Right Arrow Y
#define Y_REGION_3_MIN 1630
#define Y_REGION_3_MAX 2000

// Down Y
#define Y_REGION_4_MIN 2000
#define Y_REGION_4_MAX 2550

// Reset Y
#define Y_REGION_5_MIN 3200
#define Y_REGION_5_MAX 3700



uint32_t missClick;
uint32_t ButtonSelect(TSC2046Pos_t pos) {
    uint32_t x = pos.xpos;

    // Volume Reading X
    if ((x > X_REGION_0_MIN) && (x < X_REGION_0_MAX)) {
        return XRegion0(pos.ypos);
    }
    // Stat and Temp X
    else if ((x > X_REGION_1_MIN) && (x < X_REGION_1_MAX)) {
        return XRegion1(pos.ypos);
    }
    // Left Arrow X
    else if ((x > X_REGION_2_MIN) && (x < X_REGION_2_MAX) && DPadEnabled) {
        return XRegion2(pos.ypos);
    }
    // Up and Down Arrow X
    else if ((x > X_REGION_3_MIN) && (x < X_REGION_3_MAX) && DPadEnabled) {
        return XRegion3(pos.ypos);
    }
    // Right Arrow X
    else if ((x > X_REGION_4_MIN) && (x < X_REGION_4_MAX) && DPadEnabled) {
        return XRegion4(pos.ypos);
    }
    // Reset X
    if ((x > X_REGION_5_MIN) && (x < X_REGION_5_MAX) && DPadEnabled) {
        return XRegion5(pos.ypos);
    }
    return NULL_INPUT;


}



uint32_t VolumePressed;
uint32_t TempPressed;
uint32_t StartPressed;
uint32_t LeftPressed;
uint32_t UpPressed;
uint32_t DownPressed;
uint32_t RightPressed;

static uint32_t XRegion0(uint32_t y) {
    // Volume Button
    if ((y > Y_REGION_1_MIN) && (y < Y_REGION_1_MAX)) {
        return VOLUME_BUTTON;
    }
    return NULL_INPUT;

}
static uint32_t XRegion1(uint32_t y) {
    // Start Button
    if ((y > Y_REGION_0_MIN) && (y < Y_REGION_0_MAX)) {
        return START_BUTTON;
    }
    // Temp Button
    else if ((y > Y_REGION_1_MIN) && (y < Y_REGION_1_MAX)) {
        return TEMP_BUTTON;
    }
    return NULL_INPUT;
}
static uint32_t XRegion2(uint32_t y) {
    // Left Arrow
    if ((y > Y_REGION_3_MIN) && (y < Y_REGION_3_MAX)) {
        return LEFT_BUTTON;
    }
    return NULL_INPUT;
}
static uint32_t XRegion3(uint32_t y) {
    // Up Arrow
    if ((y > Y_REGION_2_MIN) && (y < Y_REGION_2_MAX)) {
        return UP_BUTTON;
    }
    // Down Arrow
    else if ((y > Y_REGION_4_MIN) && (y < Y_REGION_4_MAX)) {
        return DOWN_BUTTON;
    }
    return NULL_INPUT;
}
static uint32_t XRegion4(uint32_t y) {
    // Right Arrow
    if ((y > Y_REGION_3_MIN) && (y < Y_REGION_3_MAX)) {
        return RIGHT_BUTTON;
    }
    return NULL_INPUT;
}
static uint32_t XRegion5(uint32_t y) {
    // Reset Button
    if ((y > Y_REGION_5_MIN) && (y < Y_REGION_5_MAX)) {
        return RESET_BUTTON;
    }
    return NULL_INPUT;
}

void DPadControl(uint32_t DPadIsOn) {
    DPadEnabled = DPadIsOn > 0; 
}
