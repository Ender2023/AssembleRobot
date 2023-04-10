#ifndef __BSP_STEPMOTOR_H
#define __BSP_STEPMOTOR_H

#include "system.h"

/*������ȶ�ʱ��*/
#define STEP_MOTOR_TIM                      TIM2                    //���������ʱ������Ӧ��ʵ��Ӳ����Դ
#define STEP_MOTOR_TIM_RCC                  RCC_APB1Periph_TIM2     //���������ʱ��ʱ�������ַ
#define STEP_MOTOR_TIM_CLK_DIV              TIM_CKD_DIV2            //���������ʱ��ʱ�ӷ�Ƶ,����APB��Sysclk
#define STEP_MOTOR_TIM_PERIOD               100                     //���������ʱ���������
#define STEP_MOTOR_TIM_PRESCALER            72                      //���������ʱ��Ԥ��Ƶϵ��
#define STEP_MOTOR_TIM_IRQ_REGISTER         TIM2_IRQn               //���ں�ע�Ჽ�������ʱ���жϷ�����
#define STEP_MOTOR_TIM_EVENTHANDLER         TIM2_IRQHandler         //���������ʱ���ж��¼��ص�����

/*������ڿ���*/
#define STEP_MOTOR_TTYSX_BUS                USART1                  //��������������߶�Ӧ��ʵ��Ӳ����Դ
#define STEP_MOTOR_TTYSX_BUS_BUADRATE       38400                   //�ɴ������ߴ����Ĵ��ڲ����ʴ�С
#define STEP_MOTOR_TTYSX_BUS_IRQ_REGISTER   USART1_IRQHandler       //���ں�ע�Ჽ����������жϻص�����

#define STEP_MOTOR_SOFT_SPEED_MAX           1279                    //�����������ٶ�����ֵ

#define STEP_MOTOR_PULSE_BY_DELAY           false                   //ʹ�����������������巢��(������)

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
typedef enum{div1 = 1,div_2 = 2,div_4 = 4,div_8 = 8,div_16 = 16,div_32 = 32}stepMotorDivision;

/*�����������ö��*/
typedef enum{MotorIOA = 0,MotorIOB,MotorIOC,MotorIOD,MotorIOE,MotorIOF,MotorIOG}stepMotorGPIO;

/*�����������ö��*/
typedef enum{dir_pos = 0,dir_neg}stepMotorDir;

/*�������״̬ö��*/
typedef enum{disable = 0,stop,run}stepMotorState;

/*����������ڿ���ѡ��ö��*/
typedef enum{ttyS_NULL = 0,ttyS1,ttyS2,ttyS3,ttyS4,ttyS5}stepMotorSerialPort;

/*�����������ģʽ*/
typedef enum{SOFT_CTRL = 0,HARD_CTRL}stepMotorCtrlMode; 

/*����������ʼ������*/
typedef struct 
{
    /*���û����õĵ����������*/
    struct
    {
        stepMotorSerialPort     ttySx;          //ѡ���ɴ���x���Ըõ�����п���(���)
        uint8_t                 addr;           //��¼�ò�������ڴ��������ϵĵ�ַ(���)

        stepMotorGPIO           DIR_PORT;       //����˶˿�
        stepMotorGPIO           ENA_PORT;       //ʹ�ܶ˶˿�
        stepMotorGPIO           STP_PORT;       //�����˶˿�
        stepMotorGPIO           COM_PORT;       //�����˶˿�

        uint32_t                DIR_PIN;        //���������
        uint32_t                ENA_PIN;        //ʹ�ܶ�����
        uint32_t                STP_PIN;        //����������
        uint32_t                COM_PIN;        //����������
    }IO;

    /*���û����õĵ������*/
    struct
    {

        stepMotorDivision       division;       //ϸ��
        stepMotorDir            dir;            //����

        float                   speed;          //�ٶ�
        float                   DEG;            //�����
        uint32_t                freq_min;       //Ƶ������
        uint32_t                freq_max;       //Ƶ������
    }param;
    
    stepMotorCtrlMode           CTRL_MODE;      //����ģʽ

}stepMotorInitTypeDef;


/*��������ඨ��*/
typedef struct
{
    /*���в������û�����*/
    stepMotorInitTypeDef        Public;

    /*˽�в�����ϵͳ����*/
    struct 
    {
        /*Ӳ����������*/
        GPIO_TypeDef *          DIR_GPIO_PORT;   //GPIO����˶˿�
        GPIO_TypeDef *          ENA_GPIO_PORT;   //GPIOʹ�ܶ˶˿�
        GPIO_TypeDef *          STP_GPIO_PORT;   //GPIO�����˶˿�
        GPIO_TypeDef *          COM_GPIO_PORT;   //GPIO�����˶˿�

        /*���ռ�ô������жϺ�*/
        USART_TypeDef *         ttySx;           //ʹ�õĴ���
        uint16_t                ttyS_IRQ;        //ռ�õĴ����жϺ�

        /*�����������*/
        GPIO_TypeDef *          ttySx_Tx_PORT;   //���ڽ��ն˶˿�
        GPIO_TypeDef *          ttySx_Rx_PORT;   //���ڷ��Ͷ˶˿�

        uint32_t                ttySx_Tx_Pin;    //���ڷ��Ͷ�����
        uint32_t                ttySx_Rx_Pin;    //���ڷ��Ͷ�����

        /*���ƽǶ�*/
        uint16_t                pulse;           //Ҫ���͵��������
        uint16_t                pulse_cnt;       //�ѷ������������

        /*�����ٶ�*/
        uint32_t                Hz;              //�����ٶ�ռ�ȵõ�ʵ������Ƶ��
        uint32_t                nopTime_us;      //����ʵ��Ƶ�ʻ�����ʱʱ��
        uint32_t                delay_cnt;       //����ʱʱ��

        stepMotorState          status;          //����״̬

    }Private;

}stepMotorClass;

void StepMotor_Init(stepMotorClass * motor,stepMotorInitTypeDef * stepMotorParam);
int StepMotor_Cmd(stepMotorClass * motor,FunctionalState NewState);
int StepMotor_Brake(stepMotorClass * motor);
int stepMotor_StepFoward(stepMotorClass * motor, uint16_t xStep);
int stepMotor_AngleFoward(stepMotorClass * motor,float angle);
int stepMotor_DirChange(stepMotorClass * motor,stepMotorDir dir);
int stepMotor_SpeedChange(stepMotorClass * motor,float newSpeed);
int stepMotor_DivisionChange(stepMotorClass * motor,uint8_t newDivision);
int stepMotor_SpeedExecute(stepMotorClass * motor,stepMotorDir newDir,float newSpeed,uint8_t acceleration);
int stepMotor_PulseExecute(stepMotorClass * motor, stepMotorDir newDir, float newSpeed,uint8_t acceleration, uint32_t pulse);
int stepMotor_AngleExecute(stepMotorClass * motor, stepMotorDir newDir, float newSpeed,uint8_t acceleration,float angle);

#endif
