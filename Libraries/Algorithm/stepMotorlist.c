#include "stepMotorlist.h"
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_ON        1
#define DEBUG_OFF       0
#define DEBUG           DEBUG_OFF 

/**
 * @brief:  ������������ʼ��
 * @retval: ��ʼ����õ�������ͷ���ָ��
 * @note:   ���������ͷ
*/
stepMotorListType* stepMotorList_Init(void)
{
    stepMotorListType* head = (stepMotorListType*) malloc(sizeof(stepMotorListType));   //����ռ�������ͷ
    head->next = NULL;
    head->pStepMotor = NULL;

    return head;
}

/**
 * @brief:  [����]�����������β��
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  pNewMotor       ������������ַ
 * @retval: ������Ϣ        0:�����˳�,-3:��ʾ�����ڵ������������NULL
*/
int stepMotorList_TailPush(stepMotorListType* pList,stepMotorClass* pNewMotor)
{
    /*����Ƿ��ڲ��������ڵ��������*/
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
 * @brief:  [����]�����������IDɾ��
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  id              Ҫɾ��������ڵ�id 
 * @retval: ������Ϣ        0:�����˳�,-1:����Ϊ��,-2�ڵ��±겻����,-3:��ʾ�����ڵ������������NULL
*/
int stepMotorList_POPbyID(stepMotorListType* pList, uint16_t id)
{
    uint16_t id_ctr = 0;
    stepMotorListType* tmp = NULL;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!pList) { return -3; }

    tmp = pList->next;

    /*����0�ڵ��Ƿ�Ϊ��*/
    if (tmp == NULL)
    {
        return -1;  //����Ϊ�գ�����Ԫ��
    }

    while (id_ctr < id)
    {
        /*�������δ��������Ѱ��*/
        if (tmp->next != NULL)
        {
            pList = pList->next;
            tmp = pList->next;
            id_ctr ++;
        }
        else
        {
            /*������Ҫ��ѯ�ı���������в�����*/
            return -2;
        }
    }

    /*����Ҫɾ���Ľڵ�,ָ����һ�ڵ�*/
    pList->next = tmp->next;
    free(tmp);

#if DEBUG == DEBUG_ON
    printf("ɾ����%d�ڵ�\n",id);
#endif

    return 0;
}

/**
 * @brief:  [����]�������������Ϣɾ��
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  pMotor          Ҫɾ��������ڵ��е���Ϣ
 * @retval: ������Ϣ        0:�����˳�,-1:����Ϊ��,-2�ڵ��±겻����,-3:��ʾ�����ڵ������������NULL
*/
int stepMotorList_POPbyInfo(stepMotorListType* pList, stepMotorClass* pMotor)
{
    uint16_t id_ctr = 0;
    stepMotorListType* tmp = NULL;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!pList) { return -3; }

    tmp = pList->next;

    /*����0�ڵ��Ƿ�Ϊ��*/
    if (tmp == NULL)
    {
        return -1;  //����Ϊ�գ�����Ԫ��
    }

    while (tmp->pStepMotor != pMotor)
    {
        /*�������δ��������Ѱ��*/
        if (tmp->next != NULL)
        {
            pList = pList->next;
            tmp = pList->next;
            id_ctr++;
        }
        else
        {
            /*������Ҫ��ѯ�ı���������в�����*/
            return -2;
        }
    }

    /*����Ҫɾ���Ľڵ�,ָ����һ�ڵ�*/
    pList->next = tmp->next;
    free(tmp);

#if DEBUG == DEBUG_ON
    printf("���ҵ�%#xλ�ڵ�%d�ڵ㲢ɾ��\n", pMotor,id_ctr);
#endif

    return 0;
}

/**
 * @brief:  [����]��������������
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  id              ��ѯ����ĩβ�±�
 * @retval: ������Ϣ         0~32767:�����˳�,�����������еĽڵ����
 *                          -1:����Ϊ��,-3:��ʾ�����ڵ������������NULL
 * @note:   ����iterator�����÷�
 *          retval = stepMotorList_iterator(list,ALL,NULL); �����ڲ�ѯ���нڵ�������ڵ�����洢��retval��
 *          stepMotorList_iterator(list,ALL,pfunc);         �����ڵ���������ȫ��ڵ㣬�ص�������pfunc�ṩ
 *          stepMotorList_iterator(list,id,pfunc);          �����ڵ���������0~id���ڵ㣬�ص�������pfunc�ṩ
*/
int stepMotorList_iterator(stepMotorListType* pList, uint16_t id, callbackfunc pfunc)
{
    uint16_t id_ctr = 0;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!pList) { return -3; }

#if DEBUG == DEBUG_ON
    printf("��ͷ:��ַ:%#x,��Ϣ:%#x,��һ�ڵ�%#x\n", pList, pList->pStepMotor, pList->next);
