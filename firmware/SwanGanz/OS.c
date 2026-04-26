// *************os.c**************
// ECE445M Labs 1, 2, 3, 4, 5 and 6
// Starter to labs 1,2,3,4,5,6
// high level OS functions
// Students will implement these functions as part of Lab
// Runs on MSPM0
// Jonathan W. Valvano 
// January 10, 2026, valvano@mail.utexas.edu


#include <stdint.h>
#include <stdio.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/Timer.h"
#include "../inc/ST7735.h"
#include "../inc/RTOS_UART.h"
#include "OS.h"
#include "Display.h"
#include "Schedule.h"
#include "../inc/RTOS_UART.h"

// Hardware interrupt priorities
//   Priority 0: Periodic threads 
//   Priority 1: Input/output interrupts like UART and edge triggered 
//   Priority 2: 1000 Hz periodic event to implement OS_MsTime and sleep using TimerG8
//   Priority 2: SysTick for running the scheduler
//   Priority 3: PendSV for context switch 

// *****************Timers******************
// SysTick for running the scheduler
// TimerG7 is interrupts at 1000Hz to implement OS_MsTime, and sleeping
// TimerG8 for background periodic threads
// TimerG12 for 32-bit OS_Time, free running (no interrupts)

void OSDisableInterrupts(void);
void OSEnableInterrupts(void);
long StartCritical(void);
void EndCritical(long);
long sr;
#define  OSCRITICAL_ENTER() { sr=StartCritical(); }
#define  OSCRITICAL_EXIT()  { EndCritical(sr); }


// Tcb functions and variables
static void TcbFifoInit();
static void TcbFifoInit();
static tcb_t* TcbFifoGet();
static uint32_t TcbFifoPut(tcb_t* tcb);
tcb_t* RunPt;
tcb_t* NextThreadPt;
tcb_t PookieTCB;

Sema4_t TCBQueueSemaphore;
static int ThreadIdCounter = 0;
tcbFifo_t TcbFifo = {
  .tcbs = {0},
  .getI = 0,
  .putI = 0,
};


// TCB Linked List function
static void InsertSleepyThread(tcb_t* tcb, uint32_t timeOffset);
static void InsertRunThreadList(tcb_t* tcb, tcbList_t* list);
static void InsertBlockedList(tcb_t* tcb, tcbList_t* list);
static void RemoveRunThreadList(tcb_t* tcb, tcbList_t* list);
static void GetHighestPriority();

void EdgeTriggered_Init(void);

// Creating and running tasks
static void BackgroundThreadRun(void(*task)(void));
static void SpawnAperiodicTask(backgroundTask_t task);

// TCB LISTS
tcb_t THREAD_CONTROL_BLOCKS[8];
priorityScheduler_t Scheduler = {.highestPriority = 6};
tcb_t* SleepyThreadsHead;

uint32_t TimeMs;
uint32_t secretTimeMs;
uint32_t isLaunched = 0;


// Mail Box
Sema4_t MailBoxEmpty;
Sema4_t MailBoxFull;
uint32_t MailBox;

// Background Threads
backgroundTask_t S2Task;
backgroundTask_t PA28Task;

// Periodic Threads
periodicScheduler_t CurrentSchedule;
uint32_t PTaskIdx;


// ******** OS_ClearMsTime ************
// sets the system time to zero and start a periodic interrupt
// Inputs:  none
// Outputs: none
void OS_ClearMsTime(void) {
    TimeMs = 0;
    TimerG7_IntArm(10000, 8, 0);
    
};


// ******** OS_MsTime ************
// reads the current time in msec
// Inputs:  none
// Outputs: time in ms units
uint32_t OS_MsTime(void){
    return TimeMs;
};

void StartOS(void); // implemented in osasm.s


/*
  always schedule highest priority next guy
  if priority n is null, check list n+1
  check if sleeping
*/
static void OS_Scheduler() {
    uint32_t crit;
    GPIOB->DOUTTGL31_0 = (1<<1);
    GPIOB->DOUTTGL31_0 = (1<<1);
    SysTick->VAL = 0;                //Reset systick


    tcb_t** roundRobinPt = &Scheduler.lists[Scheduler.highestPriority].RoundRobinPt;

    if (!roundRobinPt) {
        GetHighestPriority();
        roundRobinPt = &Scheduler.lists[Scheduler.highestPriority].
        RoundRobinPt;
    }

    tcb_t* candidate = (*roundRobinPt)->next;
    crit = StartCritical();
    *roundRobinPt = candidate;
    NextThreadPt = candidate;
    EndCritical(crit);
    GPIOB->DOUTTGL31_0 = (1<<1);
}
//------------------------------------------------------------------------------
//  Systick Interrupt Handler
//  SysTick interrupt happens every 2 ms
// used for preemptive foreground thread switch
// ------------------------------------------------------------------------------
void SysTick_Handler(void) {   
    OS_Scheduler();
    SCB->ICSR = 0x10000000;
}

