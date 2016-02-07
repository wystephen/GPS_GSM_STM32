#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"

void TimingDelay_Decrement(void);
void DELAY_MS(__IO u32 Time);
void SysTick_Init(void);
void DELAY_US(__IO u32 Time);
void DELAY_100MS(__IO u32 Time);
#endif
