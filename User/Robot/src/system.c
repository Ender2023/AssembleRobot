#include "system.h"
#include "bsp_delay.h"
#include "debug.h"
#include "AssembleRobot.h"
#include "RF.h"
#include "relay.h"
#include "Key.h"
#include "Camera.h"
#include "turnplate.h"
#include "UI.h"

bool SYSTEM_INIT_DONE = false;
uint8_t RandSeed = 0;//随机刻种子

/**
*@brief:	系统功能定时器初始化
*/
static void System_Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	Display_Logged(LOG_RANK_INFO,"Init SYS_TIM...\n");
	RCC_APB1PeriphClockCmd(SYSTEM_TIMER_USE_PERIPH,ENABLE);

	/*定时器时基配置*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision = SYSTEM_TIMER_CLK_DIV;//APB1->72MHz
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = SYSTEM_TIMER_PRESCALER - 1;
	TIM_TimeBaseInitStructure.TIM_Period = SYSTEM_TIMER_PERIOD - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(SYSTEM_TIMER,&TIM_TimeBaseInitStructure);

	/*配置中断向量表*/
	NVIC_InitStructure.NVIC_IRQChannel = SYSTEM_TIMER_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	/*清除中断标志位*/
	TIM_ClearITPendingBit(SYSTEM_TIMER,TIM_IT_Update);

	/*开启定时器中断*/
	TIM_ITConfig(SYSTEM_TIMER,TIM_IT_Update,ENABLE);
	Display_Logged(LOG_RANK_OK,"SYS_TIM init done!\n");
}
/**
 * @brief:  系统初始化
*/
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    Display_Init();
    RF_Init();
    Key_Init();
    Robot_Init();
    serialDebugInit();
	Camera_Init();

	/*开启系统定时器*/
	System_Timer_Init();
	TIM_Cmd(SYSTEM_TIMER,ENABLE);
	Display_Logged(LOG_RANK_INFO,"SYS_TIM start!\n");
	
	/*等待摄像头应答*/
	Display_Logged(LOG_RANK_WARNNING,"[-]Cam:waiting ack\n");
    while(Camera_getStatus() == false)
	{
		LCD_ShowChar(24,LCD_STATUS.BIT.fmt_y - 12,'\\',LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
		delay_ms(500);
		if(Camera_getStatus()){break;}
		LCD_ShowChar(24,LCD_STATUS.BIT.fmt_y - 12,'|',LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
		delay_ms(500);
		if(Camera_getStatus()){break;}
		LCD_ShowChar(24,LCD_STATUS.BIT.fmt_y - 12,'/',LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
		delay_ms(500);
		if(Camera_getStatus()){break;}	
		LCD_ShowChar(24,LCD_STATUS.BIT.fmt_y - 12,'-',LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
		delay_ms(500);
		if(Camera_getStatus()){break;}
	}


	Display_Logged(LOG_RANK_OK,"Cam init done!\n");
	delay_ms(500);
	UI_Setup();
	Keys_free(&KEY1);
	Keys_free(&KEY2);
	Keys_free(&KEY3);
	Keys_free(&KEY4);
	Display_Logged(LOG_RANK_OK,"System init done!\n");
	delay_ms(500);
	SYSTEM_INIT_DONE = true;
	clr;
	/*进入主界面*/
	UI_ShowPage(&homePage);
}

/**
 *@brief:	系统定时器中断功能(5ms)
*/
void SYSTEM_TIMER_EVENTHANDLER(void)
{
	static uint32_t Times = 0;

	if(TIM_GetITStatus(SYSTEM_TIMER,TIM_IT_Update) == SET)
	{
		/*防止systick定时器因不明原因停止计数*/
		if(SysTick->VAL > 0)
		{
			SysTick->CTRL = 0x00000005;
		}

		/*随机刻种子*/
		( RandSeed > 1 ) ? RandSeed = 1 : RandSeed ++;

		/*摄像头消息处理*/
		if( (Times % SYSTEM_TIMER_CAMERA ) == 0)
		{
			Camera_ttyS_BusIDLE();
		}

		/*按键事件处理*/
		if( (Times % OPTION_RESERVED_X5MS ) == 0)
		{
			Update_Keys_State(&KEY1);
			Update_Keys_State(&KEY2);
			Update_Keys_State(&KEY3);
			Update_Keys_State(&KEY4);
		}

		TIM_ClearITPendingBit(SYSTEM_TIMER,TIM_IT_Update);
	}
}

/**
 *@brief:	断言错误
 *@code:
 *		-1:刻度线大于限制
 *		-2:信号源数目大于限制
 *		-3:调用了未初始化的图表
*/
void ASSERT_ERR(void * obj,int8_t code)
{
	clr;
	LCD_Set_Printfmt(0,80,LCD_COLOR_WHITE,LCD_COLOR_BLACK,24,true);
	LCD_Printf("Illegal Obj called at %#x!!!\nCode:%d\n",obj,code);
	while(1);
}

/************************************************************
*						End of File
************************************************************/
