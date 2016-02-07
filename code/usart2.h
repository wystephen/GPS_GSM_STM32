#ifndef __USART2_H
#define	__USART2_H
//RTrobot.org
//禁止用于商业行为

#include "stm32f10x.h"
#include <stdio.h>

extern unsigned int run_time;

void Usart2_Config(void);
void Nvic_Usart2_Config(void);
void Usart2_Send(unsigned char *str);

extern unsigned char GPS_Stop_flag;
extern char GPS_Uart2_Data[256];



#endif /* __USART1_H */
