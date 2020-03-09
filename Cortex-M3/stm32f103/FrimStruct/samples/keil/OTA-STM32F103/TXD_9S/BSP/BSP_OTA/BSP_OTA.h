#ifndef __BSP_OTA_H
#define __BSP_OTA_H

#include "sys.h"
#include "BSP_flash.h"
#include "BSP_24l01.h"

#define BSP_FLASHWrite_Addr 0x08003C00  // Ԥ��20KB bootloader
#define Single_DataSize		28
#define Single_SendSize  7*1024		// ���η�����󳤶�


extern u8 NRF_Sign; 	// NRFģʽ��ʶλ��0���� or 1����
extern u16 USART_RX_CNT;
extern u16 AppLenth;
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

void check_Firmware(void);				// ���̼��Ƿ�������
void Group_SendFirmware(void);		// Ⱥ����¹̼�
void Group_UpdateID(u8 tmp);  		// ���½��ն�ID	
void Count_SendNum(void);	// ���㷢�ʹ���,����RX�˽�����SIZE���
u8 Send_Firmware(void);		// ���͹̼�


void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);		// APPд��FLASH
void iap_load_app(u32 appxaddr);		// ��ת��Ӧ�ó����

void OTA_RxMode(void);		// ��ʼ��NRFΪ���ն�
void OTA_TxMode(void);		// ��ʼ��NRFΪ���Ͷ�

#endif





