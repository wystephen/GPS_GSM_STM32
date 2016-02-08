#include "usart2.h"
#include "stm32f10x.h"
//RTrobot.org
//��ֹ������ҵ��Ϊ

char GPS_Uart2_Data[256]={ 0 };
char GPS_Data[256]={ 0 };
unsigned int GPS_Uart2_Data_Num=0;
unsigned char GPS_Uart2_Start_flag=0;
unsigned char GPS_Stop_flag=0;

/********************************************************************************************
����2���ͺ���
********************************************************************************************/
void Usart2_Send(unsigned char *str)
{
	while(*str != '\0')
	{		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
		USART_SendData(USART2 , *str);
		str++;
	}
}


/********************************************************************************************
�˿ڳ�ʼ������
********************************************************************************************/
void Usart2_Config(void)
{		
	GPIO_InitTypeDef GPIO_Usart2;																//����˿ڱ���
	USART_InitTypeDef USART_Usart2;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			//ʹ�˿�GPIOAʱ����Ч��ʹ������ʱ��2
	
	//TX
 	GPIO_Usart2.GPIO_Pin = GPIO_Pin_2 ;													//����GPIO�Ӷ˿ں�	
  GPIO_Usart2.GPIO_Mode = GPIO_Mode_AF_PP;   								  //����Ϊ�����������
  GPIO_Usart2.GPIO_Speed = GPIO_Speed_50MHz; 									//���ö˿����Ƶ��Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_Usart2);		  												//��ʼ���˿�
	
	//RX
	GPIO_Usart2.GPIO_Pin = GPIO_Pin_3;													//����GPIO�Ӷ˿ں�
	GPIO_Usart2.GPIO_Mode = GPIO_Mode_IN_FLOATING;							//��������
	GPIO_Init(GPIOA, &GPIO_Usart2);															//��ʼ���˿�
	
	USART_Usart2.USART_BaudRate = 9600;													//���ò�����
	USART_Usart2.USART_WordLength = USART_WordLength_8b;				//��������λ
	USART_Usart2.USART_StopBits = USART_StopBits_1;							//����ֹͣλ
	USART_Usart2.USART_Parity = USART_Parity_No ;								//����У��λ
	USART_Usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//�ر�Ӳ������ģʽ
	USART_Usart2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//ʹ��
	USART_Init(USART2,&USART_Usart2); 													//��ʼ������
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);							//���ý��������ж�
	USART_Cmd(USART2, ENABLE);																	//��������һ
}

/********************************************************************************************
�жϳ�ʼ������
********************************************************************************************/
void Nvic_Usart2_Config(void)
{
	NVIC_InitTypeDef NVIC_Usart2;																//�����жϿ��������� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);							//�����ж����ȼ�
	
	NVIC_Usart2.NVIC_IRQChannel = USART2_IRQn;	 								//����Ϊ����1ȫ���ж�
	NVIC_Usart2.NVIC_IRQChannelPreemptionPriority = 1;					//������ռ���ȼ�
	NVIC_Usart2.NVIC_IRQChannelSubPriority = 0;									//���ô����ȼ�
	NVIC_Usart2.NVIC_IRQChannelCmd = ENABLE;										//IRQͨ��ʹ��
	NVIC_Init(&NVIC_Usart2);
}

void USART2_IRQHandler(void)
{
	unsigned char Data_char_usart2;	
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		if(GPS_Stop_flag == 1)
		{
			Data_char_usart2 = USART2->DR;
			return;
		}
		Data_char_usart2=USART2->DR;
		if(Data_char_usart2=='$')
		{
			GPS_Uart2_Start_flag=1;
			GPS_Uart2_Data_Num=0;
		}
		if(GPS_Uart2_Start_flag==1)
		{
			GPS_Uart2_Data[GPS_Uart2_Data_Num]=Data_char_usart2;
			GPS_Uart2_Data_Num++;			
			if(Data_char_usart2=='\n')
			{
				GPS_Stop_flag=1;
				GPS_Uart2_Data_Num=0;
				GPS_Uart2_Start_flag=0;
			}	
		}
	}	
}
