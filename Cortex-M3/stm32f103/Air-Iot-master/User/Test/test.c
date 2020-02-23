#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bsp_dht11.h"
#include "bsp_gp2y.h"

volatile uint8_t ucTcpClosedFlag = 0;

/**
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest(void) {
    uint8_t ucStatus;

    char cStr[100] = {0};
		
		// IMEI 15BYTE
		char imei[30] = {'1','1','2','2','3','3','4','4','5','5','6','6','7','7','8','8','9','9','A','A','B','B','C','C','D','D','E','E','F','F'};

    // DHT11���ݽṹ��
    DHT11_Data_TypeDef DHT11_Data;
    // PM25���ݽṹ��
    PM_Data_TypeDef PM_Data;

    printf("\r\n�������� ESP8266 WIFIģ��......\r\n");

    macESP8266_CH_ENABLE();

    ESP8266_AT_Test();

    ESP8266_Net_Mode_Choose(STA);

    while (!ESP8266_JoinAP(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));

    ESP8266_Enable_MultipleId(DISABLE);

    while (!ESP8266_Link_Server(enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0));

    while (!ESP8266_UnvarnishSend());

    printf("\r\n���� ESP8266 ���\r\n");

    // ����ѭ����ȡ����
    while (1) {

        // FIXME ��ȡPM25������ģ������
        if (PM_Read_DATA(&PM_Data) == SUCCESS) {
            sprintf(cStr,
					"G{imei:\"%s\",start:\"%d\",voutH:\"%d\",voutL:\"%d\",vrefH:\"%d\",vrefL:\"%d\",check:\"%d\",finish:\"%d\"}",
                   imei,PM_Data.start,PM_Data.voutH, PM_Data.voutL, PM_Data.vrefH, PM_Data.vrefL, PM_Data.check_sum, PM_Data.finish);
        } else
            sprintf(cStr, "PM25 Monitoring failure...");

        // ��ӡ���������ݵ�WIFI��Ϣ����
        printf("%s", cStr);
        // ͨ��WIFI���������ַ���
        ESP8266_SendString(ENABLE, cStr, 0, Single_ID_0);

        // FIXME ��ȡDHT11��ʪ�ȴ���������
        if (DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS) {
            sprintf(cStr,
					"D{imei:\"%s\",tempInt:\"%d\",tempDeci:\"%d\",humiInt:\"%d\",humiDeci:\"%d\"}",
                    imei,DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci);
        } else
            sprintf(cStr, "DHT11 Monitoring failure...");


        // ��ӡ���������ݵ�WIFI��Ϣ����
        printf("%s", cStr);
        // ͨ��WIFI���������ַ���
        ESP8266_SendString(ENABLE, cStr, 0, Single_ID_0);

        Delay_ms(1000);

        // ����Ƿ�ʧȥ����
        if (ucTcpClosedFlag) {
            // �˳�͸��ģʽ
            ESP8266_ExitUnvarnishSend();

            // ��ȡ����״̬
            do ucStatus = ESP8266_Get_LinkStatus();
            while (!ucStatus);

            // ȷ��ʧȥ���Ӻ�����
            if (ucStatus == 4) {
                printf("\r\n���������ȵ�ͷ����� ......\r\n");

                while (!ESP8266_JoinAP(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));

                while (!ESP8266_Link_Server(enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port,
                                            Single_ID_0));

                printf("\r\n�����ȵ�ͷ������ɹ�\r\n");

            }

            while (!ESP8266_UnvarnishSend());

        }

    }

}


