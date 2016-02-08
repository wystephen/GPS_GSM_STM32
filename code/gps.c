#include "gps.h" 
#include "delay.h" 								   								   
#include "string.h"	 
//RTrobot.org
//禁止用于商业行为

/********************************************************************************************
GPS数据解析
********************************************************************************************/
void GPS_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	if((strstr((const char *)buf,"$GPGSV")))//GPGSV解析
		GPGSV_Analysis(gps_data,buf);

	if((strstr((const char *)buf,"$GPGGA")))//GPGGA解析
		GPGGA_Analysis(gps_data,buf);

	if((strstr((const char *)buf,"$GPRMC")))//GPRMC解析
		GPRMC_Analysis(gps_data,buf);
	if((strstr((const char *)buf,"$GPGSA")))//GPGSA解析	
		GPGSA_Analysis(gps_data,buf);
	if((strstr((const char *)buf,"$GPGLL")))
		GPGLL_Analysis(gps_data,buf);
}

/********************************************************************************************
解析GPGSV数据
********************************************************************************************/
void GPGSV_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	unsigned char *temp_buf,*address_buf,end_buf,decimal_places;  	 
	temp_buf=buf;
	address_buf=(unsigned char*)strstr((const char *)temp_buf,"$GPGSV");
	end_buf=Data_Removal(address_buf,3);									 										//数据去除
	if(end_buf!=0XFF)
		gps_data->svnum=Data_Extraction(address_buf+end_buf,&decimal_places);		//得到已知卫星总数
}

/********************************************************************************************
解析GPGGA数据
********************************************************************************************/
void GPGGA_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	unsigned char *address_buf,end_buf,decimal_places;	
  address_buf =(unsigned char*)strstr((const char *)buf,"$GPGGA");		
	
  end_buf=Data_Removal(address_buf,6);																			//数据去除				
  if(end_buf!=0XFF)
		gps_data->gpssta=Data_Extraction(address_buf+end_buf,&decimal_places);	//得到GPS状态
	
	end_buf=Data_Removal(address_buf,7);																			//数据去除		
	if(end_buf!=0XFF)
		gps_data->posslnum=Data_Extraction(address_buf+end_buf,&decimal_places);//得到用于定位的卫星数
}

/********************************************************************************************
解析GPRMC数据
********************************************************************************************/
void GPRMC_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	unsigned char *add,*address_buf,end_buf,decimal_places;		 
	unsigned long int temp;	  
	unsigned int seg_time=0;
	unsigned int seg_num[12];
	int i=0;
	int j=0;
	float rs;  
	address_buf =(unsigned char*)strstr((const char *)buf,"GPRMC");

	//插入一段 直接 字符串提取试试~
	add=address_buf;
	
	for(i = 0;seg_time<6&&i<70;i++){
		if(*(add+i) == ','){
			seg_num[seg_time] = i;
			seg_time++;	
		}
		if(seg_time == 4){
			for(j=0;j<(seg_num[seg_time-1]-seg_num[seg_time-2]-1);j++)
			{
				gps_data->r_latitude[j] = *(add +(seg_num[seg_time-2]+j+1));
				if(gps_data->r_latitude[j] == '.'&& j>1)//这里j>1有待商榷
				{
					gps_data->r_latitude[j] = gps_data->r_latitude[j-1];
					gps_data->r_latitude[j-1] = gps_data->r_latitude[j-2];
					gps_data->r_latitude[j-2] = '.';
					
				}
			}
			gps_data->r_lat_len=j+1;
		}
		if(seg_time == 6)
		{
			for(j=0;j<(seg_num[seg_time-1]-seg_num[seg_time-2]-1);j++)
			{
				gps_data->r_longitude[j] = *(add +(seg_num[seg_time-2]+j+1));
				if(gps_data->r_longitude[j] == '.'&& j>1)
				{
					gps_data->r_longitude[j] = gps_data->r_longitude[j-1];
					gps_data->r_longitude[j-1] = gps_data->r_longitude[j-2];
					gps_data->r_longitude[j-2] = '.';
				}
			}
			gps_data->r_lon_len = j+1;
		}
		gps_data->sourcedata[i] = *(add+i);
	}
	gps_data->sourcedata[99] = '\0';
	
	
	
	
	//到这里
	end_buf=Data_Removal(address_buf,3);																			//数据去除								
	if(end_buf!=0XFF)
	{
		temp=Data_Extraction(address_buf+end_buf,&decimal_places);		 	 
		gps_data->latitude=temp/MN_Process(10,decimal_places+2);
		rs=temp%MN_Process(10,decimal_places+2);
		gps_data->latitude=gps_data->latitude*MN_Process(10,5)+(rs*MN_Process(10,5-decimal_places))/60;//得到纬度
	}
	
	
	end_buf=Data_Removal(address_buf,4);																			//数据去除								
	if(end_buf!=0XFF)
		gps_data->nshemi=*(address_buf+end_buf);																//南纬还是北纬 			

	
 	end_buf=Data_Removal(address_buf,5);																			//数据去除									
	if(end_buf!=0XFF)
	{												  
		temp=Data_Extraction(address_buf+end_buf,&decimal_places);		 	 
		gps_data->longitude=temp/MN_Process(10,decimal_places+2);
		rs=temp%MN_Process(10,decimal_places+2); 
		gps_data->longitude=gps_data->longitude*MN_Process(10,5)+(rs*MN_Process(10,5-decimal_places))/60;//得到经度
	}
	
	end_buf=Data_Removal(address_buf,6);																			//数据去除
	if(end_buf!=0XFF)
		gps_data->ewhemi=*(address_buf+end_buf);																//东经还是西经		 
}  
void GPGLL_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	unsigned char *address_buf,end_buf,decimal_places;
	unsigned long int temp;
	float rs;
	address_buf = (unsigned char *)strstr((const char *)buf,"GPGLL");
	
	
}

