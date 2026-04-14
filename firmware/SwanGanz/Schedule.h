#ifndef __SCHEDULE_H
#define __SCHEDULE_H  1

#include "DAS.h"
#include "Display.h"

#define PSCHEDULE_STARTMENU_SIZE 111
#define PSCHEDULE_ALLSAMPLES_SIZE 111

typedef struct {
    void (*periodicTask)(void);
    uint32_t timeToNext;
    uint32_t period;
} ptask_t;

typedef struct {
    const ptask_t* schedule;
    uint32_t scheulde_size;
} periodicScheduler_t; 

const ptask_t PSchedule_StartMenu[PSCHEDULE_STARTMENU_SIZE] = {
  {&DasStartMenu, 250, 200},          // 0
  {&InputPolling, 250, 20},           // 25
  {&HeartBeat, 500, 2},               // 50
  {&DasStartMenu, 1000, 200},         // 100
  {&DasStartMenu, 1000, 200},         // 200
  {&DasStartMenu, 1000, 200},         // 300
  {&DasStartMenu, 1000, 200},         // 400
  {&DasStartMenu, 1000, 200},         // 500
  {&DasStartMenu, 1000, 200},         // 600
  {&DasStartMenu, 1000, 200},         // 700
  {&DasStartMenu, 1000, 200},         // 800
  {&DasStartMenu, 1000, 200},         // 900
  {&DasStartMenu, 250, 200},          // 1000
  {&InputPolling, 750, 20},           // 1025
  {&DasStartMenu, 1000, 200},         // 1100
  {&DasStartMenu, 1000, 200},         // 1200
  {&DasStartMenu, 1000, 200},         // 1300
  {&DasStartMenu, 1000, 200},         // 1400
  {&DasStartMenu, 1000, 200},         // 1500
  {&DasStartMenu, 1000, 200},         // 1600
  {&DasStartMenu, 1000, 200},         // 1700
  {&DasStartMenu, 1000, 200},         // 1800
  {&DasStartMenu, 1000, 200},         // 1900
  {&DasStartMenu, 250, 200},          // 2000
  {&InputPolling, 750, 20},           // 2025
  {&DasStartMenu, 1000, 200},         // 2100
  {&DasStartMenu, 1000, 200},         // 2200
  {&DasStartMenu, 1000, 200},         // 2300
  {&DasStartMenu, 1000, 200},         // 2400
  {&DasStartMenu, 1000, 200},         // 2500
  {&DasStartMenu, 1000, 200},         // 2600
  {&DasStartMenu, 1000, 200},         // 2700
  {&DasStartMenu, 1000, 200},         // 2800
  {&DasStartMenu, 1000, 200},         // 2900
  {&DasStartMenu, 250, 200},          // 3000
  {&InputPolling, 750, 20},           // 3025
  {&DasStartMenu, 1000, 200},         // 3100
  {&DasStartMenu, 1000, 200},         // 3200
  {&DasStartMenu, 1000, 200},         // 3300
  {&DasStartMenu, 1000, 200},         // 3400
  {&DasStartMenu, 1000, 200},         // 3500
  {&DasStartMenu, 1000, 200},         // 3600
  {&DasStartMenu, 1000, 200},         // 3700
  {&DasStartMenu, 1000, 200},         // 3800
  {&DasStartMenu, 1000, 200},         // 3900
  {&InputPolling, 250, 200},          // 4000
  {&HeartBeat, 750, 20},              // 4025
  {&DasStartMenu, 1000, 200},         // 4100
  {&DasStartMenu, 1000, 200},         // 4200
  {&DasStartMenu, 1000, 200},         // 4300
  {&DasStartMenu, 1000, 200},         // 4400
  {&DasStartMenu, 1000, 200},         // 4500
  {&DasStartMenu, 1000, 200},         // 4600
  {&DasStartMenu, 1000, 200},         // 4700
  {&DasStartMenu, 1000, 200},         // 4800
  {&DasStartMenu, 1000, 200},         // 4900
  {&DasStartMenu, 250, 200},          // 5000
  {&InputPolling, 750, 20},           // 5025
  {&DasStartMenu, 1000, 200},         // 5100
  {&DasStartMenu, 1000, 200},         // 5200
  {&DasStartMenu, 1000, 200},         // 5300
  {&DasStartMenu, 1000, 200},         // 5400
  {&DasStartMenu, 1000, 200},         // 5500
  {&DasStartMenu, 1000, 200},         // 5600
  {&DasStartMenu, 1000, 200},         // 5700
  {&DasStartMenu, 1000, 200},         // 5800
  {&DasStartMenu, 1000, 200},         // 5900
  {&DasStartMenu, 250, 200},          // 6000
  {&InputPolling, 750, 20},           // 6025
  {&DasStartMenu, 1000, 200},         // 6100
  {&DasStartMenu, 1000, 200},         // 6200
  {&DasStartMenu, 1000, 200},         // 6300
  {&DasStartMenu, 1000, 200},         // 6400
  {&DasStartMenu, 1000, 200},         // 6500
  {&DasStartMenu, 1000, 200},         // 6600
  {&DasStartMenu, 1000, 200},         // 6700
  {&DasStartMenu, 1000, 200},         // 6800
  {&DasStartMenu, 1000, 200},         // 6900
  {&DasStartMenu, 250, 200},          // 7000
  {&InputPolling, 750, 20},           // 7025
  {&DasStartMenu, 1000, 200},         // 7100
  {&DasStartMenu, 1000, 200},         // 7200
  {&DasStartMenu, 1000, 200},         // 7300
  {&DasStartMenu, 1000, 200},         // 7400
  {&DasStartMenu, 1000, 200},         // 7500
  {&DasStartMenu, 1000, 200},         // 7600
  {&DasStartMenu, 1000, 200},         // 7700
  {&DasStartMenu, 1000, 200},         // 7800
  {&DasStartMenu, 1000, 200},         // 7900
  {&DasStartMenu, 250, 200},          // 8000
  {&InputPolling, 750, 20},           // 8025
  {&DasStartMenu, 1000, 200},         // 8100
  {&DasStartMenu, 1000, 200},         // 8200
  {&DasStartMenu, 1000, 200},         // 8300
  {&DasStartMenu, 1000, 200},         // 8400
  {&DasStartMenu, 1000, 200},         // 8500
  {&DasStartMenu, 1000, 200},         // 8600
  {&DasStartMenu, 1000, 200},         // 8700
  {&DasStartMenu, 1000, 200},         // 8800
  {&DasStartMenu, 1000, 200},         // 8900
  {&DasStartMenu, 250, 200},          // 9000
  {&InputPolling, 750, 20},           // 9025
  {&DasStartMenu, 1000, 200},         // 9100
  {&DasStartMenu, 1000, 200},         // 9200
  {&DasStartMenu, 1000, 200},         // 9300
  {&DasStartMenu, 1000, 200},         // 9400
  {&DasStartMenu, 1000, 200},         // 9500
  {&DasStartMenu, 1000, 200},         // 9600
  {&DasStartMenu, 1000, 200},         // 9700
  {&DasStartMenu, 1000, 200},         // 9800
  {&DasStartMenu, 1000, 200}          // 9900
};