uint32_t OS_LockScheduler(void){
    uint32_t old = SysTick->CTRL;
    SysTick->CTRL= 5;
    return old;
}
void OS_UnLockScheduler(uint32_t previous){
    SysTick->CTRL = previous;
}


//
//@details  Initialize operating system, disable interrupts until OS_Launch.
//Initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers.
// Interrupts not yet enabled.
 // @param  none
 // @return none
 //@brief  Initialize OS
//
void OS_Init(void){
    // Arm G7 and G12 used for TimeMs and OS_Time respectively 
    OS_ClearMsTime();

    // TCB
    RunPt = &PookieTCB;
    TcbFifoInit();
    OS_InitSemaphore(&TCBQueueSemaphore, 0);

    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        THREAD_CONTROL_BLOCKS[i].stackPointer = (void*)(&(THREAD_CONTROL_BLOCKS[i].stack[STACK_SIZE-1]));   // Initialize all tcb's stacks 
        THREAD_CONTROL_BLOCKS[i].blocked = false;
        THREAD_CONTROL_BLOCKS[i].next = NULL;
        THREAD_CONTROL_BLOCKS[i].prev = NULL;
        THREAD_CONTROL_BLOCKS[i].isSleepy = false;
        THREAD_CONTROL_BLOCKS[i].tid = 0xFFFFFFFF;
        THREAD_CONTROL_BLOCKS[i].stack[0] = 0xBADEEDAD;  // Asthon's genius idea
    }

    // Clear and Set Priority of PendSV and Systick
    SCB->SHP[1] &= 0x3FFFFFFF;
    SCB->SHP[1] |= 0xBFFFFFFF;
}

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4_t* semaPt, int32_t value){
    semaPt->value = value;
}; 


// ******** OS_bWait ************
// Lab3 block if less than zero
// input:  pointer to a blocking semaphore
// output: none
void OS_bWait(Sema4_t *semaPt){
    OSDisableInterrupts(); 
    #ifdef DEBUG
        startMaxJitterTime();
    #endif 
    // Check if available
    if ((semaPt->value) > 0) {
    semaPt->value = semaPt->value - 1;
    OSEnableInterrupts();
    return;
    }
    semaPt->value = semaPt->value - 1;
    // Add to block list
    RunPt->blocked = semaPt;
    tcb_t* running = RunPt;
    OSEnableInterrupts();
    RemoveRunThreadList(running, &Scheduler.lists[running->priority]);
    InsertBlockedList(running, &semaPt->blockedList);
    OS_Suspend();
}; 

// ******** OS_bSignal ************
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a blocking semaphore
// output: none
void OS_bSignal(Sema4_t *semaPt){
    tcbList_t* blockedList = &semaPt->blockedList;

    long sr = StartCritical();
    semaPt->value = semaPt->value + 1;
    EndCritical(sr);


    if (blockedList->head) {
        sr = StartCritical();
        tcb_t* unblockedTcb = blockedList->head;
        if (blockedList->head == blockedList->tail) {
            blockedList->tail = NULL;
            blockedList->head = NULL;
        }
        else {
            blockedList->head =  blockedList->head->next;
        }
        EndCritical(sr);

        unblockedTcb->blocked = NULL;

        // Context Switch if we unblock a higher priority thread
        uint32_t prevHighPriority = Scheduler.highestPriority;
        InsertRunThreadList(unblockedTcb, &Scheduler.lists[unblockedTcb->priority]);
        if (prevHighPriority > Scheduler.highestPriority) {
            OS_Suspend();
            return;
        }
    }
}; 


