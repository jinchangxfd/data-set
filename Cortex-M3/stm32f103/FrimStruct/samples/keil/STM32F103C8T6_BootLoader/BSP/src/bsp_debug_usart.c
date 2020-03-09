/**
  ******************************************************************************
  * �ļ�����: bsp_debug_usart.c 
  * ��    ��: kian
  * ��    ��: V1.0
  * ��д����: 2017-07-19
  * ��    ��: ���Դ��ڵײ���������ʹ��USART2
  ******************************************************************************
  * ˵����
  ******************************************************************************
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_debug_usart.h"

//#include <ctype.h>  //Ϊ��ʵ��debug��ӡ���


/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
uint8_t uart2RcvOneData = 0;



// void fill_rec_buf(char data);  

#if 0

/* �ص����� */
void handle_help(int32_t argc,void *cmd_arg);
void printf_hello(int32_t argc,void *cmd_arg);  
void handle_arg(int32_t argc,void * cmd_arg);
void get_task_state(int32_t argc,void *cmd_arg);


/*�����*/  
const cmd_list_struct cmd_list[CMD_NUM]={  
/*����        ������Ŀ        ������            ������Ϣ                         */     
{"help",    0,          handle_help,    "help                       -��ӡ������Ϣ!"},  
{"hello",   0,          printf_hello,   "hello                      -��ӡHelloWorld!"},  
{"arg",     8,          handle_arg,     "arg<arg1> <arg2> ...       -������,��ӡ����Ĳ���"},  
{"task",    0,          get_task_state, "task                       -��ȡ������Ϣ"},  
}; 
#endif


/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    
    if(huart->Instance == USART2) 
    {
      //fill_rec_buf(uart2RcvOneData);
      HAL_UART_Receive_IT(&huart2, &uart2RcvOneData, 1);
    }
}


/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart2,&ch, 1, 0xffff);
  return ch;
}



#if 0

/*****************************************************************************/
/*******************    �����н�����        START    *******************************/
/*****************************************************************************/
const char *g_pcHex = "0123456789abcdef";


/*
 * ΪUARTprintf()�ṩһ���ײ㴮�ڷ��ͺ���,
 * ��������ָ����Ŀ���ַ������������շ��͵��ַ�������
 */
int32_t UARTwrite(uint8_t *pcBuf, uint32_t ulLen)
{
     HAL_UART_Transmit(&huart2, pcBuf, ulLen, 0xffff);
  return ulLen;
}