/********************************************************************************************
解析GPGSA数据
********************************************************************************************/
void GPGSA_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
	unsigned char *address_buf,end_buf,decimal_places;			   
	address_buf=(unsigned char*)strstr((const char *)buf,"$GPGSA");
	end_buf=Data_Removal(address_buf,2);
	
	if(end_buf!=0XFF)
		gps_data->fixmode=Data_Extraction(address_buf+end_buf,&decimal_places);	//得到定位类型
}

/********************************************************************************************
数据去除
********************************************************************************************/
unsigned char Data_Removal(unsigned char *buf,unsigned char address_buf)
{	 		    
	unsigned char *temp_buf=buf;
	while(address_buf)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')
			return 0XFF;
		if(*buf==',')address_buf--;
			buf++;
	}
	return buf-temp_buf;	 
}

/********************************************************************************************
m^n函数
********************************************************************************************/
unsigned long int MN_Process(unsigned char m,unsigned char n)
{
	unsigned long int result=1;	 
	while(n--)
		result*=m;    
	return result;
}

/********************************************************************************************
提取需要的数据
********************************************************************************************/
int Data_Extraction(unsigned char *buf,unsigned char*decimal_places)
{
	unsigned char *temp_buf=buf;
	unsigned long int intbuf=0,fres=0;
	unsigned char address_int=0,address_decimal_places=0,i,mask=0;
	int res;
	while(1)
	{
		if(*temp_buf=='-')
		{
			mask|=0X02;
			temp_buf++;
		}
		
		if(*temp_buf==','||(*temp_buf=='*'))
			break;
		
		if(*temp_buf=='.')
		{
			mask|=0X01;
			temp_buf++;
		}
		
		else if(*temp_buf>'9'||(*temp_buf<'0'))
		{	
			address_int=0;
			address_decimal_places=0;
			break;
		}
		
		if(mask&0X01)
			address_decimal_places++;
		else 
			address_int++;
		
		temp_buf++;
	}
	
	if(mask&0X02)
		buf++;
	for(i=0;i<address_int;i++)
	{  
		intbuf+=MN_Process(10,address_int-1-i)*(buf[i]-'0');
	}
	
	if(address_decimal_places>5)
		address_decimal_places=5;
	
	*decimal_places=address_decimal_places;	 		
	
	for(i=0;i<address_decimal_places;i++)
	{  
		fres+=MN_Process(10,address_decimal_places-1-i)*(buf[address_int+1+i]-'0');
	} 
	
	res=intbuf*MN_Process(10,address_decimal_places)+fres;
	
	if(mask&0X02)
		res=-res;		   
	
	return res;
}	 

