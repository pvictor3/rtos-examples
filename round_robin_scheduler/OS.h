/*
    * OS.h
    * A very simple RTOS with round robin scheduler
    * vmph
    * 07/05/2019
    * 
*/

#ifndef _OS_H_
#define _OS_H_

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

#endif