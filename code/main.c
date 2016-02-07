/*****************************************************************************
程序名：GPS+GSM例程
编写人：Yayi　 
注意UART2接GPS，UART1接GSM，MOSI接PA7,MISO接PA6，SCLK接PA5，CS接PA4      
网址:rtrobot.org                                       
*****************************************************************************/
#include "stm32f10x.h"
#include "usart2.h"
#include "usart1.h"
#include "gps.h"
#include "delay.h"

#define Power_Key_GPIO_Down GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define Power_Key_GPIO_Up GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define Yes		1
#define No		0

unsigned char GPS_Data_Temp[256];
gps_process_data gps;

/********测试GSM是否启动**********/
//unsigned char test_boot;
/********测试GSM是否注册网络**********/
//unsigned char test_net_register;
/********测试GSM是否已经链接上服务器**********/
//unsigned char test_TCP;


void GPS_Show()
{
	float la,lo;
	__align(4) unsigned char outbuf[50]; 
	
	la=gps.latitude;
	lo=gps.longitude;
	//while(la > 360.0 || lo > 360.0){
	//	DELAY_MS(1000);
	//	Usart1_Send("error");
	//	la = gps.latitude;
	//	lo = gps.longitude;
	//}
	sprintf((char *)outbuf,"Latitude:%.5f,Longitude:%.5f\r\n",la/=100000,lo/=100000);		//得到经纬度字符串 
	
	Usart1_Send(outbuf);
	USART_SendData(USART1, 0x1A);
	DELAY_MS(5000);

}

void Power_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_Power;																//定义端口变量
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 			//使端口GPIOA时钟生效，使用外设时钟2
		GPIO_Power.GPIO_Pin =  GPIO_Pin_13 ;												//设置GPIO子端口号	
  	GPIO_Power.GPIO_Mode = GPIO_Mode_Out_PP;     								//设置为推挽输出
  	GPIO_Power.GPIO_Speed = GPIO_Speed_50MHz; 									//设置端口输出频率为50MHz
  	GPIO_Init(GPIOB, &GPIO_Power);		  												//初始化端口
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

int main(void)
{	
	//test_boot= No;
	//test_net_register = No;
	//test_TCP = No;
	
	Usart2_Config(); 
	Nvic_Usart2_Config();
	Usart1_Config(); 
	Nvic_Usart1_Config();

	Power_GPIO_Config();
	Power_Key_GPIO_Down;
	
	//GSM启动
	Power_Key_GPIO_Up;
	DELAY_MS(10000);
	Power_Key_GPIO_Down;
	
	//测试是否已经开机
	while(test_boot==No)
	{
		Usart1_Send("AT\r\n");
		DELAY_MS(5000);		
		test_boot = Yes;
	}
	
	//测试SIM卡是否已经注册网络
	while(test_net_register==No)
	{
		Usart1_Send("AT+COPS?\r\n");
		DELAY_MS(5000);
		test_net_register = Yes;
	}
	
	Usart1_Send("AT+CGCLASS=\"B\"\r\n");
	DELAY_MS(5000);
	
	Usart1_Send("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	DELAY_MS(5000);
	
	Usart1_Send("AT+CGATT=1\r\n");
	DELAY_MS(5000);
	
	Usart1_Send("AT+CIPCSGP=1,\"CMNET\"\r\n");
	DELAY_MS(5000);
	
	Usart1_Send("AT+CLPORT=\"TCP\",\"2222\"\r\n");
	DELAY_MS(5000);
	
	Usart1_Send("AT+CIPSTART=\"TCP\",\"120.25.218.92\",\"9919\"\r\n");//change to our own server.
	
	DELAY_MS(10000);
	//while(test_TCP==No);
	
	Usart1_Send("AT+CIPSEND\r\n");
	DELAY_MS(1000);
	
	while(1)
	{
		if(GPS_Stop_flag==1)
		{
			int i;
			GPS_Stop_flag=0;			
			for(i=0;i<256;i++)
				GPS_Data_Temp[i]=GPS_Uart2_Data[i];
			GPS_Analysis(&gps,(u8*)GPS_Data_Temp);//分析字符串			
		}
		if(gps.fixmode==3)	
		{
			Usart1_Send("AT+CIPSEND\r\n");
			GPS_Show();
		}
	}
}
