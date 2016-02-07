#include "delay.h"
static __IO u32 TimingDelay;
/********************************************************************************************
SysTick初始化函数
SystemCoreClock/1000 	1ms中断一次
SystemCoreClock/100000 	10us中断一次
SystemCoreClock/1000000 1us中断一次
SystemCoreClock为72000000
********************************************************************************************/
void SysTick_Init(void)
{	
	while(SysTick_Config(SystemCoreClock/1000000));				//设置需要定时的脉冲初值，并允许中断，使用AHB时钟源，最高72MHz
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;					//关闭定时器	
}

/********************************************************************************************
简单延迟函数
********************************************************************************************/
void DELAY_100MS(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/10));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;			//打开定时器
	while(TimingDelay != 0);					//等待1秒完成	
}

/********************************************************************************************
简单延迟函数
********************************************************************************************/
void DELAY_US(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/1000000));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;			//打开定时器
	while(TimingDelay != 0);					//等待1秒完成	
}

/********************************************************************************************
简单延迟函数
********************************************************************************************/
void DELAY_MS(__IO u32 Time)
{ 
	while(SysTick_Config(SystemCoreClock/1000));	
	TimingDelay = Time;	
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;						//打开定时器
	while(TimingDelay != 0);															//等待1秒完成	
}

/********************************************************************************************
简单延迟自减函数
TimingDelay为所需延迟时间，单位：毫秒。 
********************************************************************************************/
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0)
	{ 
		TimingDelay--;
	}
}
