#ifndef __BSP_STEPMOTOR_H
#define __BSP_STEPMOTOR_H

#include "system.h"

/*任务调度定时器*/
#define STEP_MOTOR_TIM                      TIM2                    //步进电机定时器所对应的实际硬件资源
#define STEP_MOTOR_TIM_RCC                  RCC_APB1Periph_TIM2     //步进电机定时器时钟外设地址
#define STEP_MOTOR_TIM_CLK_DIV              TIM_CKD_DIV2            //步进电机定时器时钟分频,来自APB桥Sysclk
#define STEP_MOTOR_TIM_PERIOD               100                     //步进电机定时器溢出周期
#define STEP_MOTOR_TIM_PRESCALER            72                      //步进电机定时器预分频系数
#define STEP_MOTOR_TIM_IRQ_REGISTER         TIM2_IRQn               //向内核注册步进电机定时器中断服务函数
#define STEP_MOTOR_TIM_EVENTHANDLER         TIM2_IRQHandler         //步进电机定时器中断事件回调函数

/*软件串口控制*/
#define STEP_MOTOR_TTYSX_BUS                USART1                  //步进电机串口总线对应的实际硬件资源
#define STEP_MOTOR_TTYSX_BUS_BUADRATE       38400                   //由串口总线带动的串口波特率大小
#define STEP_MOTOR_TTYSX_BUS_IRQ_REGISTER   USART1_IRQHandler       //向内核注册步进电机串口中断回调函数

#define STEP_MOTOR_SOFT_SPEED_MAX           1279                    //步进电机最大速度量化值

#define STEP_MOTOR_PULSE_BY_DELAY           false                   //使能由阻塞带动的脉冲发生(不建议)

/*步进电机细分枚举*/
/**
 * 针对时代超群42步进驱动器，其拨码开关编码与细分关系表如下：
 *      BIT0    BIT1    BIT2    DIV
 *      ON      ON      ON      1
 *      OFF     ON      ON      2
 *      ON      OFF     ON      4
 *      OFF     OFF     ON      8
 *      ON      ON      OFF     16
 *      OFF     ON      OFF     32
*/
typedef enum{div1 = 1,div_2 = 2,div_4 = 4,div_8 = 8,div_16 = 16,div_32 = 32}stepMotorDivision;

/*步进电机引脚枚举*/
typedef enum{MotorIOA = 0,MotorIOB,MotorIOC,MotorIOD,MotorIOE,MotorIOF,MotorIOG}stepMotorGPIO;

/*步进电机方向枚举*/
typedef enum{dir_pos = 0,dir_neg}stepMotorDir;

/*步进电机状态枚举*/
typedef enum{disable = 0,stop,run}stepMotorState;

/*步进电机串口控制选项枚举*/
typedef enum{ttyS_NULL = 0,ttyS1,ttyS2,ttyS3,ttyS4,ttyS5}stepMotorSerialPort;

/*步进电机控制模式*/
typedef enum{SOFT_CTRL = 0,HARD_CTRL}stepMotorCtrlMode; 

/*步进电机类初始化参数*/
typedef struct 
{
    /*由用户配置的电机引脚属性*/
    struct
    {
        stepMotorSerialPort     ttySx;          //选择由串口x来对该电机进行控制(软件)
        uint8_t                 addr;           //记录该步进电机在串口总线上的地址(软件)

        stepMotorGPIO           DIR_PORT;       //方向端端口
        stepMotorGPIO           ENA_PORT;       //使能端端口
        stepMotorGPIO           STP_PORT;       //步进端端口
        stepMotorGPIO           COM_PORT;       //公共端端口

        uint32_t                DIR_PIN;        //方向端引脚
        uint32_t                ENA_PIN;        //使能端引脚
        uint32_t                STP_PIN;        //步进端引脚
        uint32_t                COM_PIN;        //公共端引脚
    }IO;

    /*由用户配置的电机属性*/
    struct
    {

        stepMotorDivision       division;       //细分
        stepMotorDir            dir;            //方向

        float                   speed;          //速度
        float                   DEG;            //步距角
        uint32_t                freq_min;       //频率下限
        uint32_t                freq_max;       //频率上限
    }param;
    
    stepMotorCtrlMode           CTRL_MODE;      //控制模式

}stepMotorInitTypeDef;


/*步进电机类定义*/
typedef struct
{
    /*公有参数由用户配置*/
    stepMotorInitTypeDef        Public;

    /*私有参数由系统调用*/
    struct 
    {
        /*硬件控制引脚*/
        GPIO_TypeDef *          DIR_GPIO_PORT;   //GPIO方向端端口
        GPIO_TypeDef *          ENA_GPIO_PORT;   //GPIO使能端端口
        GPIO_TypeDef *          STP_GPIO_PORT;   //GPIO步进端端口
        GPIO_TypeDef *          COM_GPIO_PORT;   //GPIO公共端端口

        /*软件占用串口与中断号*/
        USART_TypeDef *         ttySx;           //使用的串口
        uint16_t                ttyS_IRQ;        //占用的串口中断号

        /*软件控制引脚*/
        GPIO_TypeDef *          ttySx_Tx_PORT;   //串口接收端端口
        GPIO_TypeDef *          ttySx_Rx_PORT;   //串口发送端端口

        uint32_t                ttySx_Tx_Pin;    //串口发送端引脚
        uint32_t                ttySx_Rx_Pin;    //串口发送端引脚

        /*控制角度*/
        uint16_t                pulse;           //要发送的脉冲个数
        uint16_t                pulse_cnt;       //已发出的脉冲个数

        /*控制速度*/
        uint32_t                Hz;              //根据速度占比得到实际所需频率
        uint32_t                nopTime_us;      //根据实际频率换算延时时间
        uint32_t                delay_cnt;       //已延时时间

        stepMotorState          status;          //运行状态

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
