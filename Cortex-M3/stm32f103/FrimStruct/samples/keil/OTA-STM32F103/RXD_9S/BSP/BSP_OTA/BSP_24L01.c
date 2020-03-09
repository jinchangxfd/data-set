#include "BSP_24l01.h"

u8  OTA_RX[OTA_RX_LEN] __attribute__ ((at(0X20001000)));	//���ջ���,���OTA_RX_LEN���ֽ�,��ʼ��ַΪ0X20001000.  ����������6KB  0X20001000
u8  SPI_RX_BUF[SPI_REC_LEN];
u16 Rxd_NUM = 0;

u8 TX_ADDRESS[TX_ADR_WIDTH]; //���͵�ַ
u8 RX_ADDRESS[RX_ADR_WIDTH];


/**
  * @brief  NRF24L01��ȡ�̼�����
  * @retval 0 û������ or 1 ���յ�����
  */
u8 NRF24L01_RxLenth(void)
{	  		    		 
	if(NRF24L01_RxPacket(SPI_RX_BUF) == 0)	
	{			
		AppLenth = SPI_RX_BUF[1]*256 + SPI_RX_BUF[0];		// �������ݳ���
		if (AppLenth%32 != 0) Rxd_NUM = AppLenth/32 + 1;		// ����NRF���ʹ���
		else  Rxd_NUM = AppLenth/32;
		printf("�̼�����:%dBytes\r\n",AppLenth);
		
		return 1; 
	}
	return 0;
}

/**
  * @brief  NRF24L01���չ̼�
  * @retval 1 û������ or 0 ���յ�����
  */
u8 NRF24L01_Rx(u16 x)
{	  		    		  
	u8 i = 0;
	u16 Index;
	Index = x * 28;
	
	if(NRF24L01_RxPacket(SPI_RX_BUF) == 0)	//һ�����յ���Ϣ,����ʾ����.
	{	
		for (i = 0; i< 28; i++)
		{
			OTA_RX[Index + i] = SPI_RX_BUF[i+3];	// ���浽SRAM
		}
	
		//printf("���յ� %3d %3d \r\n",SPI_RX_BUF[16],SPI_RX_BUF[0]+SPI_RX_BUF[1]*256);  // ����ʹ��
		return 0; 
	}
	 
	return 1;
}

/**
  * @brief  ����NRF24L01����һ������
  * @param  txbuf:�����������׵�ַ
  * @retval 0��������ɣ��������������
  */
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;	

	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}

	return 1;//û�յ��κ�����
}					 

/**
  * @brief  ��ȡSPI�Ĵ���ֵ
  * @param  reg:Ҫ���ļĴ���
  * @retval ����״ֵ̬
  */
u8 NRF24L01_Read_Reg(u8 reg)
{
		u8 reg_val;	    
	
		NRF24L01_CSN(RESET);          //ʹ��SPI����	
  	SPI2_ReadWriteByte(reg);   //���ͼĴ�����
  	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CSN(SET);          //��ֹSPI����		    
  
		return(reg_val);           //����״ֵ̬
}	

/**
  * @brief  SPIд�Ĵ���
  * @param  reg:ָ���Ĵ�����ַ
	* @param	value:д���ֵ
  * @retval ����״ֵ̬
  */
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
		u8 status;	
	
   	NRF24L01_CSN(RESET);                 //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);	//���ͼĴ����� 
  	SPI2_ReadWriteByte(value);     	 //д��Ĵ�����ֵ
  	NRF24L01_CSN(SET);                 //��ֹSPI����	  
	
  	return(status);       			//����״ֵ̬
}	

/**
  * @brief  ��ָ��λ�ö���ָ�����ȵ�����
  * @param  reg:�Ĵ���(λ��)
	* @param	*pBuf:����ָ��
	* @param	len:���ݳ���
  * @retval �˴ζ�����״̬�Ĵ���ֵ 
  */
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{	 
	u8 status,u8_ctr;	    
	
	NRF24L01_CSN (RESET);           //ʹ��SPI����
	status=SPI2_ReadWriteByte(reg);	//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++) pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);	//��������
	NRF24L01_CSN (SET);       //�ر�SPI����
	
	return status;        //���ض�����״ֵ̬
}

/**
  * @brief  ��ָ��λ��дָ�����ȵ�����
  * @param  reg:�Ĵ���(λ��)
	* @param	*pBuf:����ָ��
	* @param	len:���ݳ���
  * @retval �˴ζ�����״̬�Ĵ���ֵ
  */
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u16 len)
{	 
		u8 status,u8_ctr;	
	
		NRF24L01_CSN(RESET);          //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);		//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++) SPI2_ReadWriteByte(*pBuf++); //д������	 
		NRF24L01_CSN(SET);       //�ر�SPI����
		
		return status;          //���ض�����״ֵ̬
}	

/**
  * @brief  ������ʼ��NRF24L01��RXģʽ
  * @param  ����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR,
						��CE��ߺ�,������RXģʽ,�����Խ���������		
  * @retval NONE
  */
void NRF24L01_RX_Mode(void)
{
		NRF24L01_CE(RESET);	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ�� 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f); //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f); //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	NRF24L01_CE(SET); //CEΪ��,�������ģʽ 
		HAL_Delay(1);
	
	printf("NRF24L01 Mode: Receive\r\n");	
}	

/**
  * @brief  ���24L01�Ƿ����
  * @param  txbuf:�����������׵�ַ
  * @retval 0���ɹ� or 1��ʧ��
  */
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	
//	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ
		
	for(i=0;i<5;i++) if(buf[i]!=0XA5) break;	 							   
	if(i!=5) return 1;//���24L01����	
	return 0;		 //��⵽24L01

}


/**
  * @brief  ��ʼ��24L01��IO��
  * @retval NONE
  */
void NRF24L01_Init(void)
{
	HAL_GPIO_WritePin(NRF_IRQ_GPIO_Port,NRF_IRQ_Pin,GPIO_PIN_SET);	// NRF_IRQ_Pin����

	while(NRF24L01_Check())
	{
		printf("NRF24L01 Error");
		HAL_Delay(200);
	}
	printf("NRF24L01 Init OK\r\n");
}





