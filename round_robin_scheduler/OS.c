/*
    * OS.c
    * A very simple RTOS with round robin scheduler
    * vmph
    * 07/05/2019
    * 
*/

#include <stdint.h>
#include "OS.h"
#include "PLL.h"

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_INT_CTRL_R         (*((volatile uint32_t *)0xE000ED04))
#define NVIC_INT_CTRL_PENDSTSET 0x04000000  // Set pending SysTick interrupt
#define NVIC_SYS_PRI3_R         (*((volatile uint32_t *)0xE000ED20))  // Sys. Handlers 12 to 15 Priority

// function definitions in osasm.s
void OS_disableInterrupts(void); // Disable interrupts
void OS_enableInterrupts(void);  // Enable interrupts
int32_t startCritical(void);
void endCritical(int32_t primask);
void startOS(void);

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
    NVIC_ST_CTRL_R = 0;         //disable SysTick
    NVIC_ST_CURRENT_R = 0;      //any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0xE0000000;  //priority 7  
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
    NVIC_ST_RELOAD_R = timeSlice - 1;   //reload value
    NVIC_ST_CTRL_R = 0x00000007;        //enable, core clock and interrupt unmasked
    startOS();
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

void scheduler(void)
{
	ptRun = ptRun->next;	//Round robin
}

