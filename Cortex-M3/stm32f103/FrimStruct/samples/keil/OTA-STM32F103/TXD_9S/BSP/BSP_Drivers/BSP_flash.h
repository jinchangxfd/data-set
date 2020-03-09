#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "main.h"
#include "stm32f1xx_hal.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64
//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1
//ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000
//STM32 FLASH����ʼ��ַ
//FLASH������ֵ

extern uint32_t BSP_FLASHWrite_Addr;

uint32_t BSP_FLASHGetWriteAddr(uint8_t Len);
void BSP_FLASHRead(uint32_t ReadAddr,uint32_t *pBuffer,uint16_t NumToRead);
void BSP_FLASHWrite(uint32_t WriteAddr,uint32_t *pBuffer,uint16_t NumToWrite);
	
#endif

