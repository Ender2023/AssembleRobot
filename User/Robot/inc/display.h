#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "system.h"
#include "bsp_rgblcd.h"

/*定义显示队列最大行数*/
#define DISPLAY_FIFO_ROW    8
/*定义显示队列每行最大字节数*/
#define DISPLAY_FIFO_COL    32

extern char LCD_DispFIFO[DISPLAY_FIFO_ROW][DISPLAY_FIFO_COL];

void Display_Init(void);
void Display_LogReset(void);
void Display_Logged(const char *fmt,...);

#endif

/************************************************************
*						End of File
************************************************************/
