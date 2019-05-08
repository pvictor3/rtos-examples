/*
    * semaphore.c
    * spin-lock semaphore implementation with cooperation
    * vmph
    * 08/05/2019
    * 
*/

#include "semaphore.h"
#include "OS.h"

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
    * 
    * input: A semaphore
    * output: none
*/
void OS_wait(int32_t *semaphore)
{
	OS_disableInterrupts();
	while( (*semaphore) == 0 )
	{
		OS_enableInterrupts();
		OS_suspend();										//run thread switcher
		OS_disableInterrupts();
	}
	*semaphore = (*semaphore) - 1;
	OS_enableInterrupts();
}

/*
    * OS_signal
    * 
    * input: A semaphore
    * output: none
*/
void OS_signal(int32_t *semaphore)
{
	OS_disableInterrupts();
	*semaphore = (*semaphore) + 1;
	OS_enableInterrupts();
}
