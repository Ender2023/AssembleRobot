#ifndef __TASK_H
#define __TASK_H

#include "system.h"

#define MAX_STAGE_SIZE  16

typedef enum{stage_begin = 0,stage1,stage2,stage3,stage4,stage5,stage6,stage7,stage8,stage9,stage10,stage_end,stage_sleep}enum_TaskStage;
extern enum_TaskStage taskPreStage;
extern enum_TaskStage taskStage;


void Robot_TaskInit(void);
void Robot_TaskHandler(void);


#endif
