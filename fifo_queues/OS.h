/*
    * OS.h
    * A very simple RTOS with round robin scheduler
    * vmph
    * 07/05/2019
    * 
*/

#ifndef _OS_H_
#define _OS_H_

#include<stdint.h>

typedef struct tcb
{
    int32_t *sp;                //pointer to stack, valid for threads not runnig
    struct tcb *next;           //linked-list pointer
		int32_t *blocked;						//pointer to semaphore that blocked this thread
}tcb_t;

// function definitions in osasm.s
void OS_disableInterrupts(void); // Disable interrupts
void OS_enableInterrupts(void);  // Enable interrupts
int32_t startCritical(void);
void endCritical(int32_t primask);
void startOS(void);

/*
    * OS_init
    * initialize operating system, disable interrupts
    * input: none
    * output: none
*/
void OS_init(void);

/*
    * OS_addThread
    * add three foreground threads to the scheduler
    * input: three pointers to void/void tasks
    * output: 1 if successful, 0 if these threads cannot be added
*/
int OS_addThread(void(*task0)(void),
                  void(*task1)(void),
                  void(*task2)(void));

/*
    * OS_launch
    * start the scheduler, enable interrupts
    * input: number of 20ns clock cycles for each time slice
    * output: none (does not return)
*/                 
void OS_launch(uint32_t timeSlice);
									
/*
    * OS_suspend
    * triggers a SysTick interrupt which will stop the current thread and will run the next thread
    * input: none
    * output: none 
*/
void OS_suspend(void);									

#endif