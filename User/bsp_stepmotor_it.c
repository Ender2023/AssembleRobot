#include <string.h>
#include <stdbool.h>
#include "bsp_stepmotor.h"
#include "bsp_serial.h"
#include "stepMotorlist.h"

/**
 * DECLARE
*/
stepMotorListType *MotorTaskList;               //电机任务调度链表，由定时器唤起
uint16_t stepMotorTaskFin[16];                  //任务结束链表释放名单
uint16_t stepMotorTaskFinptr = 0;               //释放名单计数器

USART_DataFrameType stepMotorSerialDataFrame;   //步进电机串口数据帧格式

/**
 * @brief:  步进电机链表迭代回调函数
 * @param:  motor:节点中包含的电机对象
 * @retval: None
*/
static void stepMotorTaskCallback(stepMotorClass * motor, uint16_t id)
{
    /*若该电机的脉冲个数未发送完*/
    if(motor->Private.pulse_cnt < motor->Private.pulse)
    {
        /*电机处于运行状态*/
        motor->Private.status = run;

        /*若电机所需延时计数未到指定时长*/
        if(motor->Private.delay_cnt < motor->Private.nopTime_us)
        {
            motor->Private.delay_cnt += STEP_MOTOR_TIM_PERIOD;  //继续延时
        }
        else
        {
            motor->Private.STP_GPIO_PORT->ODR ^= motor->Public.IO.STP_PIN;  //对该引脚进行取反操作
            motor->Private.pulse_cnt ++;                                    //脉冲发送个数++
            motor->Private.delay_cnt = 0;                                   //清空延时，进入下一个延时周期
        }
    }
    else
    {
        /*指定个数脉冲发送完毕，电机不再运行*/
        motor->Private.status = stop;

        /*将当前对象在链表中的id填入释放名单*/
        stepMotorTaskFin[stepMotorTaskFinptr] = id;
        stepMotorTaskFinptr ++ ;
    }
}

/**
 * @brief:  步进电机中断事件回调函数
*/
void STEP_MOTOR_TIM_EVENTHANDLER(void)
{
    uint16_t tmp;

    /*查询是否由定时器溢出产生中断*/
    if(TIM_GetITStatus(STEP_MOTOR_TIM,TIM_IT_Update))
    {
        /*对各电机进行迭代处理*/
        stepMotorList_iterator(MotorTaskList,ALL,stepMotorTaskCallback);

        /*释放已经完成的任务*/
        for(tmp = 0;tmp < stepMotorTaskFinptr; tmp++)
        {
            stepMotorList_POPbyID(MotorTaskList,stepMotorTaskFin[tmp]);
            stepMotorTaskFinptr = 0;
        }

        /*如果任务数已经为0*/
        if(stepMotorList_iterator(MotorTaskList,ALL,NULL) == 0)
        {
            TIM_Cmd(STEP_MOTOR_TIM,DISABLE);                     //关闭定时器
        }

        /*清中断标志位*/
        TIM_ClearITPendingBit(STEP_MOTOR_TIM,TIM_IT_Update);
    }
}

/**
 * @brief:  步进电机串口中断回调函数
*/
void STEP_MOTOR_TTYSX_BUS_IRQ_REGISTER(void)
{
    if(USART_GetITStatus(STEP_MOTOR_TTYSX_BUS,USART_IT_RXNE))
    {
        if(stepMotorSerialDataFrame.FrameBit.FrameCNT < SERIAL_MAXDATA_SIZE)
        stepMotorSerialDataFrame.RX_BUF[stepMotorSerialDataFrame.FrameBit.FrameCNT ++] = USART_ReceiveData(STEP_MOTOR_TTYSX_BUS);
        /*通过对USART_DR的读操作可以将USART_IT_RXNE清0*/
    }
    if(USART_GetITStatus(STEP_MOTOR_TTYSX_BUS,USART_IT_IDLE) == SET )
    {
        stepMotorSerialDataFrame.FrameBit.FrameFinishFlag = 1;		//代表总线进入空闲状态，数据接收完毕

        USART_ReceiveData(STEP_MOTOR_TTYSX_BUS);				    //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
        SYS_DEBUG("%s\n",stepMotorSerialDataFrame.RX_BUF);
    }
}
