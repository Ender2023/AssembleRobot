#ifndef __ASSEMBLEROBOT_H
#define __ASSEMBLEROBOT_H

#include "system.h"
#include "bsp_stepmotor.h"

/*齿数比*/
#define ROBOARM_BIGARM_GEAR         20.0f
#define ROBOARM_SMALLARM_GEAR       16.25f
#define ROBOARM_ROTATION_GEAR       4.65f

/*臂长*/
#define ROBOARM_BIGARM_LENGTH       228.0f
#define ROBOARM_SMALLARM_LENGTH     144.0f

/*螺距*/
#define ROBOARM_HELICAL_PITCH       2
#define ROBOARM_UP_DIR              dir_neg
#define ROBOARM_DOWN_DIR            dir_pos

/*夹抓控制*/
#define	CLAMP_JAW_TIMER				TIM4
#define CLAMP_JAW_TIMER_USE_PERIPH	RCC_APB1Periph_TIM4
#define CLAMP_JAW_TIMER_PRESCALER	72
#define CLAMP_JAW_TIMER_PERIOD		20000

#define	CLAMP_JAW_SPIN_MIN	        500.0f
#define	CLAMP_JAW_SPIN_MAX		    2500.0f

#define	CLAMP_JAW_MAX_ANGLE		    180.0f

#define	SCLAMP_JAW_K		        ( ( CLAMP_JAW_SPIN_MAX - CLAMP_JAW_SPIN_MIN ) / CLAMP_JAW_MAX_ANGLE )
#define	CLAMP_JAW_B				    CLAMP_JAW_SPIN_MIN

#define	CLAMP_JAW_SetPWM(x)         TIM4->CCR1 = ( ( (uint16_t) ( SCLAMP_JAW_K * x + CLAMP_JAW_B ) ) - 1 )

#define CLAMP_JAW_GRASPED()         CLAMP_JAW_SetPWM(100)
#define CLAMP_JAW_CATCH()           CLAMP_JAW_SetPWM(70)
#define CLAMP_JAW_LOOSE()           CLAMP_JAW_SetPWM(30)
#define CLAMP_JAW_RELEASE()         CLAMP_JAW_SetPWM(0)

#define STOPSWITCH_PORT             GPIOG
#define STOPSWITCH_PIN              GPIO_Pin_5

/*机械手运动方式*/
typedef enum{rotation = 0,line}roboARM_Movetype;

/*机械手关节对象*/
typedef struct
{
    const char *        name;           //关节名称
    stepMotorClass *    motor;          //机械手臂占用的电机对象
    roboARM_Movetype    MoveType;       //该关节的运动方式
    float               gear;           //该关节的齿数比
    float               length;         //该关节的长度
    float               angle;          //记录当前关节相对上一坐标系的角度
    float               distance;       //记录当前机械手的当前高度

    float               workspace_min;  //工作区的最小阈值
    float               workspace_max;  //工作区最大阈值
    
}robot_Joint;

extern robot_Joint bigARM,smallARM,rotationJoint,upDownJoint;
extern bool robot_HeightInit;

/*初始化*/
void Robot_Init(void);
void roboJoint_MotorBinding(robot_Joint * roboJoint,stepMotorClass * motor,float gear,roboARM_Movetype moveType);

/*运动*/
int roboJoint_Relative_AngleExecute(robot_Joint * roboJoint,float angle,stepMotorDir dir,float speed,uint8_t acceleratre);
int roboJoint_Relative_LineExecute(robot_Joint * roboJoint,float distance,stepMotorDir dir,float speed,uint8_t acceleratre);
int roboJoint_Absolute_AngleExecute(robot_Joint * roboJoint,float angle,float speed,uint8_t acceleratre);
int roboJoint_Absolute_LineExecute(robot_Joint * roboJoint,float distance,float speed,uint8_t acceleratre);
int robo_InverseMotion(float x,float y,float z,float speed,uint8_t acceleratre);

/*夹抓*/
void Robot_clampJaw_Catch(bool state);
void Robot_clampJaw_Graspe(bool state);
void Robot_clampJaw_Release(bool state);
void Robot_ClampJawCmd(FunctionalState NewState);
void Robot_clampJaw_Loose(void);

/*物块操作*/
void Robot_CatchCircle(void);
void Robot_CtachRectangle(void);
void Robot_CatchTriangle(void);



#endif

/************************************************************
*						End of File
************************************************************/