#endif

    /*����0�ڵ��Ƿ�Ϊ��*/
    if (pList->next == NULL)
    { 
       return -1;                   //����Ϊ�գ�����Ԫ��
    }

    /*�����0�ڵ㿪ʼ����*/
    pList = pList->next;
    while ( id_ctr < id )
    {

        /*����ص������Ƿ�ΪNULL*/
        if(pfunc != NULL)
        {
            pfunc(pList->pStepMotor, id_ctr);   //���ڵ�����ͻص��������е���
        }
        

#if DEBUG == DEBUG_ON
        printf("��%d�ڵ�,��ַ:%#x,��Ϣ:%#x,��һ�ڵ�%#x\n",id_ctr,pList,pList->pStepMotor,pList->next);
#endif        

        pList = pList->next;

        /*���������һ�ڵ��Ƿ�Ϊ��*/
        if (pList == NULL) 
        { 
           return id_ctr + 1;       //��һ�ڵ�Ϊ�գ����������еĽڵ����Ϊid_ctr(�±�) + 1
        }
        id_ctr ++;
    }

#if DEBUG == DEBUG_ON
        printf("��%d�ڵ�,��ַ:%#x,��Ϣ:%#x,��һ�ڵ�%#x\n",id_ctr,pList,pList->pStepMotor,pList->next);
#endif   

    return id_ctr;
}

/**
 * @brief:  [����]��������������մ洢��Ϣ����id
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  pMotor          �����ҵ�����
 * @retval: ������Ϣ        -1:����Ϊ��,-2:���޴�Ԫ��,-3:��ʾ�����ڵ������������NULL,
                            0~32767:������Ԫ�����������е��±�
*/
int stepMotorList_getbyID(stepMotorListType* pList, stepMotorClass* pMotor)
{
    uint16_t id_ctr = 0;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!pList) { return -3; }

    /*����0�ڵ��Ƿ�Ϊ��*/
    if (pList->next == NULL)
    {
        return -1;           //����Ϊ�գ�����Ԫ��
    }

    /*�ӵ�0�ڵ㿪ʼ����*/
    pList = pList->next;
    /*����ǰ�ڵ��д洢����Ϣ������ҵ���Ϣ��ƥ��*/
    while (pList->pStepMotor != pMotor)
    {
        pList = pList->next;
        
        /*���������һ�ڵ��Ƿ�Ϊ��*/
        if (pList == NULL)
        {
            return -2;       //��һ�ڵ�Ϊ�գ���Ҫ���ҵ��±겻����
        }
        id_ctr ++;
    }

    return id_ctr;
}

/**
 * @brief:  [����]���������������id��ѯ�洢��Ϣ
 * @param:  pList           ����ͷ�ڵ��ַ
 * @param:  id              �����ҵĽڵ��±�
 * @retval: ���ر��ж�Ӧ�±�洢������,NULL�������ʧ��        
*/
stepMotorClass* stepMotor_getbyInfo(stepMotorListType* pList, uint16_t id)
{
    uint16_t id_ctr = 0;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!pList) { return NULL; }

    /*����0�ڵ��Ƿ�Ϊ��*/
    if (pList->next == NULL)
    {
        return NULL;           //����Ϊ�գ�����Ԫ��
    }

    /*�ӵ�0�ڵ㿪ʼ����*/
    pList = pList->next;
    /*����ǰ�ڵ���±겻����������*/
    while ( id_ctr < id )
    {
        pList = pList->next;

        /*���������һ�ڵ��Ƿ�Ϊ��*/
        if (pList == NULL)
        {
            return NULL;       //��һ�ڵ�Ϊ�գ���Ҫ���ҵ��±겻����
        }
        id_ctr++;
    }

    return pList->pStepMotor;
}

/**
 * @brief:  [����]���������������
 * @param:  pList           ����ͷ�ڵ��ַ
 * @retval: ������Ϣ        0:����,-3:��ʾ�����ڵ������������NULL
*/
int stepMotorList_DeInit(stepMotorListType** pList)
{
    stepMotorListType* tmp = NULL;

    /*����Ƿ��ڲ��������ڵ��������*/
    if (!(*pList)) { return -1; }

    tmp = (*pList)->next;

    /*������Ϊ�գ����ͷ������ͷ*/
    if(!tmp)
    {
        free((*pList));
        *pList = NULL;

        return 0;
    }

    /*�鿴��һ�ڵ����һ�ڵ��Ƿ񵽴�ĩβ*/
    while (tmp->next != NULL)
    {
        free((*pList));                //�ͷŵ�ǰ�ڵ�
        (*pList) = tmp;                //������һ�ڵ�
        tmp = (*pList)->next;          //�µ���һ�ڵ�
    }

    free((*pList));
    free(tmp);

    *pList = NULL;
    
    return 0;
}