/**  
  * ���:   һ���򵥵�printf����,֧��\%c, \%d, \%p, \%s, \%u,\%x, and \%X.  
  */    
 void UARTprintf( uint8_t *pcString, ...)    
{    
    uint32_t ulIdx;    
    uint32_t ulValue;       //����Ӳ�����������ջ��ȡ������ֵ�ͱ���    
    uint32_t ulPos, ulCount;    
    uint32_t ulBase;        //������ƻ���,��ʮ������Ϊ10,ʮ����������Ϊ16    
    uint32_t ulNeg;         //Ϊ1��ʾ�ӱ���Ϊ����    
    uint8_t *pcStr;         //����Ӳ�����������ջ��ȡ�����ַ��ͱ���    
    uint8_t pcBuf[32];      //������ֵ�ͱ����ַ�������ַ�    
    uint8_t cFill;          //'%08x'->����8���ַ���'0'���,cFill='0';      
                            //'%8x '->����8���ַ��ÿո����,cFill=' '    
    va_list vaArgP;    

    va_start(vaArgP, pcString);    
    while(*pcString)    
    {    
        // ������Ѱ��%���ַ��������ַ�    
        for(ulIdx = 0; (pcString[ulIdx] != '%') && (pcString[ulIdx] != '\0'); ulIdx++)    
        { }    
        UARTwrite(pcString, ulIdx);    

        pcString += ulIdx;    
        if(*pcString == '%')    
        {    
            pcString++;    

            ulCount = 0;    
            cFill = ' ';    
            again:    
            switch(*pcString++)    
            {    
                case '0': case '1': case '2': case '3': case '4':    
                case '5': case '6': case '7': case '8': case '9':    
                    {    
                        // �����һ������Ϊ0, ��ʹ��0�����,���ÿո����)    
                        if((pcString[-1] == '0') && (ulCount == 0))    
                        {    
                            cFill = '0';    
                        }    
                        ulCount *= 10;    
                        ulCount += pcString[-1] - '0';    
                        goto again;    
                    }    
                case 'c':            
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        UARTwrite((unsigned char *)&ulValue, 1);    
                        break;    
                    }    
                case 'd':       
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    

                        if((long)ulValue < 0)    
                        {    
                            ulValue = -(long)ulValue;    
                            ulNeg = 1;    
                        }    
                        else    
                        {    
                            ulNeg = 0;    
                        }    
                        ulBase = 10;            
                        goto convert;    
                    }    
                case 's':    
                    {    
                        pcStr = va_arg(vaArgP, unsigned char *);    

                        for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)    
                        {    
                        }    
                        UARTwrite(pcStr, ulIdx);    

                        if(ulCount > ulIdx)    
                        {    
                            ulCount -= ulIdx;    
                            while(ulCount--)    
                            {    
                                UARTwrite(" ", 1);    
                            }    
                        }    
                        break;    
                    }    
                case 'u':    
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    
                        ulBase = 10;    
                        ulNeg = 0;    
                        goto convert;    
                    }    
                case 'x': case 'X': case 'p':    
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    
                        ulBase = 16;    
                        ulNeg = 0;    
                        convert:   //����ֵת�����ַ�    
                        for(ulIdx = 1; (((ulIdx * ulBase) <= ulValue) &&(((ulIdx * ulBase) / ulBase) == ulIdx)); ulIdx *= ulBase, ulCount--)         
                        { }    
                        if(ulNeg)    
                        {    
                            ulCount--;                          
                        }    
                        if(ulNeg && (cFill == '0'))    
                        {    
                            pcBuf[ulPos++] = '-';    
                            ulNeg = 0;    
                        }    
                        if((ulCount > 1) && (ulCount < 16))    
                        {    
                            for(ulCount--; ulCount; ulCount--)    
                            {    
                                pcBuf[ulPos++] = cFill;    
                            }    
                        }    

                        if(ulNeg)    
                        {    
                            pcBuf[ulPos++] = '-';    
                        }    

                        for(; ulIdx; ulIdx /= ulBase)    
                        {    
                            pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase];   
                        }    
                        UARTwrite(pcBuf, ulPos);    
                        break;    
                    }    
                case '%':    
                {    
                    UARTwrite(pcString - 1, 1);                      
                    break;    
                }    
                default:    
                {                        
                    UARTwrite("ERROR", 5);                      
                    break;    
                }    
            }    
        }    
    }    
        //�ɱ�����������    
        va_end(vaArgP);    
    }   

cmd_analyze_struct cmd_analyze;  

/*
 * �ṩ�������жϷ�����򣬱��洮�ڽ��յ��ĵ����ַ�
 */  
void fill_rec_buf(char data)  
{  
    //��������   
    static uint32_t rec_count=0;  
     
    cmd_analyze.rec_buf[rec_count]=data;  
    
    if(0x0A==cmd_analyze.rec_buf[rec_count] && 0x0D==cmd_analyze.rec_buf[rec_count-1])  
    {  
       BaseType_t xHigherPriorityTaskWoken = pdFALSE;  
       rec_count=0;  
         
       /*�յ�һ֡���ݣ��������н�����������֪ͨ*/  
       vTaskNotifyGiveFromISR (xCmdAnalyzeHandle,&xHigherPriorityTaskWoken);  
         
       /*�Ƿ���Ҫǿ���������л�*/  
       portYIELD_FROM_ISR(xHigherPriorityTaskWoken );  
    }  
    else  
    {  
       rec_count++;  
         
       /*�����Դ��룬��ֹ����Խ��*/  
       if(rec_count>=CMD_BUF_LEN)  
       {  
           rec_count=0;  
       }  
    }      
}  

