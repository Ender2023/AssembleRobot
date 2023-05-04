#ifndef __BSP_SERIAL_H
#define __BSP_SERIAL_H

#include "system.h"

#define	SERIAL_MAXDATA_SIZE		    1024
#define SERIAL_PRINTFBUF_SIZE       128

#define TTYS_DEBUG                  UART5
#define TTYS_DEBUG_RCC              RCC_APB1Periph_UART5
#define TTYS_DEBUG_BUADRATE         115200

/*串口数据帧格式定义*/
typedef struct USART_DataFrame
{
	char RX_BUF[SERIAL_MAXDATA_SIZE];

	union
	{
		volatile uint16_t FrameAll;

		struct
		{
			volatile uint16_t FrameCNT 			:15;		//记录已接收数据帧数量
			volatile uint16_t FrameFinishFlag  	:1;			//记录数据帧的终止位
		}FrameBit;

	};

}USART_DataFrameType;

void serial_Printf(USART_TypeDef * ttySx,const char *fmt,...);
void serial_SendHexArray(USART_TypeDef * ttySx,char * pSendBuf,uint16_t length);
void serialDebugInit(void);

#endif

/************************************************************
*						End of File
************************************************************/
