#include "usart1.h"
#include "stm32f10x.h"
//RTrobot.org
//��ֹ������ҵ��Ϊ
#define Yes		1
#define No		0


char GSM_Uart2_Data[256]={ 0 };
char GSM_Data[256]={ 0 };
unsigned int GSM_Uart2_Data_Num=0;
unsigned char GSM_Uart2_Start_flag=0;


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
	USART_Cmd(USART1, ENABLE);
}

/********************************************************************************************
�жϳ�ʼ������
********************************************************************************************/
void Nvic_Usart1_Config(void)
{
	NVIC_InitTypeDef NVIC_Uart1;																										//�����жϿ��������� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																	//�����ж����ȼ�
		
	NVIC_Uart1.NVIC_IRQChannel = USART2_IRQn;	 																			//����Ϊ����1ȫ���ж�
	NVIC_Uart1.NVIC_IRQChannelPreemptionPriority = 1;																//������ռ���ȼ�
	NVIC_Uart1.NVIC_IRQChannelSubPriority = 1;																			//���ô����ȼ�
	NVIC_Uart1.NVIC_IRQChannelCmd = ENABLE;																					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_Uart1);
}

void USART1_IRQHandler(void)
{
	unsigned char Data_char_usart2;	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Data_char_usart2=USART1->DR;
		GSM_Uart2_Data[GSM_Uart2_Data_Num]=Data_char_usart2;
		GSM_Uart2_Data_Num++;			
		if(Data_char_usart2=='\n')
		{
			if(GSM_Uart2_Data[0]=='O'&&GSM_Uart2_Data[1]=='K')
				test_boot=Yes;
			else
				test_boot=No;
				//test_boot = Yes;
			
			if(GSM_Uart2_Data[12]=='C')
        test_net_register=Yes;  
			else
			{
				test_net_register=No;
				//test_net_register=Yes;	
			}				
			
			if(GSM_Uart2_Data[8]=='O'&&GSM_Uart2_Data[9]=='K')
				test_TCP=Yes;
			else
			{
				test_TCP=No;
				//test_TCP=Yes;
			}
			
			GSM_Uart2_Data_Num=0;	
		}
	}	
}
