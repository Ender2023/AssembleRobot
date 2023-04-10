#include "bsp_serial.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

/**
 * @brief:  ���Դ��ڳ�ʼ������
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
