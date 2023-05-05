#ifndef __CAMERA_H
#define __CAMERA_H

#include "system.h"

#define TTYS_CAMERA                 USART1
#define TTYS_CAMERA_RCC             RCC_APB2Periph_USART1
#define TTYS_CAMERA_BUADRATE        9600

#define CAMERA_TTYSX_BUS_IRQHANDLER USART1_IRQHandler

void Camera_Init(void);
void Camera_stopSample(void);
void Camera_startSample(void);
bool Camera_ttyS_BusIDLE(void);

#endif