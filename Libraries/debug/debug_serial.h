#ifndef __DEBUG_SERIAL_H
#define __DEBUG_SERIAL_H

#include "debug.h"

#define TTYS_DEBUG                  UART5
#define TTYS_DEBUG_RCC              RCC_APB1Periph_UART5
#define TTYS_DEBUG_BUADRATE         115200

void serialDebugInit(void);

#endif
