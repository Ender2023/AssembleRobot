#ifndef __TURNPLATE_H
#define __TURNPLATE_H

#include <stdbool.h>
#include "system.h"
#include "bsp_stepmotor.h"

#define TURNPLATE_GEAR      5.0f

/*转盘容器形状枚举*/
typedef enum{shapeSquare = 0,shapeCircle,shapeTriangle}Contentshape;

/*转盘空位对象定义*/
typedef struct
{
    bool                    isEmpty;        //是否已满
    float                   angle;          //所处角度
    Contentshape            shape;          //形状如何
}TurnplateContentType;

/*转盘对象定义*/
typedef struct
{
    stepMotorClass *        motor;          //转盘占用的电机对象
    TurnplateContentType    contentPool[8]; //容器池 
    float                   gear;           //转盘齿数比
    float                   angle;          //转盘当前角度值

}TurnplateType;

extern TurnplateType turnplate;

void Turnplate_Init(void);
void Turnplate_Cmd(FunctionalState NewState);
void Turnplate_DirChange(stepMotorDir dir);
void Turnplate_toAngle(float angle);
int Turnplate_toContent(Contentshape shape);

#endif

/************************************************************
*						End of File
************************************************************/
