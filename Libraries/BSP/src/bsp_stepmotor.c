#include "bsp_stepmotor.h"
#include "stepMotorlist.h"
#include <string.h>
#include <stdbool.h>

#define GET_GPIO_PERIPH_ADDR(GPIOX)     RCC_AHB1Periph_##GPIOX
#define GET_ACTUAL_FREQUENCY()          ( motor->Public.param.freq_max - motor->Public.param.freq_min ) * \
                                        ( motor->Public.param.speed / 100 )

static void StepMotorTimerInit(void);
static bool stepTimerInit = false;              //步进电机定时器首轮初始化标志位
static stepMotorListType *MotorTaskList;        //电机任务调度链表，由定时器唤起
static uint16_t stepMotorTaskFin[16];           //任务结束链表释放名单
static uint16_t stepMotorTaskFinptr = 0;        //释放名单计数器

/**
 * @breif:  步进电机初始化
 * @param:  步进电机对象
 * @retval: None
*/
void StepMotor_Init(stepMotorClass * motor,stepMotorInitTypeDef * stepMotorParam)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*如果未初始化过定时器，则初始化定时器*/
    if(!stepTimerInit)
    {
        StepMotorTimerInit();
        stepTimerInit = true;
    }

    /*配置时钟*/
    switch(stepMotorParam->IO.COM_PORT)
    {
        case MotorIOA:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);motor->Private.COM_GPIO_PORT = GPIOA;break;
        case MotorIOB:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);motor->Private.COM_GPIO_PORT = GPIOB;break;
        case MotorIOC:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);motor->Private.COM_GPIO_PORT = GPIOC;break;
        case MotorIOD:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);motor->Private.COM_GPIO_PORT = GPIOD;break;
        case MotorIOE:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);motor->Private.COM_GPIO_PORT = GPIOE;break;
        case MotorIOF:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);motor->Private.COM_GPIO_PORT = GPIOF;break;
        case MotorIOG:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);motor->Private.COM_GPIO_PORT = GPIOG;break;
    }

        switch(stepMotorParam->IO.DIR_PORT)
    {
        case MotorIOA:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOA;break;
        case MotorIOB:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOB;break;
        case MotorIOC:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOC;break;
        case MotorIOD:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOD;break;
        case MotorIOE:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOE;break;
        case MotorIOF:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOF;break;
        case MotorIOG:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOG;break;
    }

        switch(stepMotorParam->IO.ENA_PORT)
    {
        case MotorIOA:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOA;break;
        case MotorIOB:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOB;break;
        case MotorIOC:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOC;break;
        case MotorIOD:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOD;break;
        case MotorIOE:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOE;break;
        case MotorIOF:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOF;break;
        case MotorIOG:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOG;break;
    }

        switch(stepMotorParam->IO.STP_PORT)
    {
        case MotorIOA:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);motor->Private.STP_GPIO_PORT = GPIOA;break;
        case MotorIOB:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);motor->Private.STP_GPIO_PORT = GPIOB;break;
        case MotorIOC:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);motor->Private.STP_GPIO_PORT = GPIOC;break;
        case MotorIOD:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);motor->Private.STP_GPIO_PORT = GPIOD;break;
        case MotorIOE:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);motor->Private.STP_GPIO_PORT = GPIOE;break;
        case MotorIOF:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);motor->Private.STP_GPIO_PORT = GPIOF;break;
        case MotorIOG:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);motor->Private.STP_GPIO_PORT = GPIOG;break;
    }

    /*引脚参数配置*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

    /*公共引脚初始化*/
    motor->Public.IO.COM_PIN = stepMotorParam->IO.COM_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.COM_PIN;
    GPIO_Init(motor->Private.COM_GPIO_PORT,&GPIO_InitStruct);

    /*方向引脚初始化*/
    motor->Public.IO.DIR_PIN = stepMotorParam->IO.DIR_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.DIR_PIN;
    GPIO_Init(motor->Private.DIR_GPIO_PORT,&GPIO_InitStruct);
    
    /*使能引脚初始化*/
    motor->Public.IO.ENA_PIN = stepMotorParam->IO.ENA_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.ENA_PIN;
    GPIO_Init(motor->Private.ENA_GPIO_PORT,&GPIO_InitStruct);

    /*步进引脚初始化*/
    motor->Public.IO.STP_PIN = stepMotorParam->IO.STP_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.STP_PIN;
    GPIO_Init(motor->Private.STP_GPIO_PORT,&GPIO_InitStruct);

    /*参数拷贝*/
    motor->Public.IO.COM_PORT = stepMotorParam->IO.COM_PORT;
    motor->Public.IO.DIR_PORT = stepMotorParam->IO.DIR_PORT;
    motor->Public.IO.ENA_PORT = stepMotorParam->IO.ENA_PORT;
    motor->Public.IO.STP_PORT = stepMotorParam->IO.STP_PORT;

    motor->Public.param.DEG = stepMotorParam->param.DEG;
    motor->Public.param.dir = stepMotorParam->param.dir;
    motor->Public.param.division = stepMotorParam->param.division;
    motor->Public.param.freq_max = stepMotorParam->param.freq_max;
    motor->Public.param.freq_min = stepMotorParam->param.freq_min;
    motor->Public.param.speed = stepMotorParam->param.speed;

    /*确定默认状态为停止*/
    StepMotor_Cmd(motor,DISABLE);

}