/** 
* ʹ��SecureCRT�����շ�����,�ڷ��͵��ַ����п��ܴ��в���Ҫ���ַ��Լ������ַ�, 
* �����˸��,�����ƶ����ȵ�,��ʹ�������й��߽����ַ���֮ǰ,��Ҫ����Щ�����ַ��� 
* �������ַ�ȥ����. 
* ֧�ֵĿ����ַ���: 
*   ����:1B 5B 41 
*   ����:1B 5B 42 
*   ����:1B 5B 43 
*   ����:1B 5B 44 
*   �س�����:0D 0A 
*  Backspace:08 
*  Delete:7F 
*/  
uint32_t get_true_char_stream(char *dest,const char *src)  
{  
   uint32_t dest_count=0;  
   uint32_t src_count=0;  
     
    while(src[src_count]!=0x0D && src[src_count+1]!=0x0A)  
    {  
       if(isprint(src[src_count]))  
       {  
           dest[dest_count++]=src[src_count++];  
       }  
       else  
       {  
           switch(src[src_count])  
           {  
                case    0x08:                          //�˸����ֵ  
                {  
                    if(dest_count>0)  
                    {  
                        dest_count --;  
                    }  
                    src_count ++;  
                }break;  
                case    0x1B:  
                {  
                    if(src[src_count+1]==0x5B)  
                    {  
                        if(src[src_count+2]==0x41 || src[src_count+2]==0x42)  
                        {  
                            src_count +=3;              //���ƺ����Ƽ���ֵ  
                        }  
                        else if(src[src_count+2]==0x43)  
                        {  
                            dest_count++;               //���Ƽ���ֵ  
                            src_count+=3;  
                        }  
                        else if(src[src_count+2]==0x44)  
                        {  
                            if(dest_count >0)           //���Ƽ���ֵ  
                            {  
                                dest_count --;  
                            }  
                           src_count +=3;  
                        }  
                        else  
                        {  
                            src_count +=3;  
                        }  
                    }  
                    else  
                    {  
                        src_count ++;  
                    }  
                }break;  
                default:  
                {  
                    src_count++;  
                }break;  
           }  
       }  
    }  
    dest[dest_count++]=src[src_count++];  
    dest[dest_count++]=src[src_count++];  
    return dest_count;  
}  


/*�ַ���ת10/16������*/  
static int32_t string_to_dec(uint8_t *buf,uint32_t len)  
{  
   uint32_t i=0;  
   uint32_t base=10;       //����  
   int32_t  neg=1;         //��ʾ����,1=����  
   int32_t  result=0;  
     
    if((buf[0]=='0')&&(buf[1]=='x'))  
    {  
       base=16;  
       neg=1;  
       i=2;  
    }  
    else if(buf[0]=='-')  
    {  
       base=10;  
       neg=-1;  
       i=1;  
    }  
    for(;i<len;i++)  
    {  
       if(buf[i]==0x20 || buf[i]==0x0D)    //Ϊ�ո�  
       {  
           break;  
       }  
         
       result *= base;  
       if(isdigit(buf[i]))                 //�Ƿ�Ϊ0~9  
       {  
           result += buf[i]-'0';  
       }  
       else if(isxdigit(buf[i]))           //�Ƿ�Ϊa~f����A~F  
       {  
            result+=tolower(buf[i])-87;  
       }  
       else  
       {  
           result += buf[i]-'0';  
       }                                          
    }  
   result *= neg;  
     
    return result ;  
}  



/** 
* ���������������,�Կո���Ϊһ����������,֧������ʮ��������(��:0x15),֧�����븺��(��-15) 
* @param rec_buf   ������������� 
* @param len       ����������ܳ��� 
* @return -1:       ������������,����:�������� 
*/  
int32_t cmd_arg_analyze(char *rec_buf,unsigned int len)  
{  
   uint32_t i;  
   uint32_t blank_space_flag=0;    //�ո��־  
   uint32_t arg_num=0;             //������Ŀ  
   uint32_t index[ARG_NUM];        //��Ч�����׸����ֵ���������  
     
    /*����һ�����,�ҳ���������Ŀ,�Լ������ε��׸���������rec_buf�����е��±�*/  
    for(i=0;i<len;i++)  
    {  
       if(rec_buf[i]==0x20)        //Ϊ�ո�  
       {  
           blank_space_flag=1;                
           continue;  
       }  
        else if(rec_buf[i]==0x0D)   //����  
       {  
           break;  
       }  
       else  
       {  
           if(blank_space_flag==1)  
           {  
                blank_space_flag=0;   
                if(arg_num < ARG_NUM)  
                {  
                   index[arg_num]=i;  
                    arg_num++;           
                }  
                else  
                {  
                    return -1;      //��������̫��  
                }  
           }  
       }  
    }  
     
    for(i=0;i<arg_num;i++)  
    {  
        cmd_analyze.cmd_arg[i]=string_to_dec((unsigned char *)(rec_buf+index[i]),len-index[i]);  
    }  
    return arg_num;  
}  



/*
 *����ص�����
 *��ӡ�ַ���:Hello world!
 */  
void printf_hello(int32_t argc,void *cmd_arg)  
{  
   MY_DEBUGF(CMD_LINE_DEBUG,("Hello world!\n"));  
}  

/*
 *����ص�����
 *��ӡÿ������
 */  
