#ifndef __ASSEMBLEROBOT_H
#define __ASSEMBLEROBOT_H

#include "system.h"
#include "bsp_stepmotor.h"

/*机械手运动方式*/
typedef enum{rotation = 0,line}roboARM_Movetype;

/*机械手关节对象*/
typedef struct
{
    stepMotorClass *    motor;      //机械手臂占用的电机对象
    float               gear;       //该关节的齿数比
    roboARM_Movetype    MoveType;   //该关节的运动方式

}robot_ARM;

void roboARM_MotorBinding(robot_ARM * roboARM,stepMotorClass * motor,float gear,roboARM_Movetype moveType);
int roboARM_AngleExecute(robot_ARM * roboARM,float angle,stepMotorDir dir,float speed,uint8_t acceleratre);

#endif
