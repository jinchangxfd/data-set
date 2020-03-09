
//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "key.h"


//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

char *ESP8266_WIFI_INFO="AT+CWJAP=\"214\",\"3.1415926\"\r\n";

unsigned char esp8266_buf[512];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res,u16 timeOut)
{
	
	Usart_SendString(USART3, cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//				id:链接号
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char id,char *data, unsigned short len)
{

	char cmdBuf[100];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%u,%d\r\n",id,len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">",100))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART3, data, len);		//发送设备连接请求数据
	}

}



char string[40];
char* ESP8266_USART_Link(char *msg)
{
    cJSON *jsonObj = cJSON_Parse(msg);
    cJSON *method;
    char *wifi_ssid,*wifi_psd;
	memset(string,0,sizeof(string));
    //json字符串解析失败，直接退出
    if(!jsonObj)
    {
        printf("json string wrong!");
        return NULL;
    }
    method = cJSON_GetObjectItem(jsonObj, "SSID");
    wifi_ssid = method->valuestring;
	if(!wifi_ssid)
	{
        printf("json string wrong!");
        return NULL;
	}
    method = cJSON_GetObjectItem(jsonObj, "PSD");
    wifi_psd = method->valuestring;
	if(!wifi_psd)
	{
        printf("json string wrong!");
        return NULL;
	}
	printf("收到WiFi信息\r\nSSID:%s\r\nPSD:%s\r\n",wifi_ssid,wifi_psd);
	sprintf(string,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_ssid,wifi_psd);
	if(jsonObj)cJSON_Delete(jsonObj);	
	return string;
}



//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(u8 mode)
{
	char *ptrIPD = NULL;
	u8 time=15;
LinkWifi:
	ESP8266_RST=0;
	delay_ms(250);
	ESP8266_RST=1;
	delay_ms(500);
	
	ESP8266_Clear();
	if(mode==0)
	{
//{"M":"WELCOME TO BIGIOT"}
		if(ESP8266_SendCmd("", "GOT IP",2000)==0)//优化初始化过程，选择性初始化
		{
			UsartPrintf(USART_DEBUG,"    跳过WiFi连接初始化\r\n");
			while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK",250))
			delay_ms(500);
			goto LinkServer;
		}
	}

	UsartPrintf(USART_DEBUG, "开始WiFi连接初始化\r\n");
	
	UsartPrintf(USART_ESP8266, "+++");//退出透传
	delay_ms(500);
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK",250))
	delay_ms(500);

	UsartPrintf(USART_DEBUG, "AT+RESTORE\r\n");
	while(ESP8266_SendCmd("AT+RESTORE\r\n", "OK",250))
	delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "AT+CWMODE=1\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK",250))
	delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "AT+CIPMUX=1\r\n");
	while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK",250))
	delay_ms(500);
	
	if(mode==0)
	{
		UsartPrintf(USART_DEBUG, "CWJAP\r\n");
		while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP",250))
		{
			time++;
			if(time>=20)
			{
				time=0;
				UsartPrintf(USART_DEBUG,"\n WIFI连接出了点错误哟...\r\n");
				UsartPrintf(USART_DEBUG,"请检查WiFi网络热点是否开启或进行重新配网\r\n");
				UsartPrintf(USART_DEBUG,"配网方法一:串口配网正确格式:{\"SSID\":\"WiFi名字\",\"PSD\":\"WiFi密码\"}\r\n");
				UsartPrintf(USART_DEBUG,"配网方法二:按智能配网按键使用手机APP配网\r\n");
			}	

			if((USART_RX_STA&0x8000)!=0)//串口配网
			{
	//AT+CWJAP="214","3.1415926"\r\n	
				ptrIPD=ESP8266_USART_Link((char *)USART_RX_BUF);
				if(ptrIPD!=NULL)
				{
					ESP8266_WIFI_INFO=ptrIPD;
					printf("\r\n%s\r\n",ESP8266_WIFI_INFO);
				}
				USART_RX_STA=0;
				memset(USART_RX_BUF,NULL,sizeof(USART_RX_BUF));
			}
	
		}
	}else if(mode==1)
	{
		UsartPrintf(USART_DEBUG, "AT+CWSMARTSTART=1\r\n");
		while(ESP8266_SendCmd("AT+CWSMARTSTART=1\r\n", "OK",250))
		UsartPrintf(USART_DEBUG, "进入智能配网模式,请使用手机APP配置WiFi网络\r\n");
		if(ESP8266_SendCmd("", "GOT IP",6000)==0)UsartPrintf(USART_DEBUG, "智能配网成功\r\n");
		else
		{
			UsartPrintf(USART_DEBUG, "智能配网失败,请重试！！\r\n");
			goto LinkWifi;
		}
	}
LinkServer:	

//	SeverIP   LinkMode
	while(ESP8266_SendCmd("AT\r\n", "OK",250))
	delay_ms(500);	
	UsartPrintf(USART_DEBUG, "Client0\r\n");
	while(ESP8266_SendCmd("AT+CIPSTART=0,\"TCP\",\"121.42.180.30\",8282\r\n", "OK",250))
	delay_ms(500);		
	UsartPrintf(USART_DEBUG, "Client1\r\n");
	while(ESP8266_SendCmd("AT+CIPSTART=1,\"TCP\",\"121.42.180.30\",8282\r\n", "OK",250))
	delay_ms(500);	
	UsartPrintf(USART_DEBUG, "Client2\r\n");
	while(ESP8266_SendCmd("AT+CIPSTART=2,\"TCP\",\"121.42.180.30\",8282\r\n", "OK",250))
	delay_ms(500);	
	UsartPrintf(USART_DEBUG, "Client3\r\n");
	while(ESP8266_SendCmd("AT+CIPSTART=3,\"TCP\",\"121.42.180.30\",8282\r\n", "OK",250))
	delay_ms(500);	
	UsartPrintf(USART_DEBUG, "Client4\r\n");
	while(ESP8266_SendCmd("AT+CIPSTART=4,\"TCP\",\"121.42.180.30\",8282\r\n", "OK",250))
	delay_ms(500);	

	UsartPrintf(USART_DEBUG, "ESP8266 Init OK\r\n");
	ESP8266_Clear();
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

}

