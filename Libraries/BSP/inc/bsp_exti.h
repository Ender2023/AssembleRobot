#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H

#include "system.h"

typedef enum{EXTI_IOA = 0,EXTI_IOB,EXTI_IOC,EXTI_IOD,EXTI_IOE,EXTI_IOF,EXTI_IOG}EXTI_GPIOType;

void EXTI_gpioRegister(EXTI_GPIOType EXTI_IO,uint32_t GPIO_Pin_x,EXTITrigger_TypeDef EXTI_Trigger);

#endif
