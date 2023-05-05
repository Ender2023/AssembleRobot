#include "bsp_exti.h"

/**
 * @brief:  外部中断初始化
 * @param:  EXTI_IO         中断端口
 * @param:  GPIO_Pin_x      中断引脚号
 * @param:  EXTI_Trigger    触发方式
 * @retval: None
*/
void EXTI_gpioRegister(EXTI_GPIOType EXTI_IO,uint32_t GPIO_Pin_x,EXTITrigger_TypeDef EXTI_Trigger)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    GPIO_TypeDef *   GPIOX;

    /*时钟*/
    switch(EXTI_IO)
    {
        case EXTI_IOA: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOA;break;
        case EXTI_IOB: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOB;break;
        case EXTI_IOC: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOC;break;
        case EXTI_IOD: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOD;break;
        case EXTI_IOE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOE;break;
        case EXTI_IOF: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOF;break;
        case EXTI_IOG: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO,ENABLE);GPIOX = GPIOG;break;
    }

    /*引脚模式*/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_x;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    if(EXTI_Trigger == EXTI_Trigger_Rising)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    }
    else if(EXTI_Trigger == EXTI_Trigger_Falling)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    }
    else
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }

    GPIO_Init(GPIOX,&GPIO_InitStruct);

    /*EXTI配置*/
    EXTI_InitStruct.EXTI_Line = GPIO_Pin_x;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger;

    EXTI_Init(&EXTI_InitStruct);
    
    /*NVIC配置*/
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

    switch(GPIO_Pin_x)
    {
        case GPIO_Pin_0: NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource0);break;
        case GPIO_Pin_1: NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource1);break;
        case GPIO_Pin_2: NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource2);break;
        case GPIO_Pin_3: NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource3);break;
        case GPIO_Pin_4: NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource4);break;
        case GPIO_Pin_5: NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource5);break;
        case GPIO_Pin_6: NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource6);break;
        case GPIO_Pin_7: NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource7);break;
        case GPIO_Pin_8: NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource8);break;
        case GPIO_Pin_9: NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource9);break;
        case GPIO_Pin_10: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource10);break;
        case GPIO_Pin_11: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource11);break;
        case GPIO_Pin_12: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource12);break;
        case GPIO_Pin_13: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource13);break;
        case GPIO_Pin_14: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource14);break;
        case GPIO_Pin_15: NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;GPIO_EXTILineConfig(EXTI_IO,GPIO_PinSource15);break;
    }

    NVIC_Init(&NVIC_InitStruct);
}

/************************************************************
*						End of File
************************************************************/
