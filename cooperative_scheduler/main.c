/*
    * main.c
    * An example user program that initializes the simple operating system
    * Schedule three independent threads using preemptive round robin
    * Each thread rapidly toggles a pin on Port D and increments its counter
    * TIMESLICE is how long each thread runs
    * vmph
    * 07/05/2019
    * 
*/

#include<stdint.h>
#include "TM4C123.h"                    // Device header
#include "OS.h"

#define TIME_1MS                50000
#define TIMESLICE               TIME_1MS * 2    // thread switch time in system time units

uint32_t Count1;   // number of times thread1 loops
uint32_t Count2;   // number of times thread2 loops
uint32_t Count3;   // number of times thread3 loops
#define GPIO_PORTD1             (*((volatile uint32_t *)0x40007008))
#define GPIO_PORTD2             (*((volatile uint32_t *)0x40007010))
#define GPIO_PORTD3             (*((volatile uint32_t *)0x40007020))

void Task1(void);
void Task2(void);
void Task3(void);

int main(void){
  OS_init();           // initialize, disable interrupts, 50 MHz
	SYSCTL->RCGCGPIO |= 0x08;							// activate clock for Port D
  while((SYSCTL->PRGPIO & 0x08) == 0){}; // allow time for clock to stabilize
  GPIOD->DIR |= 0x0E;             // make PD3-1 out
  GPIOD->AFSEL &= ~0x0E;          // disable alt funct on PD3-1
  GPIOD->DEN |= 0x0E;             // enable digital I/O on PD3-1
                                        // configure PD3-1 as GPIO
  GPIOD->PCTL = (GPIOD->PCTL & 0xFFFF000F)+0x00000000;
  GPIOD->AMSEL &= ~0x0E;          // disable analog functionality on PD3-1
  OS_addThread(&Task1, &Task2, &Task3);
  OS_launch(TIMESLICE); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}

void Task1(void){
  Count1 = 0;
  for(;;){
    Count1++;
    GPIO_PORTD1 ^= 0x02;      // toggle PD1
		OS_suspend();
  }
}
void Task2(void){
  Count2 = 0;
  for(;;){
    Count2++;
    GPIO_PORTD2 ^= 0x04;      // toggle PD2
		OS_suspend();
  }
}
void Task3(void){
  Count3 = 0;
  for(;;){
    Count3++;
    GPIO_PORTD3 ^= 0x08;      // toggle PD3
		OS_suspend();
  }
}