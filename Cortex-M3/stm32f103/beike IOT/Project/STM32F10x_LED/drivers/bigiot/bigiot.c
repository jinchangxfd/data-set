#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "LED.h"
#include "cJSON.h"
#include "millis.h"
#include "bigiot.h"

//Ӳ������
#include "sys.h"
#include "usart.h"
#include "delay.h"
//�����豸����
#include "esp8266.h"


//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DeviceNum   5

/*�����������������޸�Ϊ�Լ����豸ID��APIKEY���ڱ��������û���������豸���ɻ��*/
char *DEVICEID[] = {"8033","7134","7121","7122","7123"};
char *APIKEY[] = {"d0d37d1fa","bc09513f2","a59fef6bc","237446959","e63e7c592"};
char *INPUTID[] = {"7420","7416","7419","7417","7418"};

unsigned long lastCheckInTime = 0; 
unsigned long lastCheckStatusTime= 0; 
unsigned long lastSayTime[5]=  {0,0,0,0,0}; 
const unsigned long postingInterval =8000; //40000/5=8000
const unsigned long statusInterval = 20000; //100000/5=20000

char SendBuf[100];
/*���͵�¼��Ϣ*/
void checkin(unsigned char i)
{
	memset(SendBuf,0,sizeof(SendBuf));
	sprintf(SendBuf,"{\"M\":\"checkin\",\"ID\":\"%s\",\"K\":\"%s\"}\n",DEVICEID[i], APIKEY[i]);
	ESP8266_SendData(i,SendBuf,strlen(SendBuf));
}

/*�˳���¼*/
void checkout(unsigned char i)
{
	memset(SendBuf,0,sizeof(SendBuf));
	sprintf(SendBuf,"{\"M\":\"checkout\",\"ID\":\"%s\",\"K\":\"%s\"}\n",DEVICEID[i], APIKEY[i]);
	ESP8266_SendData(i,SendBuf,strlen(SendBuf));
}

/*��鵱ǰ��¼״̬*/
void check_status(unsigned char i)
{
	memset(SendBuf,0,sizeof(SendBuf));
	sprintf(SendBuf,"{\"M\":\"status\"}\n");
	ESP8266_SendData(i,SendBuf,strlen(SendBuf));
}
//while(strstr((const char *)esp8266_buf, "SEND OK") != NULL);		//����������ؼ���

/*����ָ�Ŀ�굥λ*/
void say(char *toID, char *content)
{
    UsartPrintf(USART_ESP8266,"{\"M\":\"say\",\"ID\":\"%s\",\"C\":\"%s\"}\n", toID, content);
}
/*�ϴ�һ���ӿڵ�ʵʱ����*/
void update1(char *did, char *inputid, float value) 
{
    UsartPrintf(USART_ESP8266,"{\"M\":\"update\",\"ID\":\"%s\",\"V\":{\"%s\":\"%.1f\"}}\n", did, inputid, value);
}

/*��CJSON������յ�����Ϣ*/
int processMessage(unsigned char id,char *msg) 
{
    cJSON *jsonObj = cJSON_Parse(msg);
    cJSON *method;
    char *m;
    //json�ַ�������ʧ�ܣ�ֱ���˳�
    if(!jsonObj)
    {
        //printf("json string wrong!");
        return 0;
    }
    method = cJSON_GetObjectItem(jsonObj, "M");
    m = method->valuestring;
	
//	printf("id=%d Cmd=%s\r\n",i,m);
	
    if(strncmp(m, "WELCOME", 7) == 0)
    {
        //��ֹ�豸����״̬δ�������ȵǳ�
        checkout(id);
        //��ֹ��������ָ�����
        delay_ms(500);
        checkin(id);
    }
    if(strncmp(m, "connected", 9) == 0)
    {
//        checkout(i);
//        delay_ms(500);
        checkin(id);
    }
    //���豸���û���¼�����ͻ�ӭ��Ϣ
    if(strncmp(m, "login", 5) == 0)
    {
//        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
//        char new_content[] = "Dear friend, welcome to BIGIOT !";
//        say(from_id, new_content);
    }
    //�յ�sayָ�ִ����Ӧ��������������Ӧ�ظ�
    if(strncmp(m, "say", 3) == 0 && millis() - lastSayTime[id] > 10)
    {
        char *content = cJSON_GetObjectItem(jsonObj, "C")->valuestring;
        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
        lastSayTime[id] = millis();
		
		printf("id=%d Say=%s\r\n",id,content);
        if(strncmp(content, "play", 4) == 0)
        {
            char new_content[] = "led played";
			switch(id)//�ж����ĸ�ID��������
			{
				case 0:
						Led_On(LED2);//���LED��
						say(from_id, new_content);
				break;
				case 1:            //do something here....
					
				break;
				case 2:            //do something here....
					
				break;
				case 3:            //do something here....
					
				break;
				case 4:            //do something here....
					
				break;
			}

			
			
        }
        else if(strncmp(content, "stop", 4) == 0)
        {
            char new_content[] = "led stoped";
			switch(id)//�ж����ĸ�ID��������
			{
				case 0:
						Led_Off(LED2);//���LED��
						say(from_id, new_content);
				break;
				case 1:            //do something here....
					
				break;
				case 2:            //do something here....
					
				break;
				case 3:            //do something here....
					
				break;
				case 4:            //do something here....
					
				break;
			}
			
			
        }
    }
    if(jsonObj)cJSON_Delete(jsonObj);
    return 1;
}


void BigIot_app(void)
{
	char *ptrIPD = NULL,*ptrIPD1 = NULL;
	unsigned char id=0;
	static unsigned char i=0,j=0;
	if(i<DeviceNum)//
	{
		if (millis() - lastCheckInTime > postingInterval || lastCheckInTime == 0) 
		{
			checkin(i);
			lastCheckInTime = millis();
			i++;
//			if(i>=DeviceNum)i=0;
		}
	}
	if (millis() - lastCheckStatusTime > statusInterval) 
	{
		check_status(j);
		lastCheckStatusTime= millis();
		j++;if(j>=DeviceNum)j=0;
	}
	

	if(ESP8266_WaitRecive() == REV_OK)								//����������
	{
		ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
		if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
		{
			//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
		}
		else
		{
			ptrIPD1 = ptrIPD+4;
			ptrIPD = strchr(ptrIPD1, ',');
			*ptrIPD='\0';
			id=atoi(ptrIPD1);
			ptrIPD+=1;
			ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
			if(ptrIPD != NULL)
			{
				ptrIPD++;
				ptrIPD1=strchr(ptrIPD, '}')+1;
				*ptrIPD1='\0';
//				printf("id=%d Data=%s\r\n",id,ptrIPD);
				processMessage(id,(char *)ptrIPD);
			}
		}
		ESP8266_Clear();
	}
	delay_ms(10);

}





