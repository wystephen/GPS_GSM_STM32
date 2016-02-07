#include "delay.h"
static __IO u32 TimingDelay;
/********************************************************************************************
SysTick��ʼ������
SystemCoreClock/1000 	1ms�ж�һ��
SystemCoreClock/100000 	10us�ж�һ��
SystemCoreClock/1000000 1us�ж�һ��
SystemCoreClockΪ72000000
********************************************************************************************/
void SysTick_Init(void)
{	
	while(SysTick_Config(SystemCoreClock/1000000));				//������Ҫ��ʱ�������ֵ���������жϣ�ʹ��AHBʱ��Դ�����72MHz
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;					//�رն�ʱ��	
}

/********************************************************************************************
���ӳٺ���
********************************************************************************************/
void DELAY_100MS(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/10));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;			//�򿪶�ʱ��
	while(TimingDelay != 0);					//�ȴ�1�����	
}

/********************************************************************************************
���ӳٺ���
********************************************************************************************/
void DELAY_US(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/1000000));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;			//�򿪶�ʱ��
	while(TimingDelay != 0);					//�ȴ�1�����	
}

/********************************************************************************************
���ӳٺ���
********************************************************************************************/
void DELAY_MS(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/1000));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;						//�򿪶�ʱ��



	while(TimingDelay != 0);															//�ȴ�1�����	
}

/********************************************************************************************
���ӳ��Լ�����
TimingDelayΪ�����ӳ�ʱ�䣬��λ�����롣 
********************************************************************************************/
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0)
	{ 
		TimingDelay--;
	}
}
