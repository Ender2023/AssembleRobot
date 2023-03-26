#include "bsp_stepmotor.h"
#include "stepMotorlist.h"
#include <string.h>
#include <stdbool.h>

#define GET_GPIO_PERIPH_ADDR(GPIOX)     RCC_AHB1Periph_##GPIOX
#define GET_ACTUAL_FREQUENCY()          ( motor->Public.param.freq_max - motor->Public.param.freq_min ) * \
                                        ( motor->Public.param.speed / 100 )

static void StepMotorTimerInit(void);
static bool stepTimerInit = false;              //���������ʱ�����ֳ�ʼ����־λ
static stepMotorListType *MotorTaskList;        //���������������ɶ�ʱ������
static uint16_t stepMotorTaskFin[16];           //������������ͷ�����
static uint16_t stepMotorTaskFinptr = 0;        //�ͷ�����������

/**
 * @breif:  ���������ʼ��
 * @param:  �����������
 * @retval: None
*/
void StepMotor_Init(stepMotorClass * motor,stepMotorInitTypeDef * stepMotorParam)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*���δ��ʼ������ʱ�������ʼ����ʱ��*/
    if(!stepTimerInit)
    {
        StepMotorTimerInit();
        stepTimerInit = true;
    }

    /*����ʱ��*/
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

    /*���Ų�������*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

    /*�������ų�ʼ��*/
    motor->Public.IO.COM_PIN = stepMotorParam->IO.COM_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.COM_PIN;
    GPIO_Init(motor->Private.COM_GPIO_PORT,&GPIO_InitStruct);

    /*�������ų�ʼ��*/
    motor->Public.IO.DIR_PIN = stepMotorParam->IO.DIR_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.DIR_PIN;
    GPIO_Init(motor->Private.DIR_GPIO_PORT,&GPIO_InitStruct);
    
    /*ʹ�����ų�ʼ��*/
    motor->Public.IO.ENA_PIN = stepMotorParam->IO.ENA_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.ENA_PIN;
    GPIO_Init(motor->Private.ENA_GPIO_PORT,&GPIO_InitStruct);

    /*�������ų�ʼ��*/
    motor->Public.IO.STP_PIN = stepMotorParam->IO.STP_PIN;
    GPIO_InitStruct.GPIO_Pin = motor->Public.IO.STP_PIN;
    GPIO_Init(motor->Private.STP_GPIO_PORT,&GPIO_InitStruct);

    /*��������*/
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

    /*ȷ��Ĭ��״̬Ϊֹͣ*/
    StepMotor_Cmd(motor,DISABLE);

}

/**
 * @breif:  ���������ʱ����ʼ��
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
    
    /*�ж�ע��*/
	NVIC_InitStruct.NVIC_IRQChannel=STEP_MOTOR_TIM_IRQ_REGISTER;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_Init(&NVIC_InitStruct);
    TIM_ClearITPendingBit(STEP_MOTOR_TIM,TIM_IT_Update);
    TIM_ITConfig(STEP_MOTOR_TIM,TIM_IT_Update,ENABLE);

    /*��������ͳһ�������������������*/
    MotorTaskList = stepMotorList_Init();
}

