/*
    * main.c
    * skip the bounce of a switch
    * TIMESLICE is how long each thread runs
    * vmph
    * 07/05/2019
    * 
*/

#include<stdint.h>
#include "TM4C123.h"                    // Device header
#include "OS.h"
#include "semaphore.h"

#define TIME_1MS                50000
#define TIMESLICE               TIME_1MS * 2    // thread switch time in system time units

int32_t sw1, sw2;								//semaphores
uint8_t last1, last2;						//last state of the switch

void switchInit(void);
void switchTask1(void);
void switchTask2(void);

int main(void){
  OS_init();           // initialize, disable interrupts, 50 MHz
	switchInit();
	
	//TODO: periodic task to decrement sleep counter
	
  OS_addThread(&switchTask1, 0, &switchTask2, 0);
  OS_launch(TIMESLICE); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}

void switchInit(void)
{
	SYSCTL->RCGCGPIO |= (1<<5);		//activate clock for GPIO PORT F
	OS_initSemaphore(&sw1, 0);
	OS_initSemaphore(&sw2, 0);
	GPIOF->LOCK = 0x4C4F434B;				//unlock GPIO Port F
	GPIOF->CR = 0x1F;								//allow changes to PF4-0
	GPIOF->DIR &= ~0x11;						//make PF4,PF0 inputs
	GPIOF->DEN |= 0x11;							//enable digital I/O on PF4,PF0
	GPIOF->PUR |= 0x11;							//pullup on PF4,PF0
	GPIOF->IS &= ~0x11;							//PF4, PF0 are edge sensitive
	GPIOF->IBE |= 0x11;							//PF4,PF0 are both edges
	GPIOF->ICR = 0x11;							//clear flags
	GPIOF->IM |= 0x11;							//unmask interrupts on PF4,PF0
	NVIC->IP[30] = 0x20;						//priority 1
	NVIC->ISER[0] |= (1<<30);				//enable interrupt 30 in NVIC
}

void switchTask1(void)					//high priority main thread
{
	last1 = GPIOF->DATA & 0x10;
	while(1)
	{
		OS_wait(&sw1);							//wait for SW1 to be touched or released
		if(last1)										//was previously not touched
		{	
			//touch1();
		}else
		{
			//release1();
		}
		
		OS_sleep(10);								//wait for bouncing to be over
		last1 = GPIOF->DATA & 0x10;
		GPIOF->IM |= 0x10;					//unmask interrupts on PF4
		GPIOF->ICR = 0x10;					//clears flag 4
	}
}

void switchTask2(void)					//high priority main thread
{
	last2 = GPIOF->DATA & 0x01;
	while(1)
	{
		OS_wait(&sw2);							//wait for SW2 to be touched or released
		if(last2)										//was previously not touched
		{	
			//touch2();
		}else
		{
			//release2();
		}
		
		OS_sleep(10);								//wait for bouncing to be over
		last2 = GPIOF->DATA & 0x10;
		GPIOF->IM |= 0x01;					//unmask interrupts on PF0
		GPIOF->ICR = 0x01;					//clears flag 0
	}
}

void GPIOPortF_Handler(void)
{
	if(GPIOF->RIS & 0x10)					//poll PF4
	{
		GPIOF->ICR = 0x10;					//clears flag 4
		OS_signal(&sw1);						//signal sw1 ocurred
		GPIOF->IM &= ~0x10;					//disables interrupt on PF4
	}
	
	if(GPIOF->RIS & 0x01)					//poll PF0
	{
		GPIOF->ICR = 0x01;					//clears flag 0
		OS_signal(&sw2);						//signal sw2 ocurred
		GPIOF->IM &= ~0x01;					//mask interrrupt on PF0
	}
	OS_suspend();									//calls the scheduler
}
