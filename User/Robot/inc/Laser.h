#ifndef __LASER_H
#define __LASER_H

#include "system.h"

#define LASER_PORT      GPIOG
#define LASER_PIN       GPIO_Pin_3

void Laser_Init(void);
void Laser_BlockDetect(void);
bool Laser_GetStatus(void);

#endif
