#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "bsp_serial.h"

static char serialDataBuf[SERIAL_PRINTFBUF_SIZE]; 

/**
 * @brief:  串口打印，类似于printf
 * @param:  ttySx:   输出的串口
 * @param:  fmt:     格式化字符串
 * @param:  ...:     泛型不定参
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
 * @brief:  串口发送16进制数组
 * @param:  ttySx:      输出的串口
 * @param:  pSendBuf:   数组缓冲地址
 * @param:  length:     要发送的长度
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
