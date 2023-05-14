#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "system.h"
#include "bsp_rgblcd.h"

/*定义系统最大记录条数*/
#define DISPLAY_MAX_LOG_CNT 512
/*定义显示队列最大行数*/
#define DISPLAY_FIFO_ROW    7
/*定义显示队列每行最大字节数*/
#define DISPLAY_FIFO_COL    32

/*日志警告等级*/
#define LOG_RANK_INFO       LCD_COLOR_WHITE
#define LOG_RANK_OK         LCD_COLOR_GREEN
#define LOG_RANK_WARNNING   LCD_COLOR_YELLOW
#define LOG_RANK_ERROR      LCD_COLOR_RED

extern char LCD_LOG_FIFO[DISPLAY_MAX_LOG_CNT][DISPLAY_FIFO_COL];
extern uint16_t LOG_WARNNING_RANK[DISPLAY_MAX_LOG_CNT];
extern uint32_t LOG_count;
extern uint32_t LOG_now;

void Display_Init(void);
void Display_LogReset(void);
void Display_Logged(uint16_t rank,const char *fmt,...);
void Display_CheckLog(uint32_t LogIndex);

#endif

/************************************************************
*						End of File
************************************************************/
