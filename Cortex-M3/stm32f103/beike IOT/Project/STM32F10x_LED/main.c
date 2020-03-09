/* Includes ------------------------------------------------------------------*/
//Ӳ������
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "ds18b20.h"
#include "LED.h"
#include "key.h"
//�����豸����
#include "esp8266.h"
#include "millis.h"
#include "bigiot.h"

/*********************************************************************************
����������ESP8266�Ķ����ӹ��ܣ����֧��5·���ӣ�ʵ��5���豸ͬʱ���߱�������ƽ̨
����QQ��466554770
���ڱ���ѧʶ��ǳ�����ִ��뻹�д����ƣ�������Ҫ���ɼ�QQ��ͬ̽��
**********************************************************************************/  
//��λ����������ͬʱ���¿ɽ�����������ģʽ����Ҫʹ���ֻ�APP��������ʹ�����ε��������������

void HardWare_Init(void)
{
	delay_init();
	uart_init(115200);
	Usart3_Init(115200);	
    LED_Init();
	KEY_Init();
    MILLIS_Init();
	if(KEY_Scan(1)==KEY0_Press) 	ESP8266_Init(1);//��������
	else							ESP8266_Init(0);//��ʼ������
	DS18B20_Init();
	DS18B20_Get_Temp();
	UsartPrintf(USART_DEBUG,"HardWare_Init Succeed\r\n");
}

int main(void)
{
    u16 i = 499;
//	short temperature;
    HardWare_Init();
    while (1)
    {
		BigIot_app();
		i++;
		if(i%50==0) //50*10ms=500ms
		{
			LED=!LED;//��˸��ʾ������������
		}
//		if(i>=500)	//�¶��ϴ�����
//		{
//			i=0;
//			temperature=DS18B20_Get_Temp();		
//			if(temperature<0)	temperature=-temperature;	
//			update1(DEVICEID[0],INPUTID[0],(float)(temperature/10.0));
//			UsartPrintf(USART_DEBUG,"�¶�=%.1f\r\n",(float)(temperature/10.0));
//		}
		if(KEY_Scan(0)==KEY0_Press)Led_Toggle(2);//�ֶ�����
    }
}






