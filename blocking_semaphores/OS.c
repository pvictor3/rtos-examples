/*
    * OS.c
    * A very simple RTOS with round robin scheduler
    * vmph
    * 07/05/2019
    * 
*/

#include <stdint.h>
#include "TM4C123.h"                    // Device header
#include "OS.h"
#include "PLL.h"

static void setInitialStack(int i);

#define NUMTHREADS  3           //Maximun number of threads
#define STACKSIZE   100         //number of 32-bit words in stack

typedef struct tcb
{
    int32_t *sp;                //pointer to stack, valid for threads not runnig
    struct tcb *next;           //linked-list pointer 
}tcb_t;

tcb_t tcbs[NUMTHREADS];
tcb_t *ptRun;                           //currently running thread
int32_t stacks[NUMTHREADS][STACKSIZE];

/*
    * OS_init
    * initialize operating system, disable interrupts
    * input: none
    * output: none
*/
void OS_init(void)
{
    OS_disableInterrupts();
    PLL_init();                 //set clock to 50MHz
		SysTick->CTRL = 0;					//disable SysTick
    SysTick->VAL = 0;      			//any write to current clears it
    SCB->SHP[11] = 0xE0;  			//priority 7  
}

/*
    * OS_addThread
    * add three foreground threads to the scheduler
    * input: three pointers to void/void tasks
    * output: 1 if successful, 0 if these threads cannot be added
*/
int OS_addThread(void(*task0)(void), void(*task1)(void), void(*task2)(void))
{
    int32_t status;
    status = startCritical();

    tcbs[0].next = &tcbs[1];    //0 points to 1
    tcbs[1].next = &tcbs[2];    //1 points to 2
    tcbs[2].next = &tcbs[0];    //2 points to 0

    setInitialStack(0);
    stacks[0][STACKSIZE - 2] = (int32_t)(task0); //PC
    setInitialStack(1);
    stacks[1][STACKSIZE - 2] = (int32_t)(task1); //PC
    setInitialStack(2);
    stacks[2][STACKSIZE - 2] = (int32_t)(task2); //PC

    ptRun = &tcbs[0];           //thread 0 will run first

    endCritical(status);

    return 1;
}

/*
    * OS_launch
    * start the scheduler, enable interrupts
    * input: number of 20ns clock cycles for each time slice
    * output: none (does not return)
*/                 
void OS_launch(uint32_t timeSlice)
{
    SysTick->LOAD = timeSlice - 1;   //reload value
    SysTick->CTRL = 0x00000007;        //enable, core clock and interrupt unmasked
    startOS();
}

/*
    * OS_suspend
    * triggers a SysTick interrupt which will stop the current thread and will run the next thread
    * input: none
    * output: none 
*/
void OS_suspend(void)
{
	SCB->ICSR = 0x04000000;	//trigger systick
}


/*
    * setInitialStack
    * initialize the stack of the given thread
    * input: number of thread
    * output: none
*/
static void setInitialStack(int i)
{
	tcbs[i].sp = &stacks[i][STACKSIZE - 16];	//thread stack pointer
	stacks[i][STACKSIZE - 1] = 0x01000000;	//Thumb bit
	stacks[i][STACKSIZE - 3] = 0x14141414;	//R14
	stacks[i][STACKSIZE - 4] = 0x12121212;	//R12
	stacks[i][STACKSIZE - 5] = 0x03030303; // R3
	stacks[i][STACKSIZE - 6] = 0x02020202; // R2
	stacks[i][STACKSIZE - 7] = 0x01010101; // R1
	stacks[i][STACKSIZE - 8] = 0x00000000; // R0
	stacks[i][STACKSIZE - 9] = 0x11111111; // R11
	stacks[i][STACKSIZE - 10] = 0x10101010; // R10
	stacks[i][STACKSIZE - 11] = 0x09090909; // R9
	stacks[i][STACKSIZE - 12] = 0x08080808; // R8
	stacks[i][STACKSIZE - 13] = 0x07070707; // R7
	stacks[i][STACKSIZE - 14] = 0x06060606; // R6
	stacks[i][STACKSIZE - 15] = 0x05050505; // R5
	stacks[i][STACKSIZE - 16] = 0x04040404;	//R4
}

/*
    * scheduler
    * selects the next thread to execute
    * input: none
    * output: none
*/
void scheduler(void)
{
	ptRun = ptRun->next;	//Round robin
}

