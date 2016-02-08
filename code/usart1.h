#ifndef __USART1_H
#define	__USART1_H

//RTrobot.org
//禁止用于商业行为

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
void GSM_Msg_Send(unsigned char *str);
/*char GSM_Buf[1024] = {0};
unsigned int GSM_Buf_index  = 0;
unsigned int GSM_Buf_msg = 0;
unsigned int GSM_Buf_read_index = 0;*/

//unsigned int GSM_Uart2_Data_Num=0;

/********测试GSM是否启动**********/
extern unsigned char test_boot;
/********测试GSM是否注册网络**********/
extern unsigned char test_net_register;
/********测试GSM是否已经链接上服务器**********/
extern unsigned char test_TCP;

#endif /* __USART1_H */
