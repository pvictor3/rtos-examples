/*
    * PLL.h
    * A software function to change the bus freqeuncy using the PLL
    * vmph
    * 07/05/2019
    * 
*/

#ifndef _PLL_H_
#define _PLL_H_

//bus frequency is 400Mhz / (SYSDIV2 + 1) = 400 / ( 7 + 1 ) = 50Mhz
#define SYSDIV2 7 

// configure the system to get its clock from the PLL
void PLL_init(void);

#endif