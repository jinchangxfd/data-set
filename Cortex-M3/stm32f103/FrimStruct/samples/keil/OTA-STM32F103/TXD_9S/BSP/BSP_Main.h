#ifndef __BSP_MAIN_H
#define __BSP_MAIN_H

#include "BSP_OTA.h"
#include "BSP_24l01.h"

#include "BSP_flash.h"
#include "BSP_SYS.H"
#include "BSP_Malloc.h"

#include "usart.h"
extern 		u8 Rx[20];

struct OTA_Run
{
	u8 Run_Statu ;	// ff ���չ̼�  aa ����ID  bb�����̼�  00 ������
	u8 RxBuf;				// �̼����ջ���
	u8 ID_Num;			// 
	u8 ID_Buf[50];
};

void BSP_Main(void);
void GET_IDHandle(void);

#endif



