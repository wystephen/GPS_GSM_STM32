#ifndef __USART1_H
#define	__USART1_H

//RTrobot.org
//��ֹ������ҵ��Ϊ

#include "stm32f10x.h"
//#include <stdio.h>
#include "delay.h"

#define BUFF_SIZE 256
extern unsigned int run_time;

void Usart1_Config(void);
void Nvic_Usart1_Config(void);
void Usart1_Send(unsigned char *str);
void Data_Process(void);

void Get_IMEI(unsigned char str[100]);
int GSM_Msg_Send(unsigned char *str);


int GSM_Is_Boot(void);
//int GSM_Is_Net_Register(void);

int ISOK(void);

/*char GSM_Buf[1024] = {0};
unsigned int GSM_Buf_index  = 0;
unsigned int GSM_Buf_msg = 0;
unsigned int GSM_Buf_read_index = 0;*/

//unsigned int GSM_Uart2_Data_Num=0;

/********����GSM�Ƿ�����**********/
extern unsigned char test_boot;
/********����GSM�Ƿ�ע������**********/
extern unsigned char test_net_register;
/********����GSM�Ƿ��Ѿ������Ϸ�����**********/
extern unsigned char test_TCP;

#endif /* __USART1_H */