/**
 * @breif:  �������ʹ�ܿ���
 * @param:  motor       �������
 * @param:  NewState    �µ�ʹ��״̬
 * @retval: None
 * @note:   ʧ�ܺ�ĵ����ʧȥ���أ�����Ϊת���ɶ�
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
 * @breif:  �������ɲ������
 * @param:  motor       �������
 * @param:  NewState    �µ�ʹ��״̬
 * @retval: None
 * @note:   ɲ��ʱ�����Ȼ������أ�����Ϊת����Ȼ����
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
 * @brief:  ����������ղ����ǰ��
 * @param:  stepMotorParamTypeDef   �������
 * @param:  xStep                   ǰ������
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

        stepMotorList_TailPush(MotorTaskList,motor);        //��Ҫ���ڵ��ȵĵ�����������ȱ���
    }
}

/**
 * @brief:  �����������ǰ��ָ���Ƕ�
 * @param:  stepMotorParamTypeDef   �������
 * @param:  angle                   ���Ʋ������Ҫǰ���ĽǶ�
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

        stepMotorList_TailPush(MotorTaskList,motor);        //��Ҫ���ڵ��ȵĵ�����������ȱ���
    }

}

/**
 * @brief:  ��������������Ÿ���
 * @param:  stepMotorParamTypeDef   �������
 * @param:  dir                     �µ�ǰ������
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
 * @brief:  ��������ٶȸ���
 * @param:  stepMotorParamTypeDef   �������
 * @param:  newSpeed                �µ��ٶȰٷֱ�
*/
void stepMotor_SpeedChange(stepMotorClass * motor,float newSpeed)
{
    motor->Private.Hz = GET_ACTUAL_FREQUENCY();
    motor->Private.nopTime_us = (500000/motor->Private.Hz);
    motor->Public.param.speed = newSpeed;

}

#if STEP_MOTOR_PULSE_BY_DELAY == true
/**
 * @brief:  ����������ղ����ǰ��(�ǳ���ȷ����ռ��CPU��Դ)
 * @param:  stepMotorParamTypeDef   �������
 * @param:  xStep                   ǰ������
 * @retval: None
*/
void stepMotorStepFoward(stepMotorClass * motor, uint16_t xStep)
{
    uint16_t ctr;

    /*�����ٶ�ռ�ȵõ�ʵ��Ƶ��*/
    uint32_t Hz = GET_ACTUAL_FREQUENCY();
    /*����ʵ��Ƶ�ʻ�����ʱʱ��*/
    uint32_t nop_time = (500000/Hz);

    for(ctr = 0; ctr < (xStep * motor->Public.param.division * 2);ctr ++)
    {
        /*�Բ������ŵ�ƽ����xStep������ǵ�ȡ��*/
        motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;
        delay_us(nop_time);
    }
}

#endif

/**
 * @brief:  ���������������ص�����
 * @param:  motor:�ڵ��а����ĵ������
 * @retval: None
*/
static void stepMotorTaskCallback(stepMotorClass * motor, uint16_t id)
{
    /*���õ�����������δ������*/
    if(motor->Private.pulse_cnt < motor->Private.pulse)
    {
        /*�����������״̬*/
        motor->Private.status = run;

        /*�����������ʱ����δ��ָ��ʱ��*/
        if(motor->Private.delay_cnt < motor->Private.nopTime_us)
        {
            motor->Private.delay_cnt += STEP_MOTOR_TIM_PERIOD;  //������ʱ
        }
        else
        {
            motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;  //�Ը����Ž���ȡ������
            motor->Private.pulse_cnt ++;                                    //���巢�͸���++
            motor->Private.delay_cnt = 0;                                   //�����ʱ��������һ����ʱ����
        }
    }
    else
    {
        /*ָ���������巢����ϣ������������*/
        motor->Private.status = stop;

        /*����ǰ�����������е�id�����ͷ�����*/
        stepMotorTaskFin[stepMotorTaskFinptr] = id;
        stepMotorTaskFinptr ++ ;

    }

}

/**
 * @brief:  ��������ж��¼��ص�����
*/
void STEP_MOTOR_TIM_EVENTHANDLER(void)
{
    uint16_t tmp;

    if(TIM_GetITStatus(TIM2,TIM_IT_Update))
    {
        /*�Ը�������е�������*/
        stepMotorList_iterator(MotorTaskList,ALL,stepMotorTaskCallback);

        /*�ͷ��Ѿ���ɵ�����*/
        for(tmp = 0;tmp < stepMotorTaskFinptr; tmp++)
        {
            stepMotorList_POPbyID(MotorTaskList,stepMotorTaskFin[tmp]);
            stepMotorTaskFinptr = 0;
        }

        /*����������Ѿ�Ϊ0*/
        if(stepMotorList_iterator(MotorTaskList,ALL,NULL) == 0)
        {
            TIM_Cmd(STEP_MOTOR_TIM,DISABLE);                     //�رն�ʱ��
        }

        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }

}