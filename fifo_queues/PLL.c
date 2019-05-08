/*
    * PLL.h
    * A software function to change the bus freqeuncy using the PLL
    * vmph
    * 07/05/2019
    * 
*/

#include <stdint.h>
#include "TM4C123.h"                    // Device header
#include "PLL.h"

#define SYSCTL_RIS_PLLLRIS      0x00000040  // PLL Lock Raw Interrupt Status
#define SYSCTL_RCC_XTAL_M       0x000007C0  // Crystal Value
#define SYSCTL_RCC_XTAL_6MHZ    0x000002C0  // 6 MHz Crystal
#define SYSCTL_RCC_XTAL_8MHZ    0x00000380  // 8 MHz Crystal
#define SYSCTL_RCC_XTAL_16MHZ   0x00000540  // 16 MHz Crystal
#define SYSCTL_RCC2_USERCC2     0x80000000  // Use RCC2
#define SYSCTL_RCC2_DIV400      0x40000000  // Divide PLL as 400 MHz vs. 200
                                            // MHz
#define SYSCTL_RCC2_SYSDIV2_M   0x1F800000  // System Clock Divisor 2
#define SYSCTL_RCC2_SYSDIV2LSB  0x00400000  // Additional LSB for SYSDIV2
#define SYSCTL_RCC2_PWRDN2      0x00002000  // Power-Down PLL 2
#define SYSCTL_RCC2_BYPASS2     0x00000800  // PLL Bypass 2
#define SYSCTL_RCC2_OSCSRC2_M   0x00000070  // Oscillator Source 2
#define SYSCTL_RCC2_OSCSRC2_MO  0x00000000  // MOSC

// configure the system to get its clock from the PLL
void PLL_init(void){
  // 0) configure the system to use RCC2 for advanced features
  //    such as 400 MHz PLL and non-integer System Clock Divisor
  SYSCTL->RCC2 |= SYSCTL_RCC2_USERCC2;
  // 1) bypass PLL while initializing
  SYSCTL->RCC2 |= SYSCTL_RCC2_BYPASS2;
  // 2) select the crystal value and oscillator source
  SYSCTL->RCC &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field
  SYSCTL->RCC += SYSCTL_RCC_XTAL_16MHZ;// configure for 16 MHz crystal
  SYSCTL->RCC2 &= ~SYSCTL_RCC2_OSCSRC2_M;// clear oscillator source field
  SYSCTL->RCC2 += SYSCTL_RCC2_OSCSRC2_MO;// configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL->RCC2 &= ~SYSCTL_RCC2_PWRDN2;
  // 4) set the desired system divider and the system divider least significant bit
  SYSCTL->RCC2 |= SYSCTL_RCC2_DIV400;  // use 400 MHz PLL
  SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) // clear system clock divider field
                  + (SYSDIV2<<22);      // configure for 50 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL->RIS & SYSCTL_RIS_PLLLRIS)==0){};
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL->RCC2 &= ~SYSCTL_RCC2_BYPASS2;
}