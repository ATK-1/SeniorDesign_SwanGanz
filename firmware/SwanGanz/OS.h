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
#define OS_FIFO_SIZE 64
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



/**
 * \brief Semaphore structure. Feel free to change the type of semaphore, there are lots of good solutions
 */  
struct  Sema4{
  int32_t value;   // >0 means free, otherwise means busy        
// add other components here, if necessary to implement blocking
  tcbList_t blockedList;
};
typedef struct Sema4 Sema4_t;
Sema4_t LCDAvail;

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
int OS_AddThread(void(*task)(void), 
   uint32_t stackSize, uint32_t priority);


//******** OS_AddPeriodicThread *************** 
// Add a background periodic thread
// typically this function receives the highest priority
// Inputs: task is pointer to a void/void background function
//         period in ms
//         priority 0 is the highest, 3 is the lowest
// Priorities are relative to other background periodic threads
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddPeriodicThread(void(*task)(void), 
   uint32_t period, uint32_t priority);


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
 
// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
void OS_Fifo_Init(uint32_t size);

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(uint32_t data);  

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
uint32_t OS_Fifo_Get(void);

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
int32_t OS_Fifo_Size(void);

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
// Test Main 6 - Periodic Threads of Periods : 1, 4, 5, 10
#define PSCHEDULE_SIZE 31
ptask_t PTaskSchedule[PSCHEDULE_SIZE] = {
    {NULL,  250, 0, 1},      //        0
    {NULL,  250, 0, 4},      //      250
    {NULL,  250, 0, 5},      //      500
    {NULL,  250, 0, 10},     //      750
    {NULL, 1000, 0, 1},      //     1000
    {NULL, 1000, 0, 1},      //     2000
    {NULL, 1000, 0, 1},      //     3000
    {NULL,  250, 0, 1},      //     4000
    {NULL,  750, 0, 4},      //     4250
    {NULL,  500, 0, 1},      //     5000
    {NULL,  500, 0, 5},      //     5500
    {NULL, 1000, 0, 1},      //     6000
    {NULL, 1000, 0, 1},      //     7000
    {NULL,  250, 0, 1},      //     8000
    {NULL,  750, 0, 4},      //     8250
    {NULL, 1000, 0, 1},      //     9000
    {NULL,  500, 0, 1},      //    10000
    {NULL,  250, 0, 5},      //    10500
    {NULL,  250, 0, 10},     //    10750
    {NULL, 1000, 0, 1},      //    11000
    {NULL,  250, 0, 1},      //    12000
    {NULL,  750, 0, 4},      //    12250
    {NULL, 1000, 0, 1},      //    13000
    {NULL, 1000, 0, 1},      //    14000
    {NULL,  500, 0, 1},      //    15000
    {NULL,  500, 0, 5},      //    15500
    {NULL,  250, 0, 1},      //    16000
    {NULL,  750, 0, 4},      //    16250
    {NULL, 1000, 0, 1},      //    17000
    {NULL, 1000, 0, 1},      //    18000
    {NULL, 1000, 0, 1}       //    19000
};
*/


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

/*
// Test Main 6 schedule 0
#define PSCHEDULE_SIZE 27
ptask_t PTaskSchedule[PSCHEDULE_SIZE] = {
  {NULL, 250, 0, 1},         //     0
  {NULL, 250, 0, 4},         //   250
  {NULL, 500, 0, 10},        //   500
  {NULL, 1000, 0, 1},        //  1000
  {NULL, 1000, 0, 1},        //  2000
  {NULL, 1000, 0, 1},        //  3000
  {NULL, 250, 0, 1},         //  4000
  {NULL, 750, 0, 4},         //  4250
  {NULL, 1000, 0, 1},        //  5000
  {NULL, 1000, 0, 1},        //  6000
  {NULL, 1000, 0, 1},        //  7000
  {NULL, 250, 0, 1},         //  8000
  {NULL, 750, 0, 4},         //  8250
  {NULL, 1000, 0, 1},        //  9000
  {NULL, 500, 0, 1},         // 10000 
  {NULL, 500, 0, 10},        // 10500
  {NULL, 1000, 0, 1},        // 11000
  {NULL, 250, 0, 1},         // 12000
  {NULL, 750, 0, 2},         // 12250
  {NULL, 1000, 0, 1},        // 13000
  {NULL, 1000, 0, 1},        // 14000
  {NULL, 1000, 0, 1},        // 15000
  {NULL, 250, 0, 1},         // 16000
  {NULL, 750, 0, 4},         // 16250
  {NULL, 1000, 0, 1},        // 17000
  {NULL, 1000, 0, 1},        // 18000
  {NULL, 1000, 0, 1}         // 19000
};
*/

/*
// Test Main 6 schedule 2
#define PSCHEDULE_SIZE 134
ptask_t PTaskSchedule[PSCHEDULE_SIZE] = {
  {NULL, 250, 0, 1},         //     0
  {NULL, 250, 0, 4},         //   250
  {NULL, 250, 0, 20},        //   500
  {NULL, 250, 0, 25},        //   750
  {NULL, 1000, 0, 1},         //  1000
  {NULL, 1000, 0, 1},         //  2000
  {NULL, 1000, 0, 1},         //  3000
  {NULL, 250, 0, 1},         //  4000
  {NULL, 750, 0, 4},         //  4250
  {NULL, 1000, 0, 1},         //  5000
  {NULL, 1000, 0, 1},         //  6000
  {NULL, 1000, 0, 1},         //  7000
  {NULL, 250, 0, 1},         //  8000
  {NULL, 750, 0, 4},         //  8250
  {NULL, 1000, 0, 1},         //  9000
  {NULL, 1000, 0, 1},         //  10000
  {NULL, 1000, 0, 1},         //  11000
  {NULL, 250, 0, 1},         //  12000
  {NULL, 750, 0, 4},         //  12250
  {NULL, 1000, 0, 1},         //  13000
  {NULL, 1000, 0, 1},         //  14000
  {NULL, 1000, 0, 1},         //  15000
  {NULL, 250, 0, 1},         //  16000
  {NULL, 750, 0, 4},         //  16250
  {NULL, 1000, 0, 1},         //  17000
  {NULL, 1000, 0, 1},         //  18000
  {NULL, 1000, 0, 1},         //  19000
  {NULL, 250, 0, 1},         //  20000
  {NULL, 250, 0, 4},         //  20250
  {NULL, 500, 0, 20},        //  20500
  {NULL, 1000, 0, 1},         //  21000
  {NULL, 1000, 0, 1},         //  22000
  {NULL, 1000, 0, 1},         //  23000
  {NULL, 250, 0, 1},         //  24000
  {NULL, 750, 0, 4},         //  24250
  {NULL, 750, 0, 1},         //  25000
  {NULL, 250, 0, 25},        //  25750
  {NULL, 1000, 0, 1},         //  26000
  {NULL, 1000, 0, 1},         //  27000
  {NULL, 250, 0, 1},         //  28000
  {NULL, 750, 0, 4},         //  28250
  {NULL, 1000, 0, 1},         //  29000
  {NULL, 1000, 0, 1},         //  30000
  {NULL, 1000, 0, 1},         //  31000
  {NULL, 250, 0, 1},         //  32000
  {NULL, 750, 0, 4},         //  32250
  {NULL, 1000, 0, 1},         //  33000
  {NULL, 1000, 0, 1},         //  34000
  {NULL, 1000, 0, 1},         //  35000
  {NULL, 250, 0, 1},         //  36000
  {NULL, 750, 0, 4},         //  36250
  {NULL, 1000, 0, 1},         //  37000
  {NULL, 1000, 0, 1},         //  38000
  {NULL, 1000, 0, 1},         //  39000
  {NULL, 250, 0, 1},         //  40000
  {NULL, 250, 0, 4},         //  40250
  {NULL, 500, 0, 20},        //  40500
  {NULL, 1000, 0, 1},         //  41000
  {NULL, 1000, 0, 1},         //  42000
  {NULL, 1000, 0, 1},         //  43000
  {NULL, 250, 0, 1},         //  44000
  {NULL, 750, 0, 4},         //  44250
  {NULL, 1000, 0, 1},         //  45000
  {NULL, 1000, 0, 1},         //  46000
  {NULL, 1000, 0, 1},         //  47000
  {NULL, 250, 0, 1},         //  48000
  {NULL, 750, 0, 4},         //  48250
  {NULL, 1000, 0, 1},         //  49000
  {NULL, 750, 0, 1},         //  50000
  {NULL, 250, 0, 25},        //  50750
  {NULL, 1000, 0, 1},         //  51000
  {NULL, 250, 0, 1},         //  52000
  {NULL, 750, 0, 4},         //  52250
  {NULL, 1000, 0, 1},         //  53000
  {NULL, 1000, 0, 1},         //  54000
  {NULL, 1000, 0, 1},         //  55000
  {NULL, 250, 0, 1},         //  56000
  {NULL, 750, 0, 4},         //  56250
  {NULL, 1000, 0, 1},         //  57000
  {NULL, 1000, 0, 1},         //  58000
  {NULL, 1000, 0, 1},         //  59000
  {NULL, 250, 0, 1},         //  60000
  {NULL, 250, 0, 4},         //  60250
  {NULL, 500, 0, 20},        //  60500
  {NULL, 1000, 0, 1},         //  61000
  {NULL, 1000, 0, 1},         //  62000
  {NULL, 1000, 0, 1},         //  63000
  {NULL, 250, 0, 1},         //  64000
  {NULL, 750, 0, 4},         //  64250
  {NULL, 1000, 0, 1},         //  65000
  {NULL, 1000, 0, 1},         //  66000
  {NULL, 1000, 0, 1},         //  67000
  {NULL, 250, 0, 1},         //  68000
  {NULL, 750, 0, 4},         //  68250
  {NULL, 1000, 0, 1},         //  69000
  {NULL, 1000, 0, 1},         //  70000
  {NULL, 1000, 0, 1},         //  71000
  {NULL, 250, 0, 1},         //  72000
  {NULL, 750, 0, 4},         //  72250
  {NULL, 1000, 0, 1},         //  73000
  {NULL, 1000, 0, 1},         //  74000
  {NULL, 750, 0, 1},         //  75000
  {NULL, 250, 0, 25},        //  75750
  {NULL, 250, 0, 1},         //  76000
  {NULL, 750, 0, 4},         //  76250
  {NULL, 1000, 0, 1},         //  77000
  {NULL, 1000, 0, 1},         //  78000
  {NULL, 1000, 0, 1},         //  79000
  {NULL, 250, 0, 1},         //  80000
  {NULL, 250, 0, 4},         //  80250
  {NULL, 500, 0, 20},        //  80500
  {NULL, 1000, 0, 1},         //  81000
  {NULL, 1000, 0, 1},         //  82000
  {NULL, 1000, 0, 1},         //  83000
  {NULL, 250, 0, 1},         //  84000
  {NULL, 750, 0, 4},         //  84250
  {NULL, 1000, 0, 1},         //  85000
  {NULL, 1000, 0, 1},         //  86000
  {NULL, 1000, 0, 1},         //  87000
  {NULL, 250, 0, 1},         //  88000
  {NULL, 750, 0, 4},         //  88250
  {NULL, 1000, 0, 1},         //  89000
  {NULL, 1000, 0, 1},         //  90000
  {NULL, 1000, 0, 1},         //  91000
  {NULL, 250, 0, 1},         //  92000
  {NULL, 750, 0, 4},         //  92250
  {NULL, 1000, 0, 1},         //  93000
  {NULL, 1000, 0, 1},         //  94000
  {NULL, 1000, 0, 1},         //  95000
  {NULL, 250, 0, 1},         //  96000
  {NULL, 750, 0, 4},         //  96250
  {NULL, 1000, 0, 1},         //  97000
  {NULL, 1000, 0, 1},         //  98000
  {NULL, 1000, 0, 1}          //  99000
};
*/


#endif