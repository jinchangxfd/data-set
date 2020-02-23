/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFiģ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨ :Ұ�� iSO STM32 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "bsp_dht11.h"
#include "bsp_gp2y.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) {
    // ��ʼ������1
    USARTx_Config();
    // ���� SysTick Ϊ 1ms �ж�һ��
    SysTick_Init();

    // ��ʼ��WiFiģ��ʹ�õĽӿں�����
    ESP8266_Init();
    // ��ʼ��DHT11��ʪ�ȴ�����ģ��
    DHT11_Init();
    // ��ʼ��PM25������ģ��
    PM_Init();

    // ��ӡ����������ʾ��Ϣ
    printf("\r\nAir Iot Starting...\r\n");

    // WIFI͸����������
    ESP8266_StaTcpClient_UnvarnishTest();

    while (1);
}


/*********************************************END OF FILE**********************/
