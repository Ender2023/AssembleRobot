#include <string.h>
#include <stdbool.h>
#include "bsp_stepmotor.h"
#include "stepMotorlist.h"
#include "bsp_serial.h"
#include "bsp_delay.h"

#define GET_ACTUAL_FREQUENCY()          ( motor->Public.param.freq_max - motor->Public.param.freq_min ) * \
                                        ( motor->Public.param.speed / 100 )

/**
 * DECLARE
*/
static void StepMotorTimerInit(void);
static bool stepTimerInit = false;               //步进电机定时器首轮初始化标志位
static uint8_t stepMotor_ttySxInitflag = 0x00;   //步进电机串口首轮初始化标志位
/**
 * BIT7:    串口初始化标志位，若为1在,则本次初始化串口
 * BIT6:    RESERVED
 * BIT5:    RESERVED
 * BIT4:    ttyS5初始化记录标志位
 * BIT3:    ttyS4初始化记录标志位
 * BIT2:    ttyS3初始化记录标志位
 * BIT1:    ttyS2初始化记录标志位
 * BIT0:    ttyS1初始化记录标志位
*/

extern stepMotorListType *MotorTaskList;
extern uint16_t stepMotorTaskFin[16];
extern uint16_t stepMotorTaskFinptr;

/**
 * @breif:  步进电机初始化
 * @param:  步进电机对象
 * @retval: None
 * @note:   COM端在无光耦的情况下不能作为IO输出，否则会随电机的负载烧毁MCU!!!
*/
void StepMotor_Init(stepMotorClass * motor,stepMotorInitTypeDef * stepMotorParam)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    motor->Public.CTRL_MODE = stepMotorParam->CTRL_MODE;

    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        /*如果未初始化过定时器，则初始化定时器*/
        if(!stepTimerInit)
        {
            StepMotorTimerInit();
            stepTimerInit = true;
        }

        /*配置时钟*/
        switch(stepMotorParam->IO.COM_PORT)
        {
            case MotorIOX: break;
            case MotorIOA: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);motor->Private.COM_GPIO_PORT = GPIOA;break;
            case MotorIOB: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);motor->Private.COM_GPIO_PORT = GPIOB;break;
            case MotorIOC: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);motor->Private.COM_GPIO_PORT = GPIOC;break;
            case MotorIOD: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);motor->Private.COM_GPIO_PORT = GPIOD;break;
            case MotorIOE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);motor->Private.COM_GPIO_PORT = GPIOE;break;
            case MotorIOF: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);motor->Private.COM_GPIO_PORT = GPIOF;break;
            case MotorIOG: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);motor->Private.COM_GPIO_PORT = GPIOG;break;
        }

            switch(stepMotorParam->IO.DIR_PORT)
        {
            case MotorIOA: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOA;break;
            case MotorIOB: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOB;break;
            case MotorIOC: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOC;break;
            case MotorIOD: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOD;break;
            case MotorIOE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOE;break;
            case MotorIOF: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOF;break;
            case MotorIOG: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);motor->Private.DIR_GPIO_PORT = GPIOG;break;
        }

            switch(stepMotorParam->IO.ENA_PORT)
        {
            case MotorIOA: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOA;break;
            case MotorIOB: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOB;break;
            case MotorIOC: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOC;break;
            case MotorIOD: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOD;break;
            case MotorIOE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOE;break;
            case MotorIOF: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOF;break;
            case MotorIOG: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);motor->Private.ENA_GPIO_PORT = GPIOG;break;
        }

            switch(stepMotorParam->IO.STP_PORT)
        {
            case MotorIOA: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);motor->Private.STP_GPIO_PORT = GPIOA;break;
            case MotorIOB: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);motor->Private.STP_GPIO_PORT = GPIOB;break;
            case MotorIOC: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);motor->Private.STP_GPIO_PORT = GPIOC;break;
            case MotorIOD: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);motor->Private.STP_GPIO_PORT = GPIOD;break;
            case MotorIOE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);motor->Private.STP_GPIO_PORT = GPIOE;break;
            case MotorIOF: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);motor->Private.STP_GPIO_PORT = GPIOF;break;
            case MotorIOG: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);motor->Private.STP_GPIO_PORT = GPIOG;break;
        }

        /*引脚参数配置*/
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

        /*公共引脚初始化*/
        if(motor->Public.IO.COM_PORT != MotorIOX)
        {
            motor->Public.IO.COM_PIN = stepMotorParam->IO.COM_PIN;
            GPIO_InitStruct.GPIO_Pin = motor->Public.IO.COM_PIN;
            GPIO_Init(motor->Private.COM_GPIO_PORT,&GPIO_InitStruct);
        }

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

        motor->Public.IO.ENA_Level = stepMotorParam->IO.ENA_Level;
        /*确定默认状态为停止*/
        StepMotor_Cmd(motor,DISABLE);

        /*参数拷贝*/
        motor->Public.IO.COM_PORT = stepMotorParam->IO.COM_PORT;
        motor->Public.IO.DIR_PORT = stepMotorParam->IO.DIR_PORT;
        motor->Public.IO.ENA_PORT = stepMotorParam->IO.ENA_PORT;
        motor->Public.IO.STP_PORT = stepMotorParam->IO.STP_PORT;
    }
    else if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        USART_InitTypeDef   USART_InitStruct;
        NVIC_InitTypeDef    NVIC_InitStruct;

        /*初始化串口*/
        switch(stepMotorParam->IO.ttySx)
        {
            case ttyS1:
            {
                motor->Private.ttySx = USART1;

                if( (stepMotor_ttySxInitflag & 0x01) == 0)
                {
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
                    motor->Private.ttySx_Tx_PORT = GPIOA;
                    motor->Private.ttySx_Tx_Pin = GPIO_Pin_9;
                    motor->Private.ttySx_Rx_PORT = GPIOA;
                    motor->Private.ttySx_Rx_Pin = GPIO_Pin_10;
                    motor->Private.ttyS_IRQ = USART1_IRQn;
                    stepMotor_ttySxInitflag |= 0x81;
                    break;
                }
                else
                {
                    break;
                }
            }
            case ttyS2:
            {
                motor->Private.ttySx = USART2;

                if( (stepMotor_ttySxInitflag & 0x02) == 0)
                {
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
                    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
                    motor->Private.ttySx_Tx_PORT = GPIOA;
                    motor->Private.ttySx_Tx_Pin = GPIO_Pin_2;
                    motor->Private.ttySx_Rx_PORT = GPIOA;
                    motor->Private.ttySx_Rx_Pin = GPIO_Pin_3;
                    motor->Private.ttyS_IRQ = USART2_IRQn;
                    stepMotor_ttySxInitflag |= 0x82;
                    break;
                }
                else
                {
                    break;
                }
            }
            case ttyS3:
            {
                motor->Private.ttySx = USART3;

                if( (stepMotor_ttySxInitflag & 0x04) == 0)
                {
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
                    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
                    motor->Private.ttySx_Tx_PORT = GPIOB;
                    motor->Private.ttySx_Tx_Pin = GPIO_Pin_10;
                    motor->Private.ttySx_Rx_PORT = GPIOB;
                    motor->Private.ttySx_Rx_Pin = GPIO_Pin_11;
                    motor->Private.ttyS_IRQ = USART3_IRQn;
                    stepMotor_ttySxInitflag |= 0x84;
                    break;
                }
                else
                {
                    break;
                }
            }
            case ttyS4:
            {
                motor->Private.ttySx = UART4;

                if( (stepMotor_ttySxInitflag & 0x08) == 0)
                {
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
                    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
                    motor->Private.ttySx_Tx_PORT = GPIOC;
                    motor->Private.ttySx_Tx_Pin = GPIO_Pin_10;
                    motor->Private.ttySx_Rx_PORT = GPIOC;
                    motor->Private.ttySx_Rx_Pin = GPIO_Pin_11;
                    motor->Private.ttyS_IRQ = UART4_IRQn;
                    stepMotor_ttySxInitflag |= 0x88;
                    break;
                }
                else
                {
                    break;
                }
            }
            case ttyS5:
            {
                motor->Private.ttySx = UART5;

                if( (stepMotor_ttySxInitflag & 0x10) == 0)
                {
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
                    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
                    motor->Private.ttySx_Tx_PORT = GPIOC;
                    motor->Private.ttySx_Tx_Pin = GPIO_Pin_12;
                    motor->Private.ttySx_Rx_PORT = GPIOD;
                    motor->Private.ttySx_Rx_Pin = GPIO_Pin_2;
                    motor->Private.ttyS_IRQ = UART5_IRQn;
                    stepMotor_ttySxInitflag |= 0x90;
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        /*是否需要初始化该串口*/
        if( (stepMotor_ttySxInitflag & 0x80) != 0 )
        {
            /*串口Tx端配置为复用推挽*/
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStruct.GPIO_Pin = motor->Private.ttySx_Tx_Pin;
            GPIO_Init(motor->Private.ttySx_Tx_PORT,&GPIO_InitStruct);

            /*串口Rx端配置为浮空输入*/
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_InitStruct.GPIO_Pin = motor->Private.ttySx_Rx_Pin;
            GPIO_Init(motor->Private.ttySx_Rx_PORT,&GPIO_InitStruct);

            /*串口初始化参数*/
            USART_InitStruct.USART_BaudRate = STEP_MOTOR_TTYSX_BUS_BUADRATE;
            USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_InitStruct.USART_Parity = USART_Parity_No;
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
            USART_Init(motor->Private.ttySx ,&USART_InitStruct);

            /*串口中断配置*/
            USART_ITConfig(motor->Private.ttySx,USART_IT_RXNE,ENABLE);

            NVIC_InitStruct.NVIC_IRQChannel = motor->Private.ttyS_IRQ;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
            NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
            NVIC_Init(&NVIC_InitStruct);

            /*使能串口*/
            USART_Cmd(motor->Private.ttySx ,ENABLE);

            /*清初始化标志位*/
            stepMotor_ttySxInitflag &= 0x7F;
        }

        /*参数拷贝*/
        motor->Public.IO.addr = stepMotorParam->IO.addr;

        /*初始化电机细分*/
        stepMotor_DivisionChange(motor,stepMotorParam->param.division);
    }
    
    /*拷贝通用控制参数*/
    motor->Public.param.DEG = stepMotorParam->param.DEG;
    motor->Public.param.dir = stepMotorParam->param.dir;
    motor->Public.param.division = stepMotorParam->param.division;
    motor->Public.param.freq_max = stepMotorParam->param.freq_max;
    motor->Public.param.freq_min = stepMotorParam->param.freq_min;
    motor->Public.param.speed = stepMotorParam->param.speed;
}

/**
 * @breif:  步进电机定时器初始化
*/
static void StepMotorTimerInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(STEP_MOTOR_TIM_RCC,ENABLE);

    /*时基配置*/
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
 * @retval: 0:正常退出              -1:引用了错误的控制模式
 * @note:   失能后的电机将失去力矩，表现为转轴松动
*/
int StepMotor_Cmd(stepMotorClass * motor, FunctionalState NewState)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        if(NewState == ENABLE)
        {
            if(motor->Public.IO.ENA_Level == HIGH_LEVEL)
            {
                motor->Private.ENA_GPIO_PORT->BSRR = motor->Public.IO.ENA_PIN;
            }
            else
            {
                motor->Private.ENA_GPIO_PORT->BRR = motor->Public.IO.ENA_PIN;
            }
            
            /*若不存在COM端,则不对COM端操作*/
            if(motor->Private.COM_GPIO_PORT != MotorIOX)
            {motor->Private.COM_GPIO_PORT->BSRR = motor->Public.IO.COM_PIN;}

            motor->Private.status = stop;
        }
        else
        {
            if(motor->Public.IO.ENA_Level == HIGH_LEVEL)
            {
                motor->Private.ENA_GPIO_PORT->BRR = motor->Public.IO.ENA_PIN;
            }
            else
            {
                motor->Private.ENA_GPIO_PORT->BSRR = motor->Public.IO.ENA_PIN;
            }
            
            /*若不存在COM端,则不对COM端操作*/
            if(motor->Private.COM_GPIO_PORT != MotorIOX)
            {motor->Private.COM_GPIO_PORT->BSRR = motor->Public.IO.COM_PIN;}
            
            motor->Private.status = disable;
        }

        return 0;
    }
    else if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        char buf[4] = {0xe0,0xf3,0x00,0x6b};

        buf[0] = motor->Public.IO.addr;
        if(NewState)
        {
            buf[2] = 0x01;
            motor->Private.status = stop;
        }
        else
        {
            buf[2] = 0x00;
            motor->Private.status = disable;
        }
        serial_SendHexArray(motor->Private.ttySx,buf,sizeof(buf));

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @breif:  步进电机刹车控制
 * @param:  motor       电机对象
 * @retval: 0:正常退出              -1:引用了错误的控制模式
 * @note:   刹车时电机依然获得力矩，表现为转轴依然有力
