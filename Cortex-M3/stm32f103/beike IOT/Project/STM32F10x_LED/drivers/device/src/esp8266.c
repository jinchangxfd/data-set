
//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "key.h"


//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

char *ESP8266_WIFI_INFO="AT+CWJAP=\"214\",\"3.1415926\"\r\n";

unsigned char esp8266_buf[512];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res,u16 timeOut)
{
	
	Usart_SendString(USART3, cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//				id:���Ӻ�
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char id,char *data, unsigned short len)
{

	char cmdBuf[100];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%u,%d\r\n",id,len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">",100))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART3, data, len);		//�����豸������������
	}

}



char string[40];
char* ESP8266_USART_Link(char *msg)
{
    cJSON *jsonObj = cJSON_Parse(msg);
    cJSON *method;
    char *wifi_ssid,*wifi_psd;
	memset(string,0,sizeof(string));
    //json�ַ�������ʧ�ܣ�ֱ���˳�
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
	printf("�յ�WiFi��Ϣ\r\nSSID:%s\r\nPSD:%s\r\n",wifi_ssid,wifi_psd);
	sprintf(string,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_ssid,wifi_psd);
	if(jsonObj)cJSON_Delete(jsonObj);	
	return string;
}



//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
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
		if(ESP8266_SendCmd("", "GOT IP",2000)==0)//�Ż���ʼ�����̣�ѡ���Գ�ʼ��
		{
			UsartPrintf(USART_DEBUG,"    ����WiFi���ӳ�ʼ��\r\n");
			while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK",250))
			delay_ms(500);
			goto LinkServer;
		}
	}

	UsartPrintf(USART_DEBUG, "��ʼWiFi���ӳ�ʼ��\r\n");
	
	UsartPrintf(USART_ESP8266, "+++");//�˳�͸��
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
				UsartPrintf(USART_DEBUG,"\n WIFI���ӳ��˵����Ӵ...\r\n");
				UsartPrintf(USART_DEBUG,"����WiFi�����ȵ��Ƿ����������������\r\n");
				UsartPrintf(USART_DEBUG,"��������һ:����������ȷ��ʽ:{\"SSID\":\"WiFi����\",\"PSD\":\"WiFi����\"}\r\n");
				UsartPrintf(USART_DEBUG,"����������:��������������ʹ���ֻ�APP����\r\n");
			}	

			if((USART_RX_STA&0x8000)!=0)//��������
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
		UsartPrintf(USART_DEBUG, "������������ģʽ,��ʹ���ֻ�APP����WiFi����\r\n");
		if(ESP8266_SendCmd("", "GOT IP",6000)==0)UsartPrintf(USART_DEBUG, "���������ɹ�\r\n");
		else
		{
			UsartPrintf(USART_DEBUG, "��������ʧ��,�����ԣ���\r\n");
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
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

}

