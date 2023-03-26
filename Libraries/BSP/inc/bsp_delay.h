#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "stm32f4xx.h"

#define CORE_FRECENCY_M  168

void delay_us(uint32_t xus);
void delay_ms(uint32_t xms);
void delay_s(uint32_t xs);

#endif