*/
int StepMotor_Brake(stepMotorClass * motor)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        motor->Private.ENA_GPIO_PORT->BRR = motor->Public.IO.ENA_PIN;
        motor->Private.COM_GPIO_PORT->BRR = motor->Public.IO.COM_PIN;
    }
    else if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        stepMotor_SpeedExecute(motor,motor->Public.param.dir,0,255);
    }
    else
    {
        return -1;
    }
    motor->Private.status = stop;

    return 0;

}

/**
 * @brief:  步进电机依照步距角前进(仅限硬件控制)
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  xStep                   前进步数
 * @retval: 0:正常退出              -1:引用了错误的控制模式
*/
int stepMotor_StepFoward(stepMotorClass * motor, uint32_t xStep)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        if(motor->Private.status == stop)
        {
            motor->Private.Hz = GET_ACTUAL_FREQUENCY();         //获取当前要到达的速度所实际对应的频率值
            motor->Private.nopTime_us = (500000/motor->Private.Hz);
            motor->Private.pulse = (xStep * motor->Public.param.division * 2);
            motor->Private.pulse_cnt = 0;
            motor->Private.delay_cnt = 0;

            stepMotorList_TailPush(MotorTaskList,motor);        //将要用于调度的电机对象插入调度表中
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  步进电机按原有方向前进指定角度(仅限硬件控制)
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  angle                   控制步进电机要前进的角度
 * @retval: 0:正常退出              -1:引用了错误的控制模式
*/
int stepMotor_AngleFoward(stepMotorClass * motor,float angle)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        /*如果任务正在运行，则不能被打断*/
        if(motor->Private.status == stop)
        {
            motor->Private.Hz = GET_ACTUAL_FREQUENCY();
            motor->Private.nopTime_us = (500000/motor->Private.Hz);
            motor->Private.pulse = (angle/360.0f)*(360.0f/motor->Public.param.DEG)*motor->Public.param.division * 2;
            motor->Private.pulse_cnt = 0;
            motor->Private.delay_cnt = 0;

            stepMotorList_TailPush(MotorTaskList,motor);        //将要用于调度的电机对象插入调度表中
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  步进电机方向引脚更新
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  newdir                  新的前进方向
 * @retval: 0:正常退出              -1:引用了错误的控制模式
*/
int stepMotor_DirChange(stepMotorClass * motor,stepMotorDir newdir)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        if(newdir == dir_neg) 
        {
            motor->Private.DIR_GPIO_PORT->BSRR = motor->Public.IO.DIR_PIN;
            motor->Public.param.dir = dir_neg;
        }
        else if(newdir == dir_pos)
        {
            motor->Private.DIR_GPIO_PORT->BRR = motor->Public.IO.DIR_PIN;
            motor->Public.param.dir = dir_pos;
        }
    }
    else if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        stepMotor_SpeedExecute(motor,newdir,motor->Public.param.speed,255);
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * @brief:  步进电机速度更新(仅限硬件控制)
 * @param:  stepMotorParamTypeDef   电机对象
 * @param:  newSpeed                新的速度百分比
 * @retval: 0:正常退出              -1:引用了错误的控制模式
*/
int stepMotor_SpeedChange(stepMotorClass * motor,float newSpeed)
{
    if(motor->Public.CTRL_MODE == HARD_CTRL)
    {
        motor->Private.Hz = GET_ACTUAL_FREQUENCY();
        motor->Private.nopTime_us = (500000/motor->Private.Hz);
        motor->Public.param.speed = newSpeed;
    }
    else
    {
        return -1;
    }

    return 0;
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
    uint32_t ctr,pulse;

    /*根据速度占比得到实际频率*/
    uint32_t Hz = GET_ACTUAL_FREQUENCY();
    /*根据实际频率换算延时时间*/
    uint32_t nop_time = (500000/Hz);
    /*换算需要发送的脉冲数*/
    pulse = xStep * motor->Public.param.division * 2;
    for(ctr = 0; ctr < pulse;ctr ++)
    {
        /*对步进引脚电平进行xStep个步距角的取反*/
        motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;
        delay_us(nop_time);
    }
}

#endif

/**
 * @breif:步进电机设置细分(仅限软件控制)
 * @param:  motor:          要操作的电机对象
 * @param:  newDivision:    要设置的新的电机细分值
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int stepMotor_DivisionChange(stepMotorClass * motor,uint8_t newDivision)
{
    if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        char buf[4] = {0xe0,0x84,0x00,0x6b};

        buf[0] = motor->Public.IO.addr;
        buf[2] = newDivision;

        serial_SendHexArray(motor->Private.ttySx,buf,sizeof(buf));
        delay_ms(5);
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * @breif:步进电机速度执行(仅限软件控制)
 * @param:  motor           要操作的电机对象
 * @param:  newDir          新的前进方向
 * @param:  newSpeed        新的速度百分比
 * @param:  acceleration    加速度,当值为255时会阶跃
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int stepMotor_SpeedExecute(stepMotorClass * motor,stepMotorDir newDir,float newSpeed,uint8_t acceleration)
{
    if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        char buf[6] = {0xe0,0xF6,0x00,0x00,0x00,0x6b};
        uint16_t speedVal = ( ( newSpeed / 100.0f ) * STEP_MOTOR_SOFT_SPEED_MAX );

        /*地址*/
        buf[0] = motor->Public.IO.addr;

        /*速度*/
        buf[2] = ( speedVal >> 8 ); //高8位
        buf[3] = ( speedVal );      //低8位

        /*分辨正反转*/
        if(newDir == dir_pos)
        {
            buf[2] &= 0x0F;         //正转
        }
        else
        {
            buf[2] &= 0x0F;
            buf[2] |= 0x10;         //反转
        }

        /*加速度*/
        buf[4] = acceleration;

        serial_SendHexArray(motor->Private.ttySx,buf,sizeof(buf));
        delay_ms(5);

        /*更新电机状态*/
        motor->Public.param.dir = newDir;
        motor->Public.param.speed = newSpeed;
        if( speedVal > 0)
        {
            motor->Private.status = run;
        }
        else
        {
            motor->Private.status = stop;
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @breif:步进电机按脉冲执行(仅限软件控制)
 * @param:  motor           要操作的电机对象
 * @param:  newDir          新的前进方向
 * @param:  newSpeed        新的速度百分比
 * @param:  acceleration    加速度,当值为255时会阶跃
 * @param:  pulse           电机要走的脉冲数
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int stepMotor_PulseExecute(stepMotorClass * motor, stepMotorDir newDir, float newSpeed,
uint8_t acceleration, uint32_t pulse)
{
    if(motor->Public.CTRL_MODE == SOFT_CTRL)
    {
        char buf[9] = {0xe0,0xFD,0x00,0x00,0x00,0x00,0x00,0x00,0x6b};
        uint16_t speedVal = ( ( newSpeed / 100.0f ) * STEP_MOTOR_SOFT_SPEED_MAX );

        /*地址*/
        buf[0] = motor->Public.IO.addr;

        /*速度*/
        buf[2] = ( speedVal >> 8 ); //高8位
        buf[3] = ( speedVal );      //低8位

        /*分辨正反转*/
        if(newDir == dir_pos)
        {
            buf[2] &= 0x0F;         //正转
        }
        else
        {
            buf[2] &= 0x0F;
            buf[2] |= 0x10;         //反转
        }

        /*加速度*/
        buf[4] = acceleration;
                  
        /*脉冲数*/
        buf[5] = ( pulse >> 16);
        buf[6] = ( pulse >> 8 );
        buf[7] = ( pulse );

        serial_SendHexArray(motor->Private.ttySx,buf,sizeof(buf));
        delay_ms(5);

        /*更新电机状态*/
        motor->Public.param.dir = newDir;
        motor->Public.param.speed = newSpeed;
        if( speedVal > 0)
        {
            motor->Private.status = run;
        }
        else
        {
            motor->Private.status = stop;
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @breif:步进电机位置执行(仅限软件控制)
 * @param:  motor           要操作的电机对象
 * @param:  newDir          新的前进方向
 * @param:  newSpeed        新的速度百分比
 * @param:  acceleration    加速度,当值为255时会阶跃
 * @param:  angle           要旋转过的角度
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int stepMotor_AngleExecute(stepMotorClass * motor, stepMotorDir newDir, float newSpeed,
                            uint8_t acceleration,float angle)
{
    if( motor->Public.CTRL_MODE == SOFT_CTRL )
    {
        uint32_t pulse = ( angle / 360 ) * ( ( 360 / motor->Public.param.DEG ) * motor->Public.param.division );

        stepMotor_PulseExecute(motor,newDir,newSpeed,acceleration,pulse);
    }

    else
    {
        return -1;
    }

    return 0;
}

/**
 * @brief:  步进电机执行调度
*/
__inline void steoMotor_TaskExcute(void)
{
    STEP_MOTOR_TIM->CR1 |= TIM_CR1_CEN;
}