int OS_AddThread(void(*task)(void), uint32_t priority) { 
    // Get a free TCB
    tcb_t* newTcb = TcbFifoGet();
    if (!newTcb) {
        return 0;
    }

    // Place the TCB into the running threads linked list
    newTcb->priority = priority;
    InsertRunThreadList(newTcb, &Scheduler.lists[priority]);

    // Initialize the thread's stack for when we context switch in for the first time
    *(--newTcb->stackPointer) = (void*)0x01000000; // Push a starter PSR
    *(--newTcb->stackPointer) = task;              // Push first address of the task to be the first PC
    newTcb->stackPointer -= 10;                    // Creating space on the stack for LR, R12, R0-R3, R4-R7

    newTcb->tid = ++ThreadIdCounter;
    return 1;
}


static void InsertSleepyThread(tcb_t* tcb, uint32_t timeOffset) {
  uint32_t targetTime = OS_MsTime() + timeOffset;
  tcb->targetTime = targetTime;
  tcb->next = NULL;
  tcb->prev = NULL;

  OSDisableInterrupts();
    
  if (!SleepyThreadsHead) {
    SleepyThreadsHead = tcb;
    OSEnableInterrupts();
    return;
  }
  if (targetTime < SleepyThreadsHead->targetTime) {
    tcb->next = SleepyThreadsHead;
    SleepyThreadsHead = tcb;
    OSEnableInterrupts();
    return;
  }
  tcb_t* curr = SleepyThreadsHead;
  while (curr->next && (curr->next->targetTime <= targetTime)) {
      curr = curr->next;
  }
  tcb->next = curr->next;  
  curr->next = tcb;
  OSEnableInterrupts();  
}

//******** OS_SetPerioidcSchedule *************** 
// Sets the current fixed scheudle of periodic threads to run
void OS_SetPerioidcSchedule(uint32_t scheudleNumber) {
    CurrentSchedule = FixedSchedule[scheudleNumber];

}

/*
  TimerG7 Handler
  Increments TimeMs, and checks if any timed thread needs to wake up
*/
void TIMG7_IRQHandler(void){
    if ((TIMG7->CPU_INT.IIDX) == 1) { // this will acknowledge
        TimeMs++;
        secretTimeMs++;
        while (SleepyThreadsHead && (SleepyThreadsHead->targetTime <= TimeMs)) {
            tcb_t* curr = SleepyThreadsHead;
            curr->isSleepy = 0;

            SleepyThreadsHead = SleepyThreadsHead->next;
            InsertRunThreadList(curr, &Scheduler.lists[curr->priority]);
        }
    }
}



// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime){
    if (sleepTime) {
        RunPt->isSleepy = 1;
        RemoveRunThreadList(RunPt, &Scheduler.lists[RunPt->priority]);
        InsertSleepyThread(RunPt, sleepTime);
    }
    OS_Suspend();
} 



// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void){
    OSDisableInterrupts();
    tcb_t* curr = RunPt;
    PookieTCB.stackPointer = (void**)&PookieTCB.stack[STACK_SIZE - 1];
    RunPt = &PookieTCB;

    RemoveRunThreadList(curr, &Scheduler.lists[curr->priority]);
    curr->stackPointer = (void**)&curr->stack[STACK_SIZE - 1];
    curr->blocked = NULL;
    curr->period = 0;
    curr->isSleepy = 0;

    // Need this to store the stack pointer somewhere 
    TcbFifoPut(curr);
    OSEnableInterrupts();
    OS_Suspend();


    while(1){}
}; 


// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void) {
    OS_Scheduler();
    SCB->ICSR = 0x10000000;
};
  
 

// ******** OS_Time ************
// return the system time, counting up 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
uint32_t OS_Time(void){
    uint32_t time = TIMG12->COUNTERREGS.CTR;
    return time;
};

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
uint32_t OS_TimeDifference(uint32_t start, uint32_t stop){
    uint32_t diff = stop - start;
    return stop - start;
};



// ******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
void OS_Launch(uint32_t theTimeSlice) {
    // Initailize SysTick
    SysTick->CTRL = 0;            // 1) disable SysTick during setup
    SysTick->LOAD = theTimeSlice; // 2) stops
    SysTick->VAL = 0;             // 3) any write to current clears it
    SysTick->CTRL = 0x00000007;   // 4) enable SysTick with core clock

    isLaunched = 1;

    TimerG12_IntArm(0xFFFFFFFF, 0);
    TimerG8_IntArm(1000, 100, 0);
    OSEnableInterrupts(); 
    OS_Suspend();
}

static void TcbFifoInit() {
    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        TcbFifo.tcbs[i] = THREAD_CONTROL_BLOCKS + i;
    }
}


