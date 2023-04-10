#include <string.h>
#include <stdbool.h>
#include "bsp_stepmotor.h"
#include "bsp_serial.h"
#include "stepMotorlist.h"

stepMotorListType *MotorTaskList;               //���������������ɶ�ʱ������
uint16_t stepMotorTaskFin[16];                  //������������ͷ�����
uint16_t stepMotorTaskFinptr = 0;               //�ͷ�����������

USART_DataFrameType stepMotorSerialDataFrame;   //���������������֡��ʽ

/**
 * @brief:  ���������������ص�����
 * @param:  motor:�ڵ��а����ĵ������
 * @retval: None
*/
static void stepMotorTaskCallback(stepMotorClass * motor, uint16_t id)
{
    /*���õ�����������δ������*/
    if(motor->Private.pulse_cnt < motor->Private.pulse)
    {
        /*�����������״̬*/
        motor->Private.status = run;

        /*�����������ʱ����δ��ָ��ʱ��*/
        if(motor->Private.delay_cnt < motor->Private.nopTime_us)
        {
            motor->Private.delay_cnt += STEP_MOTOR_TIM_PERIOD;  //������ʱ
        }
        else
        {
            motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;  //�Ը����Ž���ȡ������
            motor->Private.pulse_cnt ++;                                    //���巢�͸���++
            motor->Private.delay_cnt = 0;                                   //�����ʱ��������һ����ʱ����
        }
    }
    else
    {
        /*ָ���������巢����ϣ������������*/
        motor->Private.status = stop;

        /*����ǰ�����������е�id�����ͷ�����*/
        stepMotorTaskFin[stepMotorTaskFinptr] = id;
        stepMotorTaskFinptr ++ ;
    }
}

/**
 * @brief:  ��������ж��¼��ص�����
*/
void STEP_MOTOR_TIM_EVENTHANDLER(void)
{
    uint16_t tmp;

    if(TIM_GetITStatus(TIM2,TIM_IT_Update))
    {
        /*�Ը�������е�������*/
        stepMotorList_iterator(MotorTaskList,ALL,stepMotorTaskCallback);

        /*�ͷ��Ѿ���ɵ�����*/
        for(tmp = 0;tmp < stepMotorTaskFinptr; tmp++)
        {
            stepMotorList_POPbyID(MotorTaskList,stepMotorTaskFin[tmp]);
            stepMotorTaskFinptr = 0;
        }

        /*����������Ѿ�Ϊ0*/
        if(stepMotorList_iterator(MotorTaskList,ALL,NULL) == 0)
        {
            TIM_Cmd(STEP_MOTOR_TIM,DISABLE);                     //�رն�ʱ��
        }

        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }
}

/**
 * @brief:  ������������жϻص�����
*/
void STEP_MOTOR_TTYSX_BUS_IRQ_REGISTER(void)
{
    if(USART_GetITStatus(STEP_MOTOR_TTYSX_BUS,USART_IT_RXNE))
    {
        if(stepMotorSerialDataFrame.FrameBit.FrameCNT < SERIAL_MAXDATA_SIZE)
        stepMotorSerialDataFrame.RX_BUF[stepMotorSerialDataFrame.FrameBit.FrameCNT ++] = USART_ReceiveData(STEP_MOTOR_TTYSX_BUS);
        /*ͨ����USART_DR�Ķ��������Խ�USART_IT_RXNE��0*/
    }
    if(USART_GetITStatus(STEP_MOTOR_TTYSX_BUS,USART_IT_IDLE) == SET )
    {
        stepMotorSerialDataFrame.FrameBit.FrameFinishFlag = 1;		//�������߽������״̬�����ݽ������

        USART_ReceiveData(STEP_MOTOR_TTYSX_BUS);				    //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
        SYS_DEBUG("%s\n",stepMotorSerialDataFrame.RX_BUF);
    }
}
