/* Includes ------------------------------------------------------------------*/
//硬件驱动
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "ds18b20.h"
#include "LED.h"
#include "key.h"
//网络设备驱动
#include "esp8266.h"
#include "millis.h"
#include "bigiot.h"

/*********************************************************************************
本程序利用ESP8266的多链接功能（最多支持5路链接）实现5个设备同时上线贝壳物联平台
作者QQ：466554770
由于本人学识尚浅，部分代码还有待完善，若有需要，可加QQ共同探讨
**********************************************************************************/  
//复位和配网按键同时按下可进入无线配网模式，需要使用手机APP配网（可使用乐鑫的无线配网软件）

void HardWare_Init(void)
{
	delay_init();
	uart_init(115200);
	Usart3_Init(115200);	
    LED_Init();
	KEY_Init();
    MILLIS_Init();
	if(KEY_Scan(1)==KEY0_Press) 	ESP8266_Init(1);//无线配网
	else							ESP8266_Init(0);//初始化配网
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
			LED=!LED;//闪烁提示程序正在运行
		}
//		if(i>=500)	//温度上传功能
//		{
//			i=0;
//			temperature=DS18B20_Get_Temp();		
//			if(temperature<0)	temperature=-temperature;	
//			update1(DEVICEID[0],INPUTID[0],(float)(temperature/10.0));
//			UsartPrintf(USART_DEBUG,"温度=%.1f\r\n",(float)(temperature/10.0));
//		}
		if(KEY_Scan(0)==KEY0_Press)Led_Toggle(2);//手动开关
    }
}






