/*
    * fifo_queue_3_semphr.c
    * Fifo queue implementation using 3 semaphores
    * vmph
    * 08/05/2019
    * 
*/
#include<stdint.h>
#include "semaphore.h"
#define FIFOSIZE	10

uint32_t volatile *ptPut;	// put next
uint32_t volatile *ptGet; // get next

uint32_t static fifo[FIFOSIZE];

int32_t currentSize; 	// 0 means fifo empty
int32_t roomLeft;			// 0 means fifo full
int32_t fifoMutex;		//exclusive acces to fifo

void OS_fifoInit(void)
{
	ptPut = ptGet = &fifo[0];	//empty
	OS_initSemaphore(&currentSize, 0);
	OS_initSemaphore(&roomLeft, FIFOSIZE);
	OS_initSemaphore(&fifoMutex, 1);
}

void OS_fifoPut(uint32_t data)
{
	OS_wait(&roomLeft);		// is there room?
	OS_wait(&fifoMutex);	// start critical section
	
	*ptPut = data;				// put
	ptPut++;							// place to put next
	
	if(ptPut == &fifo[FIFOSIZE])
	{
		ptPut = &fifo[0];	//wrap
	}
	
	OS_signal(&fifoMutex);	// end critical section
	OS_signal(&currentSize); //
}

uint32_t OS_fifoGet(void)
{
	uint32_t data;
	OS_wait(&currentSize);	// is there an element?
	OS_wait(&fifoMutex);		// start critical section
	
	data = *(ptGet);				// get data
	ptGet++;								// points to next data to get
	
	if(ptGet == &fifo[FIFOSIZE])
	{
		ptGet = &fifo[0];			//wrap
	}
	
	OS_signal(&fifoMutex);	// end critical section
	OS_signal(&roomLeft);
	
	return data;
}