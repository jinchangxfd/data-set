#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
	 
 
#define KEY0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)//��ȡ����0

#define KEY0_Press	1

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
