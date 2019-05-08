/*
    * semaphore.h
    * spin-lock semaphore implementation with cooperation
    * vmph
    * 08/05/2019
    * 
*/

#ifndef	_SEMAPHORE_H_	
#define	_SEMAPHORE_H_

#include<stdint.h>

/*
    * OS_initSemaphore
    * initialize the value of a given semaphore
    * input: A semaphore that you want to initialize and the value wich the semaphore will be initialized
    * output: none
*/
void OS_initSemaphore(int32_t *semaphore, int32_t value);

/*
    * OS_wait
    * 
    * input: A semaphore
    * output: none
*/
void OS_wait(int32_t *semaphore);

/*
    * OS_signal
    * 
    * input: A semaphore
    * output: none
*/
void OS_signal(int32_t *semaphore);

#endif