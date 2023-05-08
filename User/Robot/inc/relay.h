#ifndef __RELAY_H
#define __RELAY_H

#include "system.h"

#define RELAY_CTRL_RCC_PERIPH   RCC_APB2Periph_GPIOG
#define RELAY_CTRL_PORT			GPIOG
#define RELAY_CTRL_PIN			GPIO_Pin_15


void Relay_Init(void);
void Relay_actuation(void);
void Relay_Release(void);

#endif

/************************************************************
*						End of File
************************************************************/
