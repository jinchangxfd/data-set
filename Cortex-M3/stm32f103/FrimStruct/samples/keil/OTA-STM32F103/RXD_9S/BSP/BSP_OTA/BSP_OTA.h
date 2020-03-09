#ifndef __BSP_OTA_H
#define __BSP_OTA_H

#include "sys.h"
#include "BSP_24l01.h"
#include "BSP_flash.h"

extern u8 NRF_Sign; 	// NRFģʽ��ʶλ��0���� or 1����
extern u16 USART_RX_CNT;
extern u16 AppLenth;
extern u16 OTAFLAG;

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.


u8 Update_Firmware(void);
void Count_ReceiveNum(void);
u8 Receive_Firmware(void);
void check_RunStatu(void);
void Change_OTAFlag(u16 tmp);

void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);
void iap_load_app(u32 appxaddr);

void OTA_RxMode(void);


#endif

