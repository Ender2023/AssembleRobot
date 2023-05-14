#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "display.h"
#include "UI.h"

/*打印缓存*/
char LCD_LOG_FIFO[DISPLAY_MAX_LOG_CNT][DISPLAY_FIFO_COL];

/*警报等级*/
uint16_t LOG_WARNNING_RANK[DISPLAY_MAX_LOG_CNT];

static uint8_t LOG_row_ctr = 0;	//记录当前打印的行数
uint32_t LOG_count;				//记录当前已有的记录条数
uint32_t LOG_now;				//当前所指向的记录条数

/**
 * @brief:显示器初始化
*/
__inline void Display_Init(void)
{
    LCD_Init();
	Display_Logged(LOG_RANK_OK,"LCD Init done!\n");
}

/**
 * @breif:	清空记录输出信息，重新开始打印
*/
void Display_LogReset(void)
{
	/*清空记录条数*/
	LOG_count = 0;
	/*重设打印模式*/
	LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
}

/**
 * @brief:  打印系统日志
 * @param:	打印等级
 * @param:  要打印的日志信息
 * @retval: None
*/
void Display_Logged(uint16_t rank,const char *fmt,...)
{
	static char tmp_buf[32];
	va_list ap;

	va_start(ap,fmt);
	vsprintf(tmp_buf,fmt,ap);
	va_end(ap);

	if(LOG_count < DISPLAY_MAX_LOG_CNT)
	{
        /*压入队列*/
		LOG_WARNNING_RANK[LOG_count] = rank;
		strcpy(&LCD_LOG_FIFO[LOG_count][0],tmp_buf);

		/*当处于调试信息模式时才会打印信息*/
		if( UI_routinue == UI_info )
		{
			if( LOG_count < DISPLAY_FIFO_ROW )
			{
				LCD_SetPrintColor(LOG_WARNNING_RANK[LOG_count]);

				/*上电初始化不打印日志序号*/
				if(SYSTEM_INIT_DONE)
				{
					LCD_Printf("%d %s",LOG_count,LCD_LOG_FIFO[LOG_count]);
				}
				else
				{
					LCD_Printf("%s",LCD_LOG_FIFO[LOG_count]);
				}
			}
			else
			{
				/*重设打印模式*/
				LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
				/*清屏*/
				LCD_FillColor(LCD_FULLSCREEN,LCD_COLOR_BLACK);
				for( LOG_row_ctr = LOG_count - DISPLAY_FIFO_ROW + 1; LOG_row_ctr <= LOG_count ; LOG_row_ctr ++)
				{
					LCD_SetPrintColor(LOG_WARNNING_RANK[LOG_row_ctr]);

					/*上电初始化不打印日志序号*/
					if(SYSTEM_INIT_DONE)
					{
						LCD_Printf("%d %s",LOG_row_ctr,LCD_LOG_FIFO[LOG_row_ctr]);
					}
					else
					{
						LCD_Printf("%s",LCD_LOG_FIFO[LOG_row_ctr]);
					}
				}
			}

			if(SYSTEM_INIT_DONE)
			{
				LCD_Set_Printfmt(0,104,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
				LCD_Printf("K1-Up\nK2-Down\tK3[L]-Back");
			}
		}

		LOG_count ++;
	}
	else
	{
		/*记录满，清空记录*/
		Display_LogReset();

		// /*当处于调试信息模式时才会打印信息*/
		// if( UI_routinue == UI_info )
		// {
		// 	/*重设打印模式*/
		// 	LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
		// 	/*清屏*/
		// 	LCD_FillColor(LCD_FULLSCREEN,LCD_COLOR_BLACK);
		// }

        // /*移位、尾插*/
		// for(LOG_count = 0;LOG_count < ( DISPLAY_MAX_LOG_CNT - 1 );LOG_count ++)
		// {
		// 	memcpy(&LOG_WARNNING_RANK[LOG_count],&LOG_WARNNING_RANK[LOG_count + 1],sizeof(int16_t));
		// 	memcpy(&LCD_LOG_FIFO[LOG_count][0],&LCD_LOG_FIFO[LOG_count + 1][0],DISPLAY_FIFO_COL * sizeof(char));
		// }

		// LOG_WARNNING_RANK[LOG_count] = rank;
		// strcpy(&LCD_LOG_FIFO[LOG_count][0],tmp_buf);

        
		// /*当处于调试信息模式时才会打印信息*/
		// if( UI_routinue == UI_info )
		// {
		// 	for(LOG_count = 0;LOG_count < DISPLAY_FIFO_ROW;LOG_count ++)
		// 	{
		// 		LCD_SetPrintColor(LOG_WARNNING_RANK[LOG_count]);
		// 		LCD_Printf("%s",LCD_LOG_FIFO[LOG_count]);
		// 	}
		
		// 	if(SYSTEM_INIT_DONE)
		// 	{
		// 		LCD_Set_Printfmt(0,104,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
        //     	LCD_Printf("K1-Up\nK2-Down\tK3[L]-Back");
		// 	}

		// }
	}
}

/**
 * @brief:  查询系统日志
 * @param:  自要查询的记录数向前的DISPLAY_FIFO_ROW条记录
 * @retval: None
*/
void Display_CheckLog(uint32_t LogIndex)
{
	if( LOG_count < DISPLAY_FIFO_ROW )
	{
		LCD_SetPrintColor(LOG_WARNNING_RANK[LOG_count]);
		LCD_Printf("%d %s",LOG_count,LCD_LOG_FIFO[LOG_count]);
	}
	else
	{
		/*重设打印模式*/
		LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
		/*清屏*/
		LCD_FillColor(LCD_FULLSCREEN,LCD_COLOR_BLACK);
		for( LOG_row_ctr = LogIndex - DISPLAY_FIFO_ROW + 1; LOG_row_ctr <= LogIndex ; LOG_row_ctr ++)
		{
			LCD_SetPrintColor(LOG_WARNNING_RANK[LOG_row_ctr]);
			LCD_Printf("%d %s",LOG_row_ctr,LCD_LOG_FIFO[LOG_row_ctr]);
		}
	}

    LCD_FillColor(0,104,128,128,LCD_COLOR_BLACK);
	LCD_Set_Printfmt(0,104,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
	LCD_Printf("K1-Up\nK2-Down\tK3[L]-Back");

}

/************************************************************
*						End of File
************************************************************/
