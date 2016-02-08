#include "usart1.h"
#include "stm32f10x.h"
//#include <stdio.h>
//RTrobot.org
//��ֹ������ҵ��Ϊ
#define Yes		1
#define No		0


char GSM_Uart2_Data[256]={ 0 };
char GSM_Data[256]={ 0 };

unsigned char GSM_Uart2_Start_flag=0;


extern unsigned char GSM_Buf[BUFF_SIZE] = { 0 };
unsigned int GSM_Buf_index  = 0;
unsigned int GSM_Buf_msg = 0;
unsigned int GSM_Buf_read_index = 0;



/********����GSM�Ƿ�����**********/
unsigned char test_boot;
/********����GSM�Ƿ�ע������**********/
unsigned char test_net_register;
/********����GSM�Ƿ��Ѿ������Ϸ�����**********/
unsigned char test_TCP;



/********************************************************************************************
����1���ͺ���
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
�˿ڳ�ʼ������
********************************************************************************************/
void Usart1_Config(void)
{		
	GPIO_InitTypeDef GPIO_Uart;																											//����˿ڱ���
	USART_InitTypeDef USART_Uart;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE); 		//ʹ�˿�GPIOAʱ����Ч��ʹ������ʱ��2
	
	//TX
 	GPIO_Uart.GPIO_Pin = GPIO_Pin_9 ;																								//����GPIO�Ӷ˿ں�	
  	GPIO_Uart.GPIO_Mode = GPIO_Mode_AF_PP;     																		//����Ϊ�����������
  	GPIO_Uart.GPIO_Speed = GPIO_Speed_50MHz; 																			//���ö˿����Ƶ��Ϊ50MHz
  	GPIO_Init(GPIOA, &GPIO_Uart);		  																						//��ʼ���˿�
	
	//RX
	GPIO_Uart.GPIO_Pin = GPIO_Pin_10;																								//����GPIO�Ӷ˿ں�
	GPIO_Uart.GPIO_Mode = GPIO_Mode_IN_FLOATING;																		//��������
	GPIO_Init(GPIOA, &GPIO_Uart);																										//��ʼ���˿�
	
	USART_Uart.USART_BaudRate = 9600;																								//���ò�����
	USART_Uart.USART_WordLength = USART_WordLength_8b;															//��������λ
	USART_Uart.USART_StopBits = USART_StopBits_1;																		//����ֹͣλ
	USART_Uart.USART_Parity = USART_Parity_No ;																			//����У��λ
	USART_Uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;					//�ر�Ӳ������ģʽ
	USART_Uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;													//ʹ��
	USART_Init(USART1,&USART_Uart);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

/********************************************************************************************
�жϳ�ʼ������
********************************************************************************************/
void Nvic_Usart1_Config(void)
{
	NVIC_InitTypeDef NVIC_Uart1;																										//�����жϿ��������� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);																	//�����ж����ȼ�
		
	NVIC_Uart1.NVIC_IRQChannel = USART1_IRQn;	 																			//����Ϊ����1ȫ���ж�
	NVIC_Uart1.NVIC_IRQChannelPreemptionPriority = 1;																//������ռ���ȼ�
	NVIC_Uart1.NVIC_IRQChannelSubPriority = 1;																			//���ô����ȼ�
	NVIC_Uart1.NVIC_IRQChannelCmd = ENABLE;																					//IRQͨ��ʹ��
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
		if (GSM_Buf_index >= BUFF_SIZE){
			GSM_Buf_index = GSM_Buf_index % BUFF_SIZE;
		}
		
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
int ISOK(void){
	int t_i;

	for (t_i = 0;t_i < GSM_Buf_index;t_i++)
	{
		if(GSM_Buf[t_i] == 'O' && GSM_Buf[t_i + 1] == 'K')
		{
			return 1;
		}
	}
	return 0;
	
}

//GSM_��Ϣ����
int GSM_Msg_Send(unsigned char *str)
{
	int error_times;
	error_times = 0;
	
	while(1)
	{
		GSM_Buf_index =0;
		GSM_Buf_read_index = 0;
		Usart1_Send("AT+CIPSEND\r\r");
	
		Usart1_Send(str);
		USART_SendData(USART1,0x1A);
		
		//while(GSM_Buf_index < GSM_Buf_read_index +2)
		//{
		//}
		DELAY_MS(800);
		if(ISOK())
			return 0; 
		error_times ++;
		if(error_times>3)
		{
			return 1;
		}
		
	}		
}
	

//����Ƿ񿪻�
int  GSM_Is_Boot(void)
{
	GSM_Buf_index = 0;
	GSM_Buf_read_index = 0;
	Usart1_Send("AT\r\n");
	DELAY_MS(500);
	if(GSM_Buf_index > GSM_Buf_read_index)
	{
		return 1;
	}else{
		return 0;
	}
}



	
