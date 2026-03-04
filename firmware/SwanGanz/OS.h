/**
 * @file      OS.h
 * @brief     Real Time Operating System for Labs 1, 2, 3, 4 and 5
 * @details   ECE445M
 * @version   V1.0
 * @author    Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      January 10, 2026

 ******************************************************************************/

#ifndef __OS_H
#define __OS_H  1
#include <stdint.h>
#include <stdlib.h>

/**
 * \brief Set which lab is active so I can use the same OS.c for all Labs
 */ 
#define LAB1 0
#define LAB2 0
#define LAB3 0
#define LAB4 0
#define LAB5 1
#define LAB6 0
/**
 * \brief Times assuming a 80 MHz
 */      
#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/4)  

#define STACK_SIZE 256
#define MAX_THREAD_COUNT 10
#define FIFO_CAPACITY 32
#define NUM_PRIORITIES 6
#define TCB_FIFO_SIZE 16

//#define DEBUG 1


typedef struct tcb_struct {
    void** stackPointer;
    struct tcb_struct* next;
    struct tcb_struct* prev;
    uint32_t tid;
    uint32_t isSleepy;
    uint32_t priority;
    struct Sema4* blocked;
    uint32_t stack[STACK_SIZE];
    uint32_t period;
    uint32_t targetTime;
} tcb_t;

typedef struct {
    tcb_t* tcbs[MAX_THREAD_COUNT];
    int8_t getI;
    int8_t putI;
} tcbFifo_t;

typedef struct {
    tcb_t* RoundRobinPt;
    tcb_t* head;
    tcb_t* tail;
} tcbList_t;


typedef struct {
    tcbList_t lists[NUM_PRIORITIES];
    uint8_t highestPriority;
} priorityScheduler_t;

typedef struct {
    void(*task)(void);
    uint32_t priority;
} backgroundTask_t;

typedef struct {
    void (*periodicTask)(void);
    uint32_t timeToNext;
    uint32_t isActive;
    uint32_t period;
} ptask_t;

typedef struct {
    uint32_t getI;
    uint32_t putI;
    uint32_t data[FIFO_CAPACITY];
    struct Sema4* empty;
} fifo_t;

enum FIFO {
    PRESSURE_1A_FIFO,
    PRESSURE_1B_FIFO,
    THERM_LOW_FIFO,      // Low gain
    THERM_HI_FIFO,       // High gain
    PRESSURE_2A_FIFO,     
    PRESSURE_2B_FIFO,
    INPUT_FIFO,
};

/**
 * \brief Semaphore structure. Feel free to change the type of semaphore, there are lots of good solutions
 */  
struct  Sema4{
  int32_t value;   // >0 means free, otherwise means busy        
// add other components here, if necessary to implement blocking
  tcbList_t blockedList;
};
typedef struct Sema4 Sema4_t;

/**
 * @details  Initialize operating system, disable interrupts until OS_Launch.
 * Initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers.
 * Interrupts not yet enabled.
 * @param  none
 * @return none
 * @brief  Initialize OS
 */
void OS_Init(void); 

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4_t *semaPt, int32_t value); 

// ******** OS_bWait ************
// Lab2 spinlock, set to 0
// Lab3 block if less than zero
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4_t *semaPt); 

// ******** OS_bSignal ************
// Lab2 spinlock, set to 1
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4_t *semaPt); 

//******** OS_AddThread *************** 
// add a foreground thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority, 0 is highest, 255 is the lowest
// Priorities are relative to other foreground threads
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThread(void(*task)(void), uint32_t priority);


//******** OS_AddPeriodicThread *************** 
// Add a background periodic thread
// typically this function receives the highest priority
// Inputs: task is pointer to a void/void background function
//         period in ms
//         priority 0 is the highest, 3 is the lowest
// Priorities are relative to other background periodic threads
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddPeriodicThread(void(*task)(void), uint32_t freq);


// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime); 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void); 

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void);

// temporarily prevent foreground thread switch (but allow background interrupts)
uint32_t OS_LockScheduler(void);
// resume foreground thread switching
void OS_UnLockScheduler(uint32_t previous);

// ******** OS_Time ************
// return the system time counting up
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
uint32_t OS_Time(void);

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
uint32_t OS_TimeDifference(uint32_t start, uint32_t stop);

// ******** OS_ClearMsTime ************
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
void OS_ClearMsTime(void);

// ******** OS_MsTime ************
// reads the current time in msec 
// Inputs:  none
// Outputs: time in ms units
uint32_t OS_MsTime(void);

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
void OS_Launch(uint32_t theTimeSlice);



/*
Test Main 4 - Periodic Threads : 1, 2
#define PSCHEDULE_SIZE 4
ptask_t PTaskSchedule[PSCHEDULE_SIZE] = {
    {NULL, 500, 0, 1},
    {NULL, 500, 0, 2},
    {NULL, 1000, 0, 1}
};
*/



/*
// realmain, testmainfifo, Test Main 5/7 - Periodic Threads : 1
#define PSCHEDULE_SIZE 1
ptask_t PTaskSchedule[PSCHEDULE_SIZE] = {
    {NULL, 1000, 0, 1}
};
*/

// FIFO FUNCTIONS
void Fifo_Init(enum FIFO fifoNum);
int Fifo_Put(enum FIFO fifoNum, uint32_t data);
uint32_t Fifo_Get(enum FIFO fifoNum);
#endif