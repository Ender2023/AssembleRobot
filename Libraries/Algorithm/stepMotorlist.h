#ifndef __STEPMOTORLIST_H
#define __STEPMOTORLIST_H

#include "bsp_stepmotor.h"

#define ALL 65535    

/*������������*/
typedef struct node
{
    stepMotorClass * pStepMotor;            //�����صĵ������ָ��
    struct node * next;                     //ָ����һ���ڵ����ڵ�ַ
}stepMotorListType;

typedef void (*callbackfunc) (stepMotorClass*,uint16_t); //�ص�ָ������

stepMotorListType* stepMotorList_Init(void);
int stepMotorList_TailPush(stepMotorListType* pList,stepMotorClass* pNewMotor);
int stepMotorList_POPbyID(stepMotorListType* pList, uint16_t id);
int stepMotorList_POPbyInfo(stepMotorListType* pList, stepMotorClass* pMotor);
int stepMotorList_iterator(stepMotorListType* pList, uint16_t id, callbackfunc pfunc);
int stepMotorList_getbyID(stepMotorListType* pList, stepMotorClass* pMotor);
stepMotorClass* stepMotor_getbyInfo(stepMotorListType* pList, uint16_t id);
int stepMotorList_DeInit(stepMotorListType** pList);

#endif
