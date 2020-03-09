#include "BSP_Main.h"

struct OTA_Run ota;

/**
  * @brief  ������
  * @param  void
  * @retval None	  
  */
void BSP_Main(void)
{
		u8 i;
		
		my_mem_init(SRAMIN);
	  ota.Run_Statu = 0x00;  
		BSP_SYSInit();
		OTA_TxMode();
		
		while(1)
		{
			if (i++ >= 100)	{i = 0;check_Firmware();}			 // ���̼��Ƿ�������
			
			switch (ota.Run_Statu)
			{
				case 0x00:HAL_UART_Receive_IT(&huart1,(uint8_t*)&ota.RxBuf,1);break;		// �ȴ�ִ��ָ��
				case 0xff:HAL_UART_Receive_IT(&huart1,(uint8_t*)&ota.RxBuf,1);break; 		// ���չ̼�
				case 0xfe:GET_IDHandle();	ota.Run_Statu = 0x00;break;										// ��ȡ�ӻ�ID
				case 0xfd:Group_SendFirmware();ota.Run_Statu = 0x00;break;							// �����̼�	
				default :printf("����ָ���������������!\r\n");ota.Run_Statu = 0x00;break;
			}	
		}
		
}

/**
  * @brief  UART�жϷ����� 
  * @param  huart: ����
  * @retval None	  
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
		switch (ota.Run_Statu)
		{
			case 0xff:OTA_RX[USART_RX_CNT++] = ota.RxBuf;break;		// �ȴ�ִ��ָ��
			case 0x00:ota.Run_Statu = ota.RxBuf;
								if(ota.RxBuf == 0xff) printf("�뷢�͹̼�\r\n");
								break; 						// ���չ̼�
			default :ota.Run_Statu = 0x00;break;
		}	
		if (ota.Run_Statu == 0xff)	
		if (ota.Run_Statu == 0x00)	ota.Run_Statu = ota.RxBuf;
}

/**
  * @brief  ���ڻ�ȡ�ӻ�ID
  * @param  void
  * @retval None	  
  */
void GET_IDHandle(void)
{
		u8 i,x;
		
		printf("������ID������ID���룺\r\n");
		HAL_UART_Receive(&huart1,(uint8_t*)&ota.ID_Num,1,0xffff);		// ��ȡID����
		HAL_UART_Receive(&huart1,(uint8_t*)&ota.ID_Buf,ota.ID_Num * 5,0xffff);		// ��ȡID
				
		for(x = 0; x < ota.ID_Num;x++)
		{
			printf("\r\n��%d��IDΪ��",x+1);
			for(i = 0; i < 5; i++) 
				printf("%x ",ota.ID_Buf[x*5+i]);
		}
		printf("����ID���!\r\n");
}

