const ptask_t PSchedule_AllSamples[PSCHEDULE_ALLSAMPLES_SIZE] = {
  {&DasAllSamples, 250, 200},         // 0
  {&InputPolling, 250, 20},           // 25
  {&HeartBeat, 500, 2},               // 50
  {&DasAllSamples, 1000, 200},        // 100
  {&DasAllSamples, 1000, 200},        // 200
  {&DasAllSamples, 1000, 200},        // 300
  {&DasAllSamples, 1000, 200},        // 400
  {&DasAllSamples, 1000, 200},        // 500
  {&DasAllSamples, 1000, 200},        // 600
  {&DasAllSamples, 1000, 200},        // 700
  {&DasAllSamples, 1000, 200},        // 800
  {&DasAllSamples, 1000, 200},        // 900
  {&DasAllSamples, 250, 200},         // 1000
  {&InputPolling, 750, 20},           // 1025
  {&DasAllSamples, 1000, 200},        // 1100
  {&DasAllSamples, 1000, 200},        // 1200
  {&DasAllSamples, 1000, 200},        // 1300
  {&DasAllSamples, 1000, 200},        // 1400
  {&DasAllSamples, 1000, 200},        // 1500
  {&DasAllSamples, 1000, 200},        // 1600
  {&DasAllSamples, 1000, 200},        // 1700
  {&DasAllSamples, 1000, 200},        // 1800
  {&DasAllSamples, 1000, 200},        // 1900
  {&DasAllSamples, 250, 200},         // 2000
  {&InputPolling, 750, 20},           // 2025
  {&DasAllSamples, 1000, 200},        // 2100
  {&DasAllSamples, 1000, 200},        // 2200
  {&DasAllSamples, 1000, 200},        // 2300
  {&DasAllSamples, 1000, 200},        // 2400
  {&DasAllSamples, 1000, 200},        // 2500
  {&DasAllSamples, 1000, 200},        // 2600
  {&DasAllSamples, 1000, 200},        // 2700
  {&DasAllSamples, 1000, 200},        // 2800
  {&DasAllSamples, 1000, 200},        // 2900
  {&DasAllSamples, 250, 200},         // 3000
  {&InputPolling, 750, 20},           // 3025
  {&DasAllSamples, 1000, 200},        // 3100
  {&DasAllSamples, 1000, 200},        // 3200
  {&DasAllSamples, 1000, 200},        // 3300
  {&DasAllSamples, 1000, 200},        // 3400
  {&DasAllSamples, 1000, 200},        // 3500
  {&DasAllSamples, 1000, 200},        // 3600
  {&DasAllSamples, 1000, 200},        // 3700
  {&DasAllSamples, 1000, 200},        // 3800
  {&DasAllSamples, 1000, 200},        // 3900
  {&InputPolling, 250, 200},          // 4000
  {&HeartBeat, 750, 20},              // 4025
  {&DasAllSamples, 1000, 200},        // 4100
  {&DasAllSamples, 1000, 200},        // 4200
  {&DasAllSamples, 1000, 200},        // 4300
  {&DasAllSamples, 1000, 200},        // 4400
  {&DasAllSamples, 1000, 200},        // 4500
  {&DasAllSamples, 1000, 200},        // 4600
  {&DasAllSamples, 1000, 200},        // 4700
  {&DasAllSamples, 1000, 200},        // 4800
  {&DasAllSamples, 1000, 200},        // 4900
  {&DasAllSamples, 250, 200},         // 5000
  {&InputPolling, 750, 20},           // 5025
  {&DasAllSamples, 1000, 200},        // 5100
  {&DasAllSamples, 1000, 200},        // 5200
  {&DasAllSamples, 1000, 200},        // 5300
  {&DasAllSamples, 1000, 200},        // 5400
  {&DasAllSamples, 1000, 200},        // 5500
  {&DasAllSamples, 1000, 200},        // 5600
  {&DasAllSamples, 1000, 200},        // 5700
  {&DasAllSamples, 1000, 200},        // 5800
  {&DasAllSamples, 1000, 200},        // 5900
  {&DasAllSamples, 250, 200},         // 6000
  {&InputPolling, 750, 20},           // 6025
  {&DasAllSamples, 1000, 200},        // 6100
  {&DasAllSamples, 1000, 200},        // 6200
  {&DasAllSamples, 1000, 200},        // 6300
  {&DasAllSamples, 1000, 200},        // 6400
  {&DasAllSamples, 1000, 200},        // 6500
  {&DasAllSamples, 1000, 200},        // 6600
  {&DasAllSamples, 1000, 200},        // 6700
  {&DasAllSamples, 1000, 200},        // 6800
  {&DasAllSamples, 1000, 200},        // 6900
  {&DasAllSamples, 250, 200},         // 7000
  {&InputPolling, 750, 20},           // 7025
  {&DasAllSamples, 1000, 200},        // 7100
  {&DasAllSamples, 1000, 200},        // 7200
  {&DasAllSamples, 1000, 200},        // 7300
  {&DasAllSamples, 1000, 200},        // 7400
  {&DasAllSamples, 1000, 200},        // 7500
  {&DasAllSamples, 1000, 200},        // 7600
  {&DasAllSamples, 1000, 200},        // 7700
  {&DasAllSamples, 1000, 200},        // 7800
  {&DasAllSamples, 1000, 200},        // 7900
  {&DasAllSamples, 250, 200},         // 8000
  {&InputPolling, 750, 20},           // 8025
  {&DasAllSamples, 1000, 200},        // 8100
  {&DasAllSamples, 1000, 200},        // 8200
  {&DasAllSamples, 1000, 200},        // 8300
  {&DasAllSamples, 1000, 200},        // 8400
  {&DasAllSamples, 1000, 200},        // 8500
  {&DasAllSamples, 1000, 200},        // 8600
  {&DasAllSamples, 1000, 200},        // 8700
  {&DasAllSamples, 1000, 200},        // 8800
  {&DasAllSamples, 1000, 200},        // 8900
  {&DasAllSamples, 250, 200},         // 9000
  {&InputPolling, 750, 20},           // 9025
  {&DasAllSamples, 1000, 200},        // 9100
  {&DasAllSamples, 1000, 200},        // 9200
  {&DasAllSamples, 1000, 200},        // 9300
  {&DasAllSamples, 1000, 200},        // 9400
  {&DasAllSamples, 1000, 200},        // 9500
  {&DasAllSamples, 1000, 200},        // 9600
  {&DasAllSamples, 1000, 200},        // 9700
  {&DasAllSamples, 1000, 200},        // 9800
  {&DasAllSamples, 1000, 200}         // 9900
};

const periodicScheduler_t FixedSchedule[2] = {
    {PSchedule_StartMenu, PSCHEDULE_STARTMENU_SIZE},
    {PSchedule_AllSamples, PSCHEDULE_ALLSAMPLES_SIZE}
};

#endif