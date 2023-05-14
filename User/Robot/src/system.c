#include "system.h"
#include "bsp_delay.h"
#include "debug.h"
#include "AssembleRobot.h"
#include "RF.h"
#include "relay.h"
#include "Key.h"
#include "Camera.h"
#include "turnplate.h"
#include "Laser.h"
#include "task.h"
#include "UI.h"


bool 	SYSTEM_INIT_DONE = false;		//系统初始化完成标志位
uint8_t RandSeed = 0;					//随机刻种子

typedef struct
{
	int task_delay;						//任务时间分片
	void (*task)(void);					//任务回调指针
	int task_delayNow;					//当前时间分片
}OS_taskHandle;

OS_taskHandle taskHandle[OS_TASK_NUM];	//句柄声明

/**
*@brief:	系统功能定时器初始化
*/
static void SystemTimer_Init(void)
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

	/*启动系统定时器*/
	SystemTimer_Cmd(ENABLE);
	Display_Logged(LOG_RANK_OK,"SYS_TIM init done!\n");
}

/**
 * @brief:	任务初始化
*/
static void OS_TaskInit(void)
{
	/*任务0*/
	taskHandle[0].task = Camera_ttyS_CheckBusIDLE;
	taskHandle[0].task_delay = OS_TIME_PIECE_CAMERA;
	taskHandle[0].task_delayNow = taskHandle[0].task_delay;
	
	/*任务1*/
	taskHandle[1].task = Keys_StatusUpdate;
	taskHandle[1].task_delay = OS_TIME_PIECE_KEY;
	taskHandle[1].task_delayNow = taskHandle[1].task_delay;

	/*任务2*/
	taskHandle[2].task = Robot_TaskHandler;
	taskHandle[2].task_delay = OS_TIME_PIECE_ROBOT_TASK;
	taskHandle[2].task_delayNow = taskHandle[2].task_delay;


}

/**
 * @brief:	任务分片消耗
*/
void OS_TaskTimeReduce(void)
{
	for(int cnt = 0; cnt < OS_TASK_NUM; cnt ++)
	{
		taskHandle[cnt].task_delayNow --;
	}
}

/**
 * @brief:	系统调度器
*/
void OS_Scedule(void)
{
	for(int cnt = 0; cnt < OS_TASK_NUM ; cnt ++)
	{
		if(taskHandle[cnt].task_delayNow <= 0)
		{
			taskHandle[cnt].task();
			taskHandle[cnt].task_delayNow = taskHandle[cnt].task_delay;
		}
	}
}

/**
 * @brief:  系统初始化
*/
void System_Init(void)
{
	/*配置内核中断优先级分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
	/*显示器初始化*/
	Display_Init();
    
	/*射频初始化*/
	RF_Init();

	/*按键初始化*/
    Key_Init();

	/*机械手初始化*/
    Robot_Init();

	/*串口调试初始化*/
    serialDebugInit();

	/*摄像头初始化*/
	Camera_Init();

	/*继电器初始化*/
	Relay_Init();

	/*转盘初始化*/
	Turnplate_Init();

	/*激光探头初始化*/
	Laser_Init();

	/*开启系统定时器*/
	SystemTimer_Init();

	/*注入任务调度表*/
	OS_TaskInit();
	
	/*等待摄像头应答*/
	Display_Logged(LOG_RANK_WARNNING,"[-]Cam:waiting ack...\n");
    while(Camera_getStatus() == false)
	{
		static uint8_t state = 0;

		Camera_ttyS_CheckBusIDLE();
		if(Camera_getStatus()){break;}
		LCD_Set_Printfmt(0,LCD_STATUS.BIT.fmt_y - 12,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);

		switch(state)
		{
			case 0: {LCD_Printf("[\\]Cam:waiting ack\n");state = 1;break;}
			case 1: {LCD_Printf("[|]Cam:waiting ack\n");state = 2;break;}
			case 2: {LCD_Printf("[/]Cam:waiting ack\n");state = 3;break;}
			case 3: {LCD_Printf("[-]Cam:waiting ack\n");state = 0;break;}
		}
		delay_ms(500);
	}

	Display_Logged(LOG_RANK_OK,"Cam init done!\n");
	delay_ms(500);
	
	/*建立UI界面*/
	UI_Setup();
	
	/*等待机械手高度校准信息*/
	if( robot_HeightInit == false )
	{
		Display_Logged(LOG_RANK_WARNNING,"[-]Robot:waiting calibrate\n");
	}
    
	while( robot_HeightInit == false )
	{
		static uint8_t state = 0;

		if(robot_HeightInit){break;}
		LCD_Set_Printfmt(0,LCD_STATUS.BIT.fmt_y - 12,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);

		switch(state)
		{
			case 0: {LCD_Printf("[\\]Robot:calibrating\n");state = 1;break;}
			case 1: {LCD_Printf("[|]Robot:calibrating\n");state = 2;break;}
			case 2: {LCD_Printf("[/]Robot:calibrating\n");state = 3;break;}
			case 3: {LCD_Printf("[-]Robot:calibrating\n");state = 0;break;}
		}
		delay_ms(500);
	}
	
	Display_Logged(LOG_RANK_OK,"robot Init done!\n");

	/*释放按键*/
	Keys_free(&KEY1);
	Keys_free(&KEY2);
	Keys_free(&KEY3);
	Keys_free(&KEY4);
	
	Display_Logged(LOG_RANK_OK,"System init done!\n");
	delay_ms(500);

	/*开始任务调度*/
	SYSTEM_INIT_DONE = true;
	clr;

	/*进入主界面*/
	UI_ShowPage(&homePage);
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

/**
 * @brief:系统定时器开关
 * @param:	NewState:
 * 			ENABLE:开启系统定时器
 * 			DISABLE:关闭系统定时器
 * @retval:None
*/
void SystemTimer_Cmd(FunctionalState NewState)
{
	if(NewState)
	{
		SYSTEM_TIMER->CR1 |= TIM_CR1_CEN;
		Display_Logged(LOG_RANK_WARNNING,"SYS_TIM start!\n");
	}
	else
	{
		SYSTEM_TIMER->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
		Display_Logged(LOG_RANK_WARNNING,"SYS_TIM stop!\n");
	}
}

/**
 *@brief:	系统定时器中断功能(5ms)
*/
void SYSTEM_TIMER_EVENTHANDLER(void)
{
	if(TIM_GetITStatus(SYSTEM_TIMER,TIM_IT_Update) == SET)
	{
		/*防止systick定时器因不明原因停止计数*/
		if(SysTick->VAL > 0)
		{
			SysTick->CTRL = 0x00000005;
		}

		/*随机刻种子*/
		( RandSeed > 100 ) ? RandSeed = 1 : RandSeed ++;

		if( SYSTEM_INIT_DONE == true )
		{
			/*激光探头物块检测*/
			Laser_BlockDetect();

			/*减少各任务时间片*/
			OS_TaskTimeReduce();
		}

		TIM_ClearITPendingBit(SYSTEM_TIMER,TIM_IT_Update);
	}
}

/************************************************************
*						End of File
************************************************************/
