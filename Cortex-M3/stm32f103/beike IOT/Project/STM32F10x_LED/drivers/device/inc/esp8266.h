#ifndef _ESP8266_H_
#define _ESP8266_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

extern unsigned char esp8266_buf[512];
extern unsigned short esp8266_cnt, esp8266_cntPre;

void ESP8266_Init(u8 mode);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char id,char *data, unsigned short len);

_Bool ESP8266_WaitRecive(void);

#endif