void handle_arg(int32_t argc,void * cmd_arg)  
{  
   uint32_t i;  
  // uint8_t  *arg=(uint8_t *)cmd_arg;  
   int32_t  *arg=(int32_t *)cmd_arg; 
     
    if(argc==0)  
    {  
       MY_DEBUGF(CMD_LINE_DEBUG,("�޲���\n"));  
    }  
    else  
    {  
       for(i=0;i<argc;i++)  
       {  
           MY_DEBUGF(CMD_LINE_DEBUG,("��%d������:%d\n",(i+1),arg[i]));  
       }  
    }  
}  


/*
 *����ص�����
 *��ȡOS������Ϣ
 */  
void get_task_state(int32_t argc,void *cmd_arg)  
{  
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, x;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
   
    /* ��ȡ��������Ŀ */  
    uxArraySize = uxTaskGetNumberOfTasks();  
   if(uxArraySize>MAX_TASK_NUM)  
    {  
        MY_DEBUGF(CMD_LINE_DEBUG,("��ǰ�����������࣡\n"));  
    }  
   
    /*��ȡÿ�������״̬��Ϣ */  
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime );  
   
    #if (configGENERATE_RUN_TIME_STATS==1)  
    
    MY_DEBUGF(CMD_LINE_DEBUG,("========\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("������          ״̬  ID    ���ȼ�  ��ջ    CPUʹ����\n"));  
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));  
   
    /* ���������� */  
    if( ulTotalRunTime > 0 )  
    {  
        /* ����õ�ÿһ������״̬��Ϣ���ֵ�ת��Ϊ����Ա����ʶ����ַ�����ʽ */  
        for( x = 0; x < uxArraySize; x++ )  
        {  
            char tmp[128];  
             
            /* ������������ʱ����������ʱ��İٷֱȡ�*/  
            ulStatsAsPercentage =(uint64_t)(pxTaskStatusArray[ x ].ulRunTimeCounter)*100 / ulTotalRunTime;  
   
            if( ulStatsAsPercentage > 0UL )  
            {  
   
               sprintf(tmp,"%-16s%-6c%-6d%-8d%-8d%d%%",pxTaskStatusArray[ x].pcTaskName,task_state[pxTaskStatusArray[ x ].eCurrentState],  
                                                       pxTaskStatusArray[ x ].xTaskNumber,pxTaskStatusArray[ x].uxCurrentPriority,  
                                                       pxTaskStatusArray[ x ].usStackHighWaterMark,ulStatsAsPercentage);  
            }  
            else  
            {  
                /* ��������ʱ�䲻��������ʱ���1%*/  
                sprintf(tmp,"%-16s%-6c%-6d%-8d%-8dt<1%%",pxTaskStatusArray[x ].pcTaskName,task_state[pxTaskStatusArray[ x ].eCurrentState],  
                                                                       pxTaskStatusArray[ x ].xTaskNumber,pxTaskStatusArray[ x].uxCurrentPriority,  
                                                                       pxTaskStatusArray[ x ].usStackHighWaterMark);                 
            }  
           MY_DEBUGF(CMD_LINE_DEBUG,("%s\n",tmp));  
        }  
    }      
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("����״̬:   r-����  R-����  B-����  S-����  D-ɾ��\n\n"));       
    MY_DEBUGF(CMD_LINE_DEBUG,("[��ջ]�����Ӧ����ʣ��Ķ�ջ�ռ䣬��λ��StackType_t���ͣ�\n      �����������ֲ�㶨�壬һ�㶨��Ϊ4�ֽڡ�\n\n"));   
    #endif //#if (configGENERATE_RUN_TIME_STATS==1)  
}  


/*
 *����ص�����
 *��ȡOS������Ϣ
 */  
void handle_help(int32_t argc,void *cmd_arg)   
{ 
    uint8_t n = 0;
    uint8_t x = 0;

    n = ( sizeof(cmd_list) / sizeof(cmd_list[0]) );
    
    MY_DEBUGF(CMD_LINE_DEBUG,("========\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("������                    ˵��\n"));  
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));  
   
    /* ����õ�ÿһ������״̬��Ϣ���ֵ�ת��Ϊ����Ա����ʶ����ַ�����ʽ */  
    for( x = 0; x < n; x++ )  
    {  
        char tmp[128];                 
          
        /* ��������ʱ�䲻��������ʱ���1%*/  
        sprintf(tmp,"%s",cmd_list[x].help);
      
        MY_DEBUGF(CMD_LINE_DEBUG,("%s\n",tmp));  
    }  
    MY_DEBUGF(CMD_LINE_DEBUG,("\n"));
}


/*****************************************************************************/
/*******************    �����н�����        END      *******************************/
/*****************************************************************************/

#endif

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
