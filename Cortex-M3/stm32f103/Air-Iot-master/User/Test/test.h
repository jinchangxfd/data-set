#ifndef  __TEST_H
#define     __TEST_H

#include "stm32f10x.h"

/** TODO �޸�WIFI������Ϣ */

/********************************** �û���Ҫ���õĲ��� **********************************/
// Ҫ���ӵ��ȵ�����ƣ���WIFI����
#define      macUser_ESP8266_ApSsid           "ten"

// Ҫ���ӵ��ȵ����Կ
#define      macUser_ESP8266_ApPwd            "wangshihao"

// Ҫ���ӵķ�������IP�������Ե�IP��Windows��ʹ��ipconfig����鿴
#define      macUser_ESP8266_TcpServer_IP     "192.168.43.74"

// Ҫ���ӵķ������Ķ˿�
#define      macUser_ESP8266_TcpServer_Port    "2759"


/********************************** �ⲿȫ�ֱ��� ***************************************/
extern volatile uint8_t ucTcpClosedFlag;


/********************************** ���Ժ������� ***************************************/
void ESP8266_StaTcpClient_UnvarnishTest(void);


#endif

