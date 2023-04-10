#ifndef __SYSTEM_H
#define __SYSTEM_h

#pragma anon_unions

/* Compiler */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 5000000) /* ARM Compiler \*/
#define __WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define __WEAK __weak
#elif defined(__MINGW32__) /* MINGW32 Compiler */
#define __WEAK
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define __WEAK __attribute__((weak))
#endif
/* default _WEAK */
#ifndef __WEAK
#define __WEAK
#endif

/**
 * INCLUDE
*/
#include "stm32f10x.h"

/**
 * DEBUG
*/
#define SYS_DEBUG_ON        1
#define SYS_DEBUG_OFF       0

#define SYS_DEBUG_SW        SYS_DEBUG_ON

#if     SYS_DEBUG_SW == SYS_DEBUG_ON
#define SYS_DEBUG(...)          do{\
                            serial_Printf(TTYS_DEBUG,"file:%s func:%s line:%d:",__FILE__,__func__,__LINE__);\
                            serial_Printf(TTYS_DEBUG,__VA_ARGS__);\
                            }while(0)

#else
__NOP;
#endif

/**
 * DEFINE
*/
#define ROBOARM_BIGARM_GEAR     20.0f
#define ROBOARM_SMALLARM_GEAR   16.25f
#define ROBOARM_ROTATION_GEAR   4.65f


/**
 * DECLARE
*/

void System_Init(void);


#endif
