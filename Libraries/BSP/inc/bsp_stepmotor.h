#ifndef __BSP_STEPMOTOR_H
#define __BSP_STEPMOTOR_H

#include "stm32f4xx.h"
#include "bsp_delay.h"

#define STEP_MOTOR_TIM              TIM2                    //���������ʱ������Ӧ��ʵ��Ӳ����Դ
#define STEP_MOTOR_TIM_RCC          RCC_APB1Periph_TIM2     //���������ʱ��ʱ�������ַ
#define STEP_MOTOR_TIM_CLK_DIV      TIM_CKD_DIV2            //���������ʱ��ʱ�ӷ�Ƶ,����APB��Sysclk
#define STEP_MOTOR_TIM_PERIOD       50                     //���������ʱ���������
#define STEP_MOTOR_TIM_PRESCALER    84                      //���������ʱ��Ԥ��Ƶϵ��
#define STEP_MOTOR_TIM_IRQ_REGISTER TIM2_IRQn               //���ں�ע�Ჽ�������ʱ���жϷ�����
#define STEP_MOTOR_TIM_EVENTHANDLER TIM2_IRQHandler         //���������ʱ���ж��¼��ص�����

#define STEP_MOTOR_PULSE_BY_DELAY   false

/*�������ϸ��ö��*/
/**
 * ���ʱ����Ⱥ42�������������䲦�뿪�ر�����ϸ�ֹ�ϵ�����£�
 *      BIT0    BIT1    BIT2    DIV
 *      ON      ON      ON      1
 *      OFF     ON      ON      2
 *      ON      OFF     ON      4
 *      OFF     OFF     ON      8
 *      ON      ON      OFF     16
 *      OFF     ON      OFF     32
*/
typedef enum{div1 = 0,div_2 = 2,div_4 = 4,div_8 = 8,div_16 = 16,div_32 = 32}stepMotorDivision;

/*�����������ö��*/
typedef enum{MotorIOA = 0,MotorIOB,MotorIOC,MotorIOD,MotorIOE,MotorIOF,MotorIOG}stepMotorGPIO;

/*�����������ö��*/
typedef enum{dir_pos = 0,dir_neg}stepMotorDir;

/*�������״̬ö��*/
typedef enum{stop = 0,run,disable}stepMotorState;

/*����������ʼ������*/
typedef struct 
{
    /*���û����õĵ����������*/
    struct
    {
        stepMotorGPIO       DIR_PORT;       //����˶˿�
        stepMotorGPIO       ENA_PORT;       //ʹ�ܶ˶˿�
        stepMotorGPIO       STP_PORT;       //�����˶˿�
        stepMotorGPIO       COM_PORT;       //�����˶˿�

        uint32_t            DIR_PIN;        //���������
        uint32_t            ENA_PIN;        //ʹ�ܶ�����
        uint32_t            STP_PIN;        //����������
        uint32_t            COM_PIN;        //����������
    }IO;

    /*���û����õĵ������*/
    struct
    {
        stepMotorDivision   division;       //ϸ��
        stepMotorDir        dir;            //����

        float               speed;          //�ٶ�
        float               DEG;            //�����
        uint32_t            freq_min;       //Ƶ������
        uint32_t            freq_max;       //Ƶ������
    }param;

}stepMotorInitTypeDef;

/*��������ඨ��*/
typedef struct
{
    /*���в������û�����*/
    stepMotorInitTypeDef    Public;

    /*˽�в�����ϵͳ����*/
    struct 
    {
        /*��������*/
        GPIO_TypeDef *      DIR_GPIO_PORT;   //GPIO����˶˿�
        GPIO_TypeDef *      ENA_GPIO_PORT;   //GPIOʹ�ܶ˶˿�
        GPIO_TypeDef *      STP_GPIO_PORT;   //GPIO�����˶˿�
        GPIO_TypeDef *      COM_GPIO_PORT;   //GPIO�����˶˿�

        /*���ƽǶ�*/
        uint16_t pulse;                      //Ҫ���͵��������
        uint16_t pulse_cnt;                  //�ѷ������������

        /*�����ٶ�*/
        uint32_t Hz;                         //�����ٶ�ռ�ȵõ�ʵ������Ƶ��
        uint32_t nopTime_us;                 //����ʵ��Ƶ�ʻ�����ʱʱ��
        uint32_t delay_cnt;                  //����ʱʱ��

        stepMotorState      status;          //����״̬

    }Private;

}stepMotorClass;

void StepMotor_Init(stepMotorClass * motor,stepMotorInitTypeDef * stepMotorParam);
void StepMotor_Cmd(stepMotorClass * motor,FunctionalState NewState);
void StepMotor_Brake(stepMotorClass * motor, FunctionalState NewState);
void stepMotor_StepFoward(stepMotorClass * motor, uint16_t xStep);
void stepMotor_AngleFoward(stepMotorClass * motor,float angle);
void stepMotor_DirChange(stepMotorClass * motor,stepMotorDir dir);
void stepMotor_SpeedChange(stepMotorClass * motor,float newSpeed);

#endif
