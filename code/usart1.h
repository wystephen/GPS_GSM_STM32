#ifndef __USART1_H
#define	__USART1_H

//RTrobot.org
//��ֹ������ҵ��Ϊ

#include "stm32f10x.h"
#include <stdio.h>

extern unsigned int run_time;

void Usart1_Config(void);
void Nvic_Usart1_Config(void);
void Usart1_Send(unsigned char *str);

/********����GSM�Ƿ�����**********/
extern unsigned char test_boot;
/********����GSM�Ƿ�ע������**********/
extern unsigned char test_net_register;
/********����GSM�Ƿ��Ѿ������Ϸ�����**********/
extern unsigned char test_TCP;

#endif /* __USART1_H */
