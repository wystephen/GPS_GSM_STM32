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

//#include <stdio.h>

#define Power_Key_GPIO_Down GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define Power_Key_GPIO_Up GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define Yes		1
#define No		0

unsigned char GPS_Data_Temp[256];
gps_process_data gps;

unsigned char imei[100] = {0} ;//储存 imei信息

int GSM_Send_Error=0;//GSM发送错误计数器

int GPS_Get_Error  = 0;//记录进入while循环但是没有接收到数据
int GPS_Ana_Error = 0;//记录进入while 循环 没有 GPS解析成功次数。


void GPS_Show()
{
	float la,lo;
	__align(4) unsigned char outbuf[100]; 
	
	la=gps.latitude;
	lo=gps.longitude;
	/*if(la > 360.0 || lo > 360.0){
		DELAY_MS(1000);
	GSM_Msg_Send("error");
	la = gps.latitude;
	lo = gps.longitude;
		//return;
	}-----------------*/
	//sprintf((char *)outbuf,"Latitude:%.5f,Longitude:%.5f\r\n%.15s\r\n",la/=100000,lo/=100000,imei);		//得到经纬度字符串 
	//注意，附带的代码解算出的数据精确度有较大误差。有时候甚至偏离很远。
	if(gps.latitude/100000 > 360.0 || gps.longitude/100000 > 360.0)
	{
		GSM_Msg_Send("ERROR\0");
	}
	//sprintf((char *)outbuf,"{\"IMEI\":\"%.15s\",\"lng\":\"%.5f\",\"lat\":\"%.5f\"}\r\n",imei,lo/=100000,la/=100000);	
	//GSM_Msg_Send(outbuf);
	//sprintf((char *)outbuf,"lat:%.*s,lon:%.*s",gps.r_lat_len,gps.r_latitude,gps.r_lon_len,gps.r_longitude);
	//GSM_Send_Error +=
	/*sprintf((char *)outbuf,"{\"IMEI\":\"%.15s\",\"lng\":\"%.*s\",\"lat\":\"%.*s\"}\r\n",
	imei,
	gps.r_lon_len,gps.r_longitude,
	gps.r_lat_len,gps.r_latitude);*/
	sprintf((char *)outbuf,"{\"lng\":\"%.*s\",\"lat\":\"%.*s\"}\r\n",
	//imei,
	gps.r_lon_len,gps.r_longitude,
	gps.r_lat_len,gps.r_latitude);
	
	GSM_Send_Error +=GSM_Msg_Send(outbuf);
	//GSM_Msg_Send(gps.sourcedata);
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

void Reboot_GSM(void)
{
	GSM_Send_Error = 0;
	Power_Key_GPIO_Up;
	DELAY_MS(10000);
	Power_Key_GPIO_Down;
	while(test_boot==No)
	{
		if(GSM_Is_Boot())
			break ;
		Power_Key_GPIO_Up;
		DELAY_MS(10000);
		Power_Key_GPIO_Down;
		if(GSM_Is_Boot()){
			test_boot = Yes;
			//GSM_Send_Error = 0;
		}
	}
	Usart1_Send("AT+CGCLASS=\"B\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CGATT=1\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CIPCSGP=1,\"CMNET\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CLPORT=\"TCP\",\"2222\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CIPSTART=\"TCP\",\"120.25.218.92\",\"9919\"\r\n");//change to our own server.
	
	DELAY_MS(1000);
}

int main(void)
{	
	//*imei = (unsigned char*)molloc(sizeof(unsigned char)*100);
	//printf("ok~test");
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
		Power_Key_GPIO_Up;
		DELAY_MS(10000);
		Power_Key_GPIO_Down;
		if(GSM_Is_Boot()){
			test_boot = Yes;
		}
	}
	//USART_SendData(USART1, 0x1A);

	//测试SIM卡是否已经注册网络
	//while(test_net_register==No)

		Usart1_Send("AT+COPS?\r\n");
		DELAY_MS(2000);
		//est_net_register = Yes;

	Usart1_Send("AT+CGCLASS=\"B\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CGATT=1\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CIPCSGP=1,\"CMNET\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CLPORT=\"TCP\",\"2222\"\r\n");
	DELAY_MS(3000);
	
	Usart1_Send("AT+CIPSTART=\"TCP\",\"120.25.218.92\",\"9919\"\r\n");//change to our own server.
	
	DELAY_MS(1000);

	Get_IMEI(imei);//查询 IMEI 
	GSM_Msg_Send(imei);

	GPS_Ana_Error =0;
	GPS_Get_Error = 0;
	while(1)
	{
		if(GPS_Stop_flag==1)
		{
			int i;			
			for(i=0;i<256;i++)
				GPS_Data_Temp[i]=GPS_Uart2_Data[i];
			GPS_Stop_flag=0;
			GPS_Analysis(&gps,(u8*)GPS_Data_Temp);//分析字符串		
				
			GPS_Get_Error = 0;
		}
		if(gps.fixmode==3)	
		{
			GPS_Ana_Error = 0;
			GPS_Show();
			
		}
		if(GSM_Send_Error>3)
		{
			Reboot_GSM();
			
		}
		GPS_Ana_Error ++;
		GPS_Get_Error ++;
		if((GPS_Ana_Error > 20) || (GPS_Get_Error > 20))
		{
			char buf_t[100];
			sprintf(buf_t,"Get_Error:%d,Ana_Error:%d",GPS_Get_Error,GPS_Ana_Error);
			GSM_Msg_Send(buf_t);
		}
	}
}