/**
 * @breif:  步进电机定时器初始化
*/
static void StepMotorTimerInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(STEP_MOTOR_TIM_RCC,ENABLE);
 
    TIM_TimeBaseInitStruct.TIM_ClockDivision = STEP_MOTOR_TIM_CLK_DIV;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = (STEP_MOTOR_TIM_PERIOD - 1);
    TIM_TimeBaseInitStruct.TIM_Prescaler = (STEP_MOTOR_TIM_PRESCALER - 1);
    TIM_TimeBaseInit(STEP_MOTOR_TIM,&TIM_TimeBaseInitStruct);
    
    /*中断注册*/
	NVIC_InitStruct.NVIC_IRQChannel=STEP_MOTOR_TIM_IRQ_REGISTER;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_Init(&NVIC_InitStruct);
    TIM_ClearITPendingBit(STEP_MOTOR_TIM,TIM_IT_Update);
    TIM_ITConfig(STEP_MOTOR_TIM,TIM_IT_Update,ENABLE);

    /*生成链表统一管理各个电机的脉冲输出*/
    MotorTaskList = stepMotorList_Init();
}

/**
 * @breif:  步进电机使能控制
 * @param:  motor       电机对象
 * @param:  NewState    新的使能状态
 * @retval: None
 * @note:   失能后的电机将失去力矩，表现为转轴松动
*/
void StepMotor_Cmd(stepMotorClass * motor, FunctionalState NewState)
{
    if(NewState)
    {
        motor->Private.ENA_GPIO_PORT->BSRRL = motor->Public.IO.ENA_PIN;
        motor->Private.COM_GPIO_PORT->BSRRL = motor->Public.IO.COM_PIN;
        motor->Private.status = stop;
    }
    else
    {
        motor->Private.ENA_GPIO_PORT->BSRRH = motor->Public.IO.ENA_PIN;
        motor->Private.COM_GPIO_PORT->BSRRL = motor->Public.IO.COM_PIN;
        motor->Private.status = disable;
    }
}

/**
 * @breif:  步进电机刹车控制
 * @param:  motor       电机对象
 * @param:  NewState    新的使能状态
 * @retval: None
 * @note:   刹车时电机依然获得力矩，表现为转轴依然有力
*/
void StepMotor_Brake(stepMotorClass * motor, FunctionalState NewState)
{
    if(NewState)
    {
        motor->Private.ENA_GPIO_PORT->BSRRH = motor->Public.IO.ENA_PIN;
        motor->Private.COM_GPIO_PORT->BSRRH = motor->Public.IO.COM_PIN;
    }
    else
    {
        motor->Private.ENA_GPIO_PORT->BSRRL = motor->Public.IO.ENA_PIN;
        motor->Private.COM_GPIO_PORT->BSRRL = motor->Public.IO.COM_PIN;
    }
    motor->Private.status = stop;
}

/**
 * @brief:  步进电机依照步距角前进
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  xStep                   前进步数
 * @retval: None
*/
void stepMotor_StepFoward(stepMotorClass * motor, uint16_t xStep)
{
    if(motor->Private.status == stop)
    {
        motor->Private.Hz = GET_ACTUAL_FREQUENCY();
        motor->Private.nopTime_us = (500000/motor->Private.Hz);
        motor->Private.pulse = (xStep * motor->Public.param.division * 2);
        motor->Private.pulse_cnt = 0;
        motor->Private.delay_cnt = 0;

        stepMotorList_TailPush(MotorTaskList,motor);        //将要用于调度的电机对象插入调度表中
    }
}

