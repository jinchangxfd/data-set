#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdarg.h>               /*֧�ֺ������ղ���������*/ 
#include "string.h"


/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define MY_DEBUG  //��debug ���Դ�ӡ���ܣ������ε�������е�MY_DEBUGF(message)��ӡ���ܴӴ�����ȥ��
#define MAX_TASK_NUM        8  //������������������������ʱ���õ�

#if 0
#define DEBUG_USARTx                                 USART1
#define DEBUG_USARTx_BAUDRATE                        115200
#define DEBUG_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART1_CLK_DISABLE()

#define DEBUG_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN                     GPIO_PIN_9
#define DEBUG_USARTx_Tx_GPIO                         GPIOA
#define DEBUG_USARTx_Rx_GPIO_PIN                     GPIO_PIN_10
#define DEBUG_USARTx_Rx_GPIO                         GPIOA

#define DEBUG_USART_IRQn                             USART1_IRQn
#endif
/* ��չ���� ------------------------------------------------------------------*/
//extern UART_HandleTypeDef husart_debug;

/* ��չ����------------------------------------------------------------------*/
//extern void UARTprintf( uint8_t *pcString, ...);    

/* �������� ------------------------------------------------------------------*/
void MX_DEBUG_USART_Init(void);




/******************************************************************************/
/****************    �����н�����ʵ��            **************************************/
/******************************************************************************/
#define CMD_NUM         4   //������Ŀ������ʵ���޸�
#define CMD_LINE_DEBUG  1
#define MY_DBG_ON       0x01


#ifdef MY_DEBUG    
/*
*   ʹ�÷�����
*    ��1�� MY_DEBUGF(CMD_LINE_DEBUG, ("��%d������:%d\n", (i+1), arg[i]) );
*    ��2��   MY_DEBUGF(CMD_LINE_DEBUG, ("�޲���\n") ); 
*/
#define MY_DEBUGF(debug, message) do {\
                                if ( ((debug) & MY_DBG_ON)){UARTprintf message;}\
                               } while(0)    
#else      
#define MY_DEBUGF(message)      
#endif /* PLC_DEBUG */ 

typedef struct {  
     char const *cmd_name;                        //�����ַ���  
     uint32_t max_args;                            //��������Ŀ  
     void (*handle)(int argc,void * cmd_arg);     //����ص�����  
     char  *help;                                 //������Ϣ  
 }cmd_list_struct;  

#define ARG_NUM     8          //����������Ĳ�������  
#define CMD_LEN     20         //������ռ�õ�����ַ�����  
#define CMD_BUF_LEN 60         //��������󳤶�  
   
typedef struct {  
    char rec_buf[CMD_BUF_LEN];            //�����������  
    char processed_buf[CMD_BUF_LEN];      //�洢�ӹ��������(ȥ�������ַ�)  
    uint32_t cmd_arg[ARG_NUM];             //��������Ĳ���  
}cmd_analyze_struct;  


uint32_t get_true_char_stream(char *dest,const char *src);  
int32_t cmd_arg_analyze(char *rec_buf,unsigned int len);


/******************************************************************************/
/****************    �����н�����ʵ��         END   ***********************************/
/******************************************************************************/



#endif  /* __BSP_DEBUG_USART_H__ */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
