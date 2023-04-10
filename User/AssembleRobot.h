#ifndef __ASSEMBLEROBOT_H
#define __ASSEMBLEROBOT_H

#include "system.h"
#include "bsp_stepmotor.h"

/*��е���˶���ʽ*/
typedef enum{rotation = 0,line}roboARM_Movetype;

/*��е�ֹؽڶ���*/
typedef struct
{
    stepMotorClass *    motor;      //��е�ֱ�ռ�õĵ������
    float               gear;       //�ùؽڵĳ�����
    roboARM_Movetype    MoveType;   //�ùؽڵ��˶���ʽ

}robot_ARM;

void roboARM_MotorBinding(robot_ARM * roboARM,stepMotorClass * motor,float gear,roboARM_Movetype moveType);
int roboARM_AngleExecute(robot_ARM * roboARM,float angle,stepMotorDir dir,float speed,uint8_t acceleratre);

#endif
