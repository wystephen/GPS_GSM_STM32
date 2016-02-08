#include "usart1.h"
#include "stm32f10x.h"
//#include <stdio.h>
//RTrobot.org
//禁止用于商业行为
#define Yes		1
#define No		0


char GSM_Uart2_Data[256]={ 0 };
char GSM_Data[256]={ 0 };

unsigned char GSM_Uart2_Start_flag=0;


extern unsigned char GSM_Buf[BUFF_SIZE] = { 0 };
unsigned int GSM_Buf_index  = 0;
unsigned int GSM_Buf_msg = 0;
unsigned int GSM_Buf_read_index = 0;



/********测试GSM是否启动**********/
unsigned char test_boot;
/********测试GSM是否注册网络**********/
unsigned char test_net_register;
/********测试GSM是否已经链接上服务器**********/
unsigned char test_TCP;



/********************************************************************************************
串口1发送函数
********************************************************************************************/
void Usart1_Send(unsigned char *str)
{
	while(*str != '\0')
	{		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
		USART_SendData(USART1 , *str);
		str++;
	}
}


/********************************************************************************************
端口初始化函数
********************************************************************************************/
void Usart1_Config(void)
{		
	GPIO_InitTypeDef GPIO_Uart;																											//定义端口变量
	USART_InitTypeDef USART_Uart;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE); 		//使端口GPIOA时钟生效，使用外设时钟2
	
	//TX
 	GPIO_Uart.GPIO_Pin = GPIO_Pin_9 ;																								//设置GPIO子端口号	
  	GPIO_Uart.GPIO_Mode = GPIO_Mode_AF_PP;     																		//设置为复用推挽输出
  	GPIO_Uart.GPIO_Speed = GPIO_Speed_50MHz; 																			//设置端口输出频率为50MHz
  	GPIO_Init(GPIOA, &GPIO_Uart);		  																						//初始化端口
	
	//RX
	GPIO_Uart.GPIO_Pin = GPIO_Pin_10;																								//设置GPIO子端口号
	GPIO_Uart.GPIO_Mode = GPIO_Mode_IN_FLOATING;																		//浮空输入
	GPIO_Init(GPIOA, &GPIO_Uart);																										//初始化端口
	
	USART_Uart.USART_BaudRate = 9600;																								//设置波特率
	USART_Uart.USART_WordLength = USART_WordLength_8b;															//设置数据位
	USART_Uart.USART_StopBits = USART_StopBits_1;																		//设置停止位
	USART_Uart.USART_Parity = USART_Parity_No ;																			//设置校检位
	USART_Uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;					//关闭硬件控制模式
	USART_Uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;													//使能
	USART_Init(USART1,&USART_Uart);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

/********************************************************************************************
中断初始化函数
********************************************************************************************/
void Nvic_Usart1_Config(void)
{
	NVIC_InitTypeDef NVIC_Uart1;																										//设置中断控制器变量 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);																	//设置中断优先级
		
	NVIC_Uart1.NVIC_IRQChannel = USART1_IRQn;	 																			//设置为串口1全局中断
	NVIC_Uart1.NVIC_IRQChannelPreemptionPriority = 1;																//设置抢占优先级
	NVIC_Uart1.NVIC_IRQChannelSubPriority = 1;																			//设置从优先级
	NVIC_Uart1.NVIC_IRQChannelCmd = ENABLE;																					//IRQ通道使能
	NVIC_Init(&NVIC_Uart1);
}

void USART1_IRQHandler(void)
{
	unsigned char Data_char_usart1;	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Data_char_usart1 = USART1->DR;

		GSM_Buf[GSM_Buf_index] = Data_char_usart1;
		
		if(Data_char_usart1 == '\n'){
			GSM_Buf_msg++;
		}
		
		GSM_Buf_index  = GSM_Buf_index + 1;
		//if (GSM_Buf_index >= BUFF_SIZE){
			//GSM_Buf_index /= BUFF_SIZE;
		
	}	
}

void Get_IMEI(unsigned char str[100])
{
	int k = 0;
	//unsigned char *str = *strt;
	unsigned char strt[100];
	unsigned int i =0;
	GSM_Buf_index = 0;
	GSM_Buf_read_index = GSM_Buf_index;

	USART_SendData(USART1,0x1A);
	
	GSM_Buf_index = 0;
	GSM_Buf_read_index = 0;
	
	Usart1_Send("AT+GSN\r\n");

	DELAY_MS(500);
	
	for(i = 0;i< GSM_Buf_index ;i++){
		strt[i] = GSM_Buf[i];
	}
	sprintf(strt,"len:");

	str[99] = '\0';

	for(i = 0;i<99;i++)
	{
		if(strt[i] > '0' && strt[i]<'9')
		{
			str[k] = strt[i];
			k = k+ 1;
		}
	}
}
//
void GSM_Msg_Send(unsigned char *str)
{
	Usart1_Send("AT+CIPSEND\r\r");
	DELAY_MS(800);
	Usart1_Send(str);
	USART_SendData(USART1,0x1A);
}
	
	
