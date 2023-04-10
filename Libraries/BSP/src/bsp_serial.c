#include "bsp_serial.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

/**
 * @brief:  调试串口初始化函数
*/
void serialDebugInit(void)
{
    USART_InitTypeDef   USART_InitStruct;
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD,&GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = TTYS_DEBUG_BUADRATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    USART_Init(TTYS_DEBUG,&USART_InitStruct);

    USART_Cmd(TTYS_DEBUG,ENABLE);
}
