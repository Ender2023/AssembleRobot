#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "display.h"

/*打印缓存*/
char LCD_LOG_FIFO[DISPLAY_FIFO_ROW][DISPLAY_FIFO_COL];

uint8_t LOG_row_ctr = 0;

/**
 * @brief:显示器初始化
*/
__inline void Display_Init(void)
{
    LCD_Init();
	Display_Logged("LCD Init done!\n");
}

/**
 * @brief:  打印系统日志
 * @param:  系统日志
 * @retval: None
*/
void Display_Logged(const char *fmt,...)
{
	static char tmp_buf[32];
	va_list ap;

	va_start(ap,fmt);
	vsprintf(tmp_buf,fmt,ap);
	va_end(ap);

	if(LOG_row_ctr < DISPLAY_FIFO_ROW)
	{
		/*重设打印模式*/
        LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
        /*压入队列*/
		strcpy(&LCD_LOG_FIFO[LOG_row_ctr][0],tmp_buf);
		LCD_Printf("%s",LCD_LOG_FIFO[LOG_row_ctr]);
		LOG_row_ctr ++;
	}
	else
	{
        /*重设打印模式*/
        LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
		/*清屏*/
		LCD_FillColor(LCD_FULLSCREEN,LCD_COLOR_BLACK);

        /*移位、尾插*/
		for(LOG_row_ctr = 0;LOG_row_ctr < ( DISPLAY_FIFO_ROW - 1 );LOG_row_ctr ++)
		{
			memcpy(&LCD_LOG_FIFO[LOG_row_ctr][0],&LCD_LOG_FIFO[LOG_row_ctr + 1][0],DISPLAY_FIFO_COL * sizeof(char));
		}

		strcpy(&LCD_LOG_FIFO[LOG_row_ctr][0],tmp_buf);

        /*打印*/
		for(LOG_row_ctr = 0;LOG_row_ctr < DISPLAY_FIFO_ROW;LOG_row_ctr ++)
		{
            LCD_Printf("%s",LCD_LOG_FIFO[LOG_row_ctr]);
		}
	}
}

/************************************************************
*						End of File
************************************************************/
