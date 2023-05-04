#include "stepMotorlist.h"
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_ON        1
#define DEBUG_OFF       0
#define DEBUG           DEBUG_OFF 

/**
 * @brief:  步进电机链表初始化
 * @retval: 初始化后得到的链表头结点指针
 * @note:   采用虚拟表头
*/
stepMotorListType* stepMotorList_Init(void)
{
    stepMotorListType* head = (stepMotorListType*) malloc(sizeof(stepMotorListType));   //分配空间用作表头
    head->next = NULL;
    head->pStepMotor = NULL;

    return head;
}

/**
 * @brief:  [方法]步进电机链表尾插
 * @param:  pList           链表头节点地址
 * @param:  pNewMotor       步进电机对象地址
 * @retval: 错误信息        0:正常退出,-3:表示不存在的链表操作对象NULL
*/
int stepMotorList_TailPush(stepMotorListType* pList,stepMotorClass* pNewMotor)
{
    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return -3; }

    while (pList->next != NULL)
    {
        pList = pList->next;
    }

    pList->next = (stepMotorListType*)malloc(sizeof(stepMotorListType));
    pList = pList->next;
    pList->pStepMotor = pNewMotor;
    pList->next = NULL;
    
    return 0;
}

/**
 * @brief:  [方法]步进电机链表按ID删除
 * @param:  pList           链表头节点地址
 * @param:  id              要删除的链表节点id 
 * @retval: 错误信息        0:正常退出,-1:链表为空,-2节点下标不存在,-3:表示不存在的链表操作对象NULL
*/
int stepMotorList_POPbyID(stepMotorListType* pList, uint16_t id)
{
    uint16_t id_ctr = 0;
    stepMotorListType* tmp = NULL;

    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return -3; }

    tmp = pList->next;

    /*检查第0节点是否为空*/
    if (tmp == NULL)
    {
        return -1;  //链表为空，不含元素
    }

    while (id_ctr < id)
    {
        /*如果链表未穷尽，则继续寻找*/
        if (tmp->next != NULL)
        {
            pList = pList->next;
            tmp = pList->next;
            id_ctr ++;
        }
        else
        {
            /*否则所要查询的标号在链表中不存在*/
            return -2;
        }
    }

    /*跳过要删除的节点,指向下一节点*/
    pList->next = tmp->next;
    free(tmp);

#if DEBUG == DEBUG_ON
    printf("删除第%d节点\n",id);
#endif

    return 0;
}

/**
 * @brief:  [方法]步进电机链表按信息删除
 * @param:  pList           链表头节点地址
 * @param:  pMotor          要删除的链表节点中的信息
 * @retval: 错误信息        0:正常退出,-1:链表为空,-2节点下标不存在,-3:表示不存在的链表操作对象NULL
*/
int stepMotorList_POPbyInfo(stepMotorListType* pList, stepMotorClass* pMotor)
{
    uint16_t id_ctr = 0;
    stepMotorListType* tmp = NULL;

    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return -3; }

    tmp = pList->next;

    /*检查第0节点是否为空*/
    if (tmp == NULL)
    {
        return -1;  //链表为空，不含元素
    }

    while (tmp->pStepMotor != pMotor)
    {
        /*如果链表未穷尽，则继续寻找*/
        if (tmp->next != NULL)
        {
            pList = pList->next;
            tmp = pList->next;
            id_ctr++;
        }
        else
        {
            /*否则所要查询的标号在链表中不存在*/
            return -2;
        }
    }

    /*跳过要删除的节点,指向下一节点*/
    pList->next = tmp->next;
    free(tmp);

#if DEBUG == DEBUG_ON
    printf("查找到%#x位于第%d节点并删除\n", pMotor,id_ctr);
#endif

    return 0;
}

/**
 * @brief:  [方法]步进电机链表遍历
 * @param:  pList           链表头节点地址
 * @param:  id              查询到的末尾下标
 * @retval: 错误信息         0~32767:正常退出,并返回链表中的节点个数
 *                          -1:链表为空,-3:表示不存在的链表操作对象NULL
 * @note:   关于iterator几种用法
 *          retval = stepMotorList_iterator(list,ALL,NULL); 可用于查询表中节点个数，节点个数存储于retval中
 *          stepMotorList_iterator(list,ALL,pfunc);         可用于迭代链表中全体节点，回调方法由pfunc提供
 *          stepMotorList_iterator(list,id,pfunc);          可用于迭代链表中0~id个节点，回调方法由pfunc提供
*/
int stepMotorList_iterator(stepMotorListType* pList, uint16_t id, callbackfunc pfunc)
{
    uint16_t id_ctr = 0;

    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return -3; }

