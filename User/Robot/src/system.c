#include "system.h"
#include "bsp_delay.h"
#include "bsp_serial.h"
#include "AssembleRobot.h"
#include "RF.h"
#include "relay.h"
#include "Key.h"
#include "turnplate.h"

uint8_t RandSeed = 0;//随机刻种子

/**
*@brief:	系统功能定时器初始化
*/
static void System_Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	Display_Logged("Init SYS_TIM...\n");
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	/*清除中断标志位*/
	TIM_ClearITPendingBit(SYSTEM_TIMER,TIM_IT_Update);

	/*开启定时器中断*/
	TIM_ITConfig(SYSTEM_TIMER,TIM_IT_Update,ENABLE);
	Display_Logged("SYS_TIM init done!\n");
}
/**
 * @brief:  系统初始化
*/
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    Display_Init();
    serialDebugInit();
    Robot_Init();
    RF_Init();
    Key_Init();
	System_Timer_Init();

	/*开启系统定时器*/
	TIM_Cmd(SYSTEM_TIMER,ENABLE);
	Display_Logged("SYS_TIM start!\n");
	delay_s(1);
	Display_Logged("System start in\n");
	Display_Logged("3..\n");
	delay_s(1);
	Display_Logged("2..\n");
	delay_s(1);
	Display_Logged("1..\n");
	delay_s(1);
	clr;
	LOG_row_ctr = 0;
	Display_Logged("AssembleRobot.\n");
}

/**
 *@brief:	系统定时器中断功能(5ms)
*/
void SYSTEM_TIMER_EVENTHANDLER(void)
{
	static uint16_t Times = 0;

	if(TIM_GetITStatus(SYSTEM_TIMER,TIM_IT_Update) == SET)
	{
		/*防止systick定时器因不明原因停止计数*/
		if(SysTick->VAL > 0)
		{
			SysTick->CTRL = 0x00000005;
		}

		/*随机刻种子*/
		( RandSeed > 1 ) ? RandSeed = 1 : RandSeed ++;
		/*按键状态更新*/
		Update_Keys_State(&KEY1);
		Update_Keys_State(&KEY2);
		Update_Keys_State(&KEY3);
		Update_Keys_State(&KEY4);

		if( (Times ++) >= OPTION_RESERVED_X5MS)
		{
			Times = 0;
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
