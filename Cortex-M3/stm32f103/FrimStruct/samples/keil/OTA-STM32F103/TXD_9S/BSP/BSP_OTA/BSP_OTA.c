#include "BSP_OTA.h"
#include "BSP_Main.h"


#define SendNum_len			10		

u16 sendNum[SendNum_len];
u32 iapbuf[512];  
iapfun jump2app; 
u16 AppLenth,oldCount,USART_RX_CNT = 0;
u8 seng_num[3];
extern 	struct OTA_Run ota;

/**
  * @brief  check_Firmware
	* @note   ���̼��Ƿ�������
	* @param  void 
  * @retval NONE
  */
void check_Firmware(void)
{
		if(USART_RX_CNT)
		{
			if(oldCount == USART_RX_CNT)	//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				AppLenth = USART_RX_CNT;
				oldCount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",AppLenth);
				
				seng_num[0] = AppLenth%256;
				seng_num[1] = AppLenth/256;	
				Count_SendNum();		// ���㷢�ʹ���	
				ota.Run_Statu = 0x00;	
			}
			else 
				oldCount=USART_RX_CNT;			
		}	
}

/**
  * @brief  Group_SendFirmware
	* @note   Ⱥ����¹̼�
  * @param  void
  * @retval NONE
  */
void Group_SendFirmware(void)
{
		u8 i = 0,y = 0;
	
		printf("Group_SendFirmware\r\n");
	
		if (!seng_num[0] && !seng_num[1]) printf("û�й̼����봫��̼���\r\n");
		else 
		{
			while( y < ota.ID_Num )		// Ⱥ����¹̼�
			{
				Group_UpdateID(y++);		// ���½��ն�ID
				
				i = 0;
				while	(NRF24L01_TxLenth(seng_num) && i< 10) // ���͹̼����ȣ������RX���Ƿ���ڣ�1s�ڲ����ڣ����Զ��˳��˱�����	
				{
					HAL_Delay(100);
					printf("��%d�η��ͳ���ʧ�ܣ�\r\n",++i); 
				}  	
				
				if (i < 10)	Send_Firmware();		// ���͹̼�
				else {printf("ID:%x%x%x%x%x  ����ʧ�ܣ���鲻�����նˣ�\r\n",RX_ADDRESS[0],RX_ADDRESS[1],RX_ADDRESS[2],RX_ADDRESS[3],RX_ADDRESS[4]);}	
			}
		}
		//seng_num[0] = seng_num[1] = 0;
}

/**
  * @brief  Group_UpdateID
	* @note   ���½��ն�ID
  * @param  tmp��ID��
  * @retval None	   
  */
void Group_UpdateID(u8 tmp)
{
	u8 i;
	for(i = 0; i < 5; i++)
	{
		RX_ADDRESS[i] = TX_ADDRESS[i] = ota.ID_Buf[i + tmp*5];	// ��ֵ��ַ
	}
	
	NRF24L01_TX_Mode();	// ��ʼ����ַ
}

/**
  * @brief  Send_Firmware
	* @note   ���͹̼�
  * @param  void
  * @retval 1,���ͳɹ� or 0XFF	RXDû��Ӧ��,�������η���
  */
u8 Send_Firmware(void)
{
	u8 z;
	u16 y,x,index;
	x = y = z = 0;
	
	HAL_Delay(3);
	
	while(sendNum[z])
	{	 
		for ( y = 0; y < sendNum[z]; y++ )
		{
			HAL_Delay(4);
			index = z*sendNum[0] + y;
			
			while( NRF24L01_Tx( index ) && x < 1000)	// ���͹̼�,1s��û���յ�Ӧ��λ��������
			{
				HAL_Delay(2);
				printf("�����ж�  %d\r\n",index); 
				x++;
				if (x >= 950) { printf("ID��%d%d%d%d%d �����ж�\r\n",RX_ADDRESS[0],RX_ADDRESS[1],RX_ADDRESS[2],RX_ADDRESS[3],RX_ADDRESS[4]); return 0xFF; }		// ��ʱ���
			}
			//HAL_Delay(2);
			x = 0;
	  }
		
		HAL_Delay(840);  // �ȴ�д��flash
		++z;
	}
	
	printf("���ͽ���");
	
	return 1;
}

/**
  * @brief  Count_SendNum
	* @note   ���㷢�ʹ���,����RX�˽�����SIZE���
  * @param  size:�����������
  * @retval None	  
  */
void Count_SendNum(void)
{
		u8 i = 0;
	
		while (AppLenth)
		{
			if (AppLenth>=Single_SendSize)	
			{
				sendNum[i] = Single_SendSize/Single_DataSize;	// ����NRF���ʹ���
				AppLenth -= Single_SendSize;
			}				
			else 
			{	
				if (AppLenth%Single_DataSize != 0) sendNum[i] = AppLenth/Single_DataSize + 1;		// ����NRF���ʹ���
				else  sendNum[i] = AppLenth/Single_DataSize;
				AppLenth = 0;
			}
			printf("��%d�ν��մ���Ϊ%d\r\n",i+1,sendNum[i]);
			i++;		
		}
		printf("\r\n");
}

/**
  * @brief  OTA_RxMode
	* @note   ��ʼ��NRFΪ���ն�
  * @param  void
  * @retval None	  
  */
void OTA_RxMode(void)
{
		NRF24L01_Init();
		NRF24L01_RX_Mode();
		printf("NRF24L01 Mode: Receive\r\n");	
}

/**
  * @brief  OTA_TxMode
	* @note   ��ʼ��NRFΪ���Ͷ�
  * @param  void
  * @retval None	  
  */
void OTA_TxMode(void)
{
		NRF24L01_Init();
		NRF24L01_TX_Mode();
		printf("NRF24L01 Mode: Sends\r\n");	
}

/**
  * @brief  iap_write_appbin
	* @note   APPд��FLASH
  * @param  appxaddr:Ӧ�ó������ʼ��ַ
  * @param 	appbuf:  Ӧ�ó���CODE.
  * @param	appsize: Ӧ�ó����С(�ֽ�).
  * @retval None	  
  */
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u32 temp;
	u32 fwaddr=appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu=appbuf;

	for(t=0;t<appsize;t+=4)
	{			
		temp=(u32)dfu[3]<<24;
		temp+=(u32)dfu[2]<<16;
		temp+=(u32)dfu[1]<<8;
		temp+=(u32)dfu[0];	  
		dfu+=4;				//ƫ��4���ֽ�
		iapbuf[i++]=temp;	 	
		if(i==512)
		{
			i=0;
			BSP_FLASHWrite(fwaddr,iapbuf,512);
			fwaddr+=2048; 		//ƫ��2048  32=4*8.����Ҫ����4.
		}
	}
	if(i)BSP_FLASHWrite(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
}

/**
  * @brief  iap_load_app
	* @note   ��ת��Ӧ�ó����
  * @param  appxaddr:�û�������ʼ��ַ.
  * @retval None	  
  */
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)					
		__set_MSP(*(vu32*) appxaddr);  	//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP. 
	}
}	