#if DEBUG == DEBUG_ON
    printf("表头:地址:%#x,信息:%#x,下一节点%#x\n", pList, pList->pStepMotor, pList->next);
#endif

    /*检查第0节点是否为空*/
    if (pList->next == NULL)
    { 
       return -1;                   //链表为空，不含元素
    }

    /*进入第0节点开始遍历*/
    pList = pList->next;
    while ( id_ctr < id )
    {

        /*检验回调函数是否为NULL*/
        if(pfunc != NULL)
        {
            pfunc(pList->pStepMotor, id_ctr);   //将节点对象送回调函数进行迭代
        }
        

#if DEBUG == DEBUG_ON
        printf("第%d节点,地址:%#x,信息:%#x,下一节点%#x\n",id_ctr,pList,pList->pStepMotor,pList->next);
#endif        

        pList = pList->next;

        /*检查进入的下一节点是否为空*/
        if (pList == NULL) 
        { 
           return id_ctr + 1;       //下一节点为空，返回链表中的节点个数为id_ctr(下标) + 1
        }
        id_ctr ++;
    }

#if DEBUG == DEBUG_ON
        printf("第%d节点,地址:%#x,信息:%#x,下一节点%#x\n",id_ctr,pList,pList->pStepMotor,pList->next);
#endif   

    return id_ctr;
}

/**
 * @brief:  [方法]步进电机链表依照存储信息查找id
 * @param:  pList           链表头节点地址
 * @param:  pMotor          待查找的内容
 * @retval: 错误信息        -1:链表为空,-2:查无此元素,-3:表示不存在的链表操作对象NULL,
                            0~32767:待查找元素所在链表中的下标
*/
int stepMotorList_getbyID(stepMotorListType* pList, stepMotorClass* pMotor)
{
    uint16_t id_ctr = 0;

    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return -3; }

    /*检查第0节点是否为空*/
    if (pList->next == NULL)
    {
        return -1;           //链表为空，不含元素
    }

    /*从第0节点开始查找*/
    pList = pList->next;
    /*若当前节点中存储的信息与待查找的信息不匹配*/
    while (pList->pStepMotor != pMotor)
    {
        pList = pList->next;
        
        /*检查进入的下一节点是否为空*/
        if (pList == NULL)
        {
            return -2;       //下一节点为空，则要查找的下标不存在
        }
        id_ctr ++;
    }

    return id_ctr;
}

/**
 * @brief:  [方法]步进电机链表依照id查询存储信息
 * @param:  pList           链表头节点地址
 * @param:  id              待查找的节点下标
 * @retval: 返回表中对应下标存储的内容,NULL代表查找失败        
*/
stepMotorClass* stepMotor_getbyInfo(stepMotorListType* pList, uint16_t id)
{
    uint16_t id_ctr = 0;

    /*检查是否在操作不存在的链表对象*/
    if (!pList) { return NULL; }

    /*检查第0节点是否为空*/
    if (pList->next == NULL)
    {
        return NULL;           //链表为空，不含元素
    }

    /*从第0节点开始查找*/
    pList = pList->next;
    /*若当前节点的下标不是所期望的*/
    while ( id_ctr < id )
    {
        pList = pList->next;

        /*检查进入的下一节点是否为空*/
        if (pList == NULL)
        {
            return NULL;       //下一节点为空，则要查找的下标不存在
        }
        id_ctr++;
    }

    return pList->pStepMotor;
}

/**
 * @brief:  [方法]步进电机链表析构
 * @param:  pList           链表头节点地址
 * @retval: 错误信息        0:正常,-3:表示不存在的链表操作对象NULL
*/
int stepMotorList_DeInit(stepMotorListType** pList)
{
    stepMotorListType* tmp = NULL;

    /*检查是否在操作不存在的链表对象*/
    if (!(*pList)) { return -1; }

    tmp = (*pList)->next;

    /*若表本身为空，则释放虚拟表头*/
    if(!tmp)
    {
        free((*pList));
        *pList = NULL;

        return 0;
    }

    /*查看下一节点的下一节点是否到达末尾*/
    while (tmp->next != NULL)
    {
        free((*pList));                //释放当前节点
        (*pList) = tmp;                //进入下一节点
        tmp = (*pList)->next;          //新的下一节点
    }

    free((*pList));
    free(tmp);

    *pList = NULL;
    
    return 0;
}

/************************************************************
*						End of File
************************************************************/
