#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "bsp_serial.h"

static char serialDataBuf[SERIAL_PRINTFBUF_SIZE]; 

/**
 * @brief:  ���ڴ�ӡ��������printf
 * @param:  ttySx:   ����Ĵ���
 * @param:  fmt:     ��ʽ���ַ���
 * @param:  ...:     ���Ͳ�����
*/
void serial_Printf(USART_TypeDef * ttySx,const char *fmt,...)
{
	__IO char * pSendBuf = serialDataBuf;

	va_list ap;
	va_start(ap,fmt);
	vsprintf(serialDataBuf,fmt,ap);
	va_end(ap);

    while(*pSendBuf != '\0')
    {
        ttySx->SR;
        USART_SendData(ttySx,*pSendBuf);
        pSendBuf ++;
        while(USART_GetFlagStatus(ttySx,USART_FLAG_TC) == RESET);
    }
}

/**
 * @brief:  ���ڷ���16��������
 * @param:  ttySx:      ����Ĵ���
 * @param:  pSendBuf:   ���黺���ַ
 * @param:  length:     Ҫ���͵ĳ���
*/
void serial_SendHexArray(USART_TypeDef * ttySx,char * pSendBuf,uint16_t length)
{
    uint16_t ctr;
    
    for(ctr = 0; ctr < length; ctr ++)
    {
        ttySx->SR;
        USART_SendData(ttySx,*pSendBuf);
        pSendBuf ++;
        while(USART_GetFlagStatus(ttySx,USART_FLAG_TC) == RESET);
    }
}

/************************************************************
*						End of File
************************************************************/