static tcb_t* TcbFifoGet() {
    tcb_t* data;
    StartCritical();
    // Let code hardfault if empty
    // if (FreeTcbFifo.getI == FreeTcbFifo.putI) {
    //   return NULL;
    // }
    data = TcbFifo.tcbs[TcbFifo.getI];
    TcbFifo.getI = (TcbFifo.getI + 1) & (TCB_FIFO_SIZE - 1);
    EndCritical(sr);
    return data;
}

static uint32_t TcbFifoPut(tcb_t* tcb) {
    StartCritical();
    uint32_t newPutI = (TcbFifo.putI + 1) & (TCB_FIFO_SIZE - 1);
    // Let code hardfault, this shouldn't happen so we don't want to waste cycles
    // if (newPutI == FreeTcbFifo.getI) { 
    //   return 0;
    // }
    TcbFifo.tcbs[TcbFifo.putI] = tcb;
    TcbFifo.putI = newPutI;
    EndCritical(sr);
    return 1;
}


static void InsertRunThreadList(tcb_t* tcb, tcbList_t* list) {
    StartCritical();
    if (!list->head) {
        tcb->next = tcb;
        tcb->prev = tcb;
        list->head = tcb;
        list->tail = tcb;
        list->RoundRobinPt = tcb;
    }
    else {
        list->head->prev = tcb;
        list->tail->next = tcb;
        tcb->next = list->head;
        tcb->prev = list->tail;
        list->tail = tcb;
    }
    if (tcb->priority < Scheduler.highestPriority) {
        Scheduler.highestPriority = tcb->priority;
    }
    EndCritical(sr);
}

// sort by priority
static void InsertBlockedList(tcb_t* tcb, tcbList_t* blockedList) {
    // Interrupts already disabled
    StartCritical();
    if (!blockedList->head) {
        blockedList->head = tcb;
    }
    else {
        blockedList->tail->next = tcb;
    }
    blockedList->tail = tcb;
    tcb->next = NULL;
    tcb->prev = NULL;
    EndCritical(sr);
}

/*
    Remove thread from RunThread list 
    Assume interrupts have already been disabled before calling this function
    That way if any additional task-specific work needs to be done, it can be
    done after the function all without having to disable and enable interrupts again
*/
static void RemoveRunThreadList(tcb_t* tcb, tcbList_t* list) {
    StartCritical();
    tcb_t* prev = tcb->prev;
    tcb_t* next = tcb->next;

    if (tcb->next == tcb) {
        list->head = NULL;
        list->tail = NULL;
        list->RoundRobinPt = NULL;
    }
    else {
        if (tcb == list->head) {
            list->head = next;
        }
        else if (tcb == list->tail) {
            list->tail = prev;
        }
        if (tcb == list->RoundRobinPt) {
            list->RoundRobinPt = next;
        }
        prev->next = next;
        next->prev = prev;
        tcb->next = NULL;
        tcb->prev = NULL;
    }
    EndCritical(sr);
  
  // Change highestPriority
  uint32_t highestPriority = Scheduler.highestPriority;
  if (tcb->priority == highestPriority && list->head == NULL) {
    StartCritical();
    for (int i = highestPriority + 1; i < NUM_PRIORITIES; i++) {
      if (Scheduler.lists[i].head) {
        Scheduler.highestPriority = i;
        break;
      }
    }
    EndCritical(sr);
  }
}


static void BackgroundThreadRun(void(*task)(void)) {
    // Call Background Task
    (*task)();
    OS_Kill();
} 


///////////////////////// Periodic Tasks //////////////////////////

/*
  TimerG8 Handler
  Runs Periodic Threads
*/
void TIMG8_IRQHandler(void){
  if ((TIMG8->CPU_INT.IIDX) == 1) {
    (*CurrentSchedule.schedule[PTaskIdx].periodicTask)();

    // Accumulate next time
    PTaskIdx = (PTaskIdx + 1) % CurrentSchedule.scheulde_size;
    uint32_t nextTime = CurrentSchedule.schedule[PTaskIdx].timeToNext;
    TIMG8->COUNTERREGS.LOAD = nextTime - 1;
  }
}


static void GetHighestPriority() {
    StartCritical();
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (Scheduler.lists[i].head) {
            Scheduler.highestPriority = i;
            break;
        }
    }
    EndCritical(sr);
}