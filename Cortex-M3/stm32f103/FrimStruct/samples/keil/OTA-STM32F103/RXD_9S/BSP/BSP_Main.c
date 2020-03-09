#include "BSP_Main.h"

u8 RxBuf;
u8 time = 0;

/**
  * @brief  BSP_Main
  * @param  void
  * @retval None	  
  */
void BSP_Main(void)
{	 
		u16 i = 0;
	
		printf("OTA bootloader V1.3.1\r\n");

		check_RunStatu();				// ���Bootloader ���б�־λ
		BSP_SYSInit();					// ��ȡID
		OTA_RxMode();						// ��ʼ��Ϊ����ģʽ
	
		HAL_TIM_Base_Start_IT(&htim3);		// ������ʱ���ж�
	
		while(1)
		{
			HAL_Delay(1);
			
			if (NRF24L01_RxLenth()) 	// ���չ̼�����
			{	
					Count_ReceiveNum();		// ������մ���	
					Receive_Firmware();		// ���չ̼�
			}	
			if (i++ >= 500) {i = 0;HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);}  // ����ָʾ��
		}	
}

///**
//  * @brief  UART�жϷ�����
//  * @param  huart: ����
//  * @retval None	  
//  */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{ 
//	 if(huart->Instance == USART1) 
//	 { 
//			OTA_RX[USART_RX_CNT++] = RxBuf;
//   }
//}

/**
  * @brief  TIM�жϷ�����
  * @param  htim����ʱ��
  * @retval None	  
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	time++;
	printf("\r\n");
	
	if (time >= 5)	
	{
		HAL_TIM_Base_Stop_IT(&htim3);
		check_RunStatu();
		time = 0;
	}
}







