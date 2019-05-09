/*
    * semaphore.c
    * spin-lock semaphore implementation with cooperation
    * vmph
    * 08/05/2019
    * 
*/

#include "semaphore.h"
#include "OS.h"

extern tcb_t *ptRun;

/*
    * OS_initSemaphore
    * initialize the value of a given semaphore
    * input: A semaphore that you want to initialize and the value wich the semaphore will be initialized
    * output: none
*/
void OS_initSemaphore(int32_t *semaphore, int32_t value)
{
	*semaphore = value;
}

/*
    * OS_wait
    * decrement the value of the given semaphore, if the semaphore is less than 1 the thread will be blocked
    * input: A semaphore
    * output: none
*/
void OS_wait(int32_t *semaphore)
{
	OS_disableInterrupts();
	
	*semaphore = (*semaphore) - 1;		//decrement the semaphore
	
	if( (*semaphore) < 0 )						//Another thread has taken the semaphore
	{
		ptRun->blocked = semaphore;			//reason it is blocked
		OS_enableInterrupts();
		OS_suspend();										//run thread switcher
	}
	
	OS_enableInterrupts();
}

/*
    * OS_signal
    * increment the value of the given semaphore, if the semaphore is less or equal than 0 the function will
		* wake up the first thread that is blocked by the semaphore 
    * input: A semaphore
    * output: none
*/
void OS_signal(int32_t *semaphore)
{
	tcb_t *pt;
	OS_disableInterrupts();
	*semaphore = (*semaphore) + 1;
	
	if( (*semaphore) <= 0 )	//At least one thread is blocked by this semaphore
	{
		pt = ptRun->next;			//points to the next thread	 
		while( pt->blocked != semaphore) //search for one blocked thread
		{
			pt = pt->next;
		}
		pt->blocked = 0;		//wake up this this one
	}
	
	OS_enableInterrupts();
}
