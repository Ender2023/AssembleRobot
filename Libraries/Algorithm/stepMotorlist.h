#ifndef __STEPMOTORLIST_H
#define __STEPMOTORLIST_H

#include "bsp_stepmotor.h"

#define ALL 65535    

/*步进电机单向表*/
typedef struct node
{
    stepMotorClass * pStepMotor;            //所挂载的电机对象指针
    struct node * next;                     //指向下一个节点所在地址
}stepMotorListType;

typedef void (*callbackfunc) (stepMotorClass*,uint16_t); //回调指针声明

stepMotorListType* stepMotorList_Init(void);
int stepMotorList_TailPush(stepMotorListType* pList,stepMotorClass* pNewMotor);
int stepMotorList_POPbyID(stepMotorListType* pList, uint16_t id);
int stepMotorList_POPbyInfo(stepMotorListType* pList, stepMotorClass* pMotor);
int stepMotorList_iterator(stepMotorListType* pList, uint16_t id, callbackfunc pfunc);
int stepMotorList_getbyID(stepMotorListType* pList, stepMotorClass* pMotor);
stepMotorClass* stepMotor_getbyInfo(stepMotorListType* pList, uint16_t id);
int stepMotorList_DeInit(stepMotorListType** pList);

#endif
