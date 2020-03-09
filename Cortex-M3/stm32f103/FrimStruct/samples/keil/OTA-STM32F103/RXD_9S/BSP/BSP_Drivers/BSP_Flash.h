#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "main.h"
#include "stm32F1xx_hal.h"

#define STM32_FLASH_SIZE 64		//��ѡSTM32��FLASH������С(��λΪK)

#define STM32_FLASH_BASE 0x08000000			//FLASH��ʼ��ַ
#define OTA_Sign				 0x08003800			// OTA����־λ

void BSP_FLASHWrite(uint32_t WriteAddr,uint32_t *pBuffer,uint16_t NumToWrite);
void BSP_FLASHWrite_NoCheck(uint32_t WriteAddr,uint32_t *pBuffer,uint16_t NumToWrite);
void BSP_FLASHWriteWord(uint32_t WriteAddr,uint16_t pBuffer);

void BSP_FLASHRead(uint32_t ReadAddr,uint32_t *pBuffer,uint16_t NumToRead);
void BSP_ReadByte(uint32_t ReadAddr,uint8_t *pBuffer);
uint32_t BSP_FLASHReadWord(uint32_t faddr);

void Read_Firmware(void);// ����ʹ�ã���ȡ����̼�	
	
#endif

