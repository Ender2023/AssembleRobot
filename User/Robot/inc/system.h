#ifndef __SYSTEM_H
#define __SYSTEM_h

#pragma anon_unions

/************************************************************
*						    编译器配置
************************************************************/
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 5000000) /* ARM Compiler \*/
#define __WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define __WEAK __weak
#elif defined(__MINGW32__) /* MINGW32 Compiler */
#define __WEAK
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define __WEAK __attribute__((weak))
#endif
/* default _WEAK */
#ifndef __WEAK
#define __WEAK
#endif

/************************************************************
*						 头文件
************************************************************/
#include <stdbool.h>
#include "stm32f10x.h"
#include "display.h"


/************************************************************
*					    系统定时器
************************************************************/
#define	SYSTEM_TIMER				TIM6
#define SYSTEM_TIMER_IRQN			TIM6_IRQn
#define SYSTEM_TIMER_USE_PERIPH		RCC_APB1Periph_TIM6
#define SYSTEM_TIMER_CLK_DIV		1
#define SYSTEM_TIMER_PRESCALER		72
#define SYSTEM_TIMER_PERIOD			5000

#define SYSTEM_TIMER_EVENTHANDLER	TIM6_IRQHandler

#define OPTION_RESERVED_X5MS        200
#define SYSTEM_TIMER_CAMERA         4
#define SYSTEM_TIMER_KEY            2

/************************************************************
*						    调试
************************************************************/
#define SYS_DEBUG_ON        1
#define SYS_DEBUG_OFF       0

#define SYS_DEBUG_SW        SYS_DEBUG_ON

#if     SYS_DEBUG_SW == SYS_DEBUG_ON
#define SYS_DEBUG(...)      do{\
                            serial_Printf(TTYS_DEBUG,"file:%s func:%s line:%d:",__FILE__,__func__,__LINE__);\
                            serial_Printf(TTYS_DEBUG,__VA_ARGS__);\
                            }while(0)

#else
#define SYS_DEBUG(...)      __NOP()
#endif

#if SYS_DEBUG_SW == SYS_DEBUG_ON
#include "debug.h"
#endif


/************************************************************
*						全局定义
************************************************************/
#define pi                      3.14159f

#define rads_to_angle(x)        ( ( 180.0f / pi ) * x )
#define angle_to_rads(x)        ( ( pi / 180.0f ) * x )

/************************************************************
*						快捷指令
************************************************************/
#define assert(c1,req,c2,abort)		( c1 req c2 ) ? __NOP() : abort ;

#define max2(a,b)					( ( a > b ) ? ( a ) : ( b ) )
#define min2(a,b)					( ( a < b ) ? ( a ) : ( b ) )
#define max3(a,b,c)					( ( a > max2(b,c) ) ? ( a ) : max2(b,c) )
#define min3(a,b,c)					( ( a < min2(b,c) ) ? ( a ) : min2(b,c) )

#define clr	        				LCD_FillColor(LCD_FULLSCREEN,LCD_COLOR_BLACK);
#define keyban(KEYX)				KEYX.STA=KEY_STA_FORBIDEN;

/*角度映射至-180° ~ +180°*/
#define angle_mapping(_angle)    {                                                       \
                                    _angle = fmod(_angle,360);                           \
                                    if( _angle > 180 )                                   \
                                    {_angle -= 360;}                                     \
                                    else if( _angle < -180 )                             \
                                    {_angle += 360;}                                     \
                                }


extern uint8_t RandSeed;//随机刻种子

/**
 * DECLARE
*/

void System_Init(void);
void ASSERT_ERR(void * obj,int8_t code);

#endif

/************************************************************
*						End of File
************************************************************/