/**
 * @brief:  步进电机按照前进指定角度
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  angle                   控制步进电机要前进的角度
 * @retval: None
*/
void stepMotor_AngleFoward(stepMotorClass * motor,float angle)
{
    if(motor->Private.status == stop)
    {
        motor->Private.Hz = GET_ACTUAL_FREQUENCY();
        motor->Private.nopTime_us = (500000/motor->Private.Hz);
        motor->Private.pulse =   (angle/360.0f)*(360.0f/motor->Public.param.DEG)*motor->Public.param.division * 2;
        motor->Private.pulse_cnt = 0;
        motor->Private.delay_cnt = 0;

        stepMotorList_TailPush(MotorTaskList,motor);        //将要用于调度的电机对象插入调度表中
    }

}

/**
 * @brief:  步进电机方向引脚更新
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  dir                     新的前进方向
*/
void stepMotor_DirChange(stepMotorClass * motor,stepMotorDir dir)
{
    if( dir == dir_neg ) 
    {
        motor->Private.DIR_GPIO_PORT->BSRRL = motor->Public.IO.DIR_PIN;
        motor->Public.param.dir = dir_neg;
    }
    else
    {
        motor->Private.DIR_GPIO_PORT->BSRRH = motor->Public.IO.DIR_PIN;
        motor->Public.param.dir = dir_pos;
    }
}

/**
 * @brief:  步进电机速度更新
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  newSpeed                新的速度百分比
*/
void stepMotor_SpeedChange(stepMotorClass * motor,float newSpeed)
{
    motor->Private.Hz = GET_ACTUAL_FREQUENCY();
    motor->Private.nopTime_us = (500000/motor->Private.Hz);
    motor->Public.param.speed = newSpeed;

}

#if STEP_MOTOR_PULSE_BY_DELAY == true
/**
 * @brief:  步进电机依照步距角前进(非常精确，但占用CPU资源)
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  xStep                   前进步数
 * @retval: None
*/
void stepMotorStepFoward(stepMotorClass * motor, uint16_t xStep)
{
    uint16_t ctr;

    /*根据速度占比得到实际频率*/
    uint32_t Hz = GET_ACTUAL_FREQUENCY();
    /*根据实际频率换算延时时间*/
    uint32_t nop_time = (500000/Hz);

    for(ctr = 0; ctr < (xStep * motor->Public.param.division * 2);ctr ++)
    {
        /*对步进引脚电平进行xStep个步距角的取反*/
        motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;
        delay_us(nop_time);
    }
}

#endif

/**
 * @brief:  步进电机链表迭代回调函数
 * @param:  motor:节点中包含的电机对象
 * @retval: None
*/
static void stepMotorTaskCallback(stepMotorClass * motor, uint16_t id)
{
    /*若该电机的脉冲个数未发送完*/
    if(motor->Private.pulse_cnt < motor->Private.pulse)
    {
        /*电机处于运行状态*/
        motor->Private.status = run;

        /*若电机所需延时计数未到指定时长*/
        if(motor->Private.delay_cnt < motor->Private.nopTime_us)
        {
            motor->Private.delay_cnt += STEP_MOTOR_TIM_PERIOD;  //继续延时
        }
        else
        {
            motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;  //对该引脚进行取反操作
            motor->Private.pulse_cnt ++;                                    //脉冲发送个数++
            motor->Private.delay_cnt = 0;                                   //清空延时，进入下一个延时周期
        }
    }
    else
    {
        /*指定个数脉冲发送完毕，电机不再运行*/
        motor->Private.status = stop;

        /*将当前对象在链表中的id填入释放名单*/
        stepMotorTaskFin[stepMotorTaskFinptr] = id;
        stepMotorTaskFinptr ++ ;

    }

}

/**
 * @brief:  步进电机中断事件回调函数
*/
void STEP_MOTOR_TIM_EVENTHANDLER(void)
{
    uint16_t tmp;

    if(TIM_GetITStatus(TIM2,TIM_IT_Update))
    {
        /*对各电机进行迭代处理*/
        stepMotorList_iterator(MotorTaskList,ALL,stepMotorTaskCallback);

        /*释放已经完成的任务*/
        for(tmp = 0;tmp < stepMotorTaskFinptr; tmp++)
        {
            stepMotorList_POPbyID(MotorTaskList,stepMotorTaskFin[tmp]);
            stepMotorTaskFinptr = 0;
        }

        /*如果任务数已经为0*/
        if(stepMotorList_iterator(MotorTaskList,ALL,NULL) == 0)
        {
            TIM_Cmd(STEP_MOTOR_TIM,DISABLE);                     //关闭定时器
        }

        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }

}