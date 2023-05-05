#include <math.h>
#include <stdbool.h>
#include "bsp_exti.h"
#include "bsp_delay.h"
#include "AssembleRobot.h"

/*机器人初始化标志位*/
bool robot_InitFlag = false;

/**
 * DECLARE
*/
stepMotorClass motor1,motor2,motor3,motor4;
robot_Joint bigARM,smallARM,rotationJoint,upDownJoint;
static void Robot_stopSwitchInit(void);
static void Robot_clampJawInit(void);

/**
 * @brief:  机器人整体初始化
*/
void Robot_Init(void)
{
    Display_Logged("Init robot...\n");
    /*如果尚未初始化过机器手*/
    if( robot_InitFlag == false )
    {
        stepMotorInitTypeDef stepMotorInitStruct;

        stepMotorInitStruct.CTRL_MODE = SOFT_CTRL;

        stepMotorInitStruct.IO.addr = 1;
        stepMotorInitStruct.IO.ttySx = ttyS2;
        stepMotorInitStruct.param.DEG = 1.8f;
        stepMotorInitStruct.param.dir = dir_pos;
        stepMotorInitStruct.param.division = 16;

        StepMotor_Init(&motor1,&stepMotorInitStruct);
        Display_Logged("Init motor1...\n");

        stepMotorInitStruct.IO.addr = 2;
        StepMotor_Init(&motor2,&stepMotorInitStruct);
        Display_Logged("Init motor2...\n");

        stepMotorInitStruct.IO.addr = 3;
        StepMotor_Init(&motor3,&stepMotorInitStruct);
        Display_Logged("Init motor3...\n");

        stepMotorInitStruct.IO.addr = 4;
        StepMotor_Init(&motor4,&stepMotorInitStruct);
        Display_Logged("Init motor4...\n");

        /*大臂关节参数绑定与初始化*/
        roboJoint_MotorBinding(&bigARM,&motor1,ROBOARM_BIGARM_GEAR,rotation);
        bigARM.length = ROBOARM_BIGARM_LENGTH;
        Display_Logged("Binding big arm...\n");

        /*小臂关节参数绑定与初始化*/
        roboJoint_MotorBinding(&smallARM,&motor2,ROBOARM_SMALLARM_GEAR,rotation);
        smallARM.length = ROBOARM_SMALLARM_LENGTH;
        Display_Logged("Binding small arm...\n");

        /*上下关节参数绑定与初始化*/
        roboJoint_MotorBinding(&upDownJoint,&motor3,1,line);
        Display_Logged("Binding up/down joint...\n");
        upDownJoint.workspace_max = 283;
        upDownJoint.workspace_min = 60;

        /*旋转关节参数绑定与初始化*/
        roboJoint_MotorBinding(&rotationJoint,&motor4,ROBOARM_ROTATION_GEAR,rotation);
        Display_Logged("Binding rotation joint...\n");

        /*限位开关初始化*/
        Robot_stopSwitchInit();
        Display_Logged("Init stop switch...\n");

        /*夹爪初始化*/
        Robot_clampJawInit();
        Display_Logged("Init clamp jaw...\n");

        /*整体向上移动，获得绝对高度*/
        //stepMotor_SpeedExecute(upDownJoint.motor,ROBOARM_UP_DIR,5,200);

        //Robot_clampJaw_Catch(false);
        Display_Logged("robot Init done!\n");
    }
}

/**
 * @brief:  添加限位开关
*/
static void Robot_stopSwitchInit(void)
{
    /*最大高度限位*/
    EXTI_gpioRegister(EXTI_IOD,GPIO_Pin_6,EXTI_Trigger_Rising);
}

/**
 * @brief:  夹爪初始化
*/
static void Robot_clampJawInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    /*时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB,&GPIO_InitStruct);  

    /*时基*/
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = ( CLAMP_JAW_TIMER_PERIOD -1 );
    TIM_TimeBaseInitStruct.TIM_Prescaler = ( CLAMP_JAW_TIMER_PRESCALER - 1 );
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(CLAMP_JAW_TIMER,&TIM_TimeBaseInitStruct); 

    /*输出通道配置*/
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = ( 1000 - 1 );
    TIM_OC1Init(CLAMP_JAW_TIMER,&TIM_OCInitStruct);
}

/**
 * @brief:  绑定机器人关节与对应电机对象
 * @param:  roboJoint  要绑定的机器人关节对象
 * @param:  motor       要绑定的电机对象
 * @param:  gear        齿数比
 * @param:  moveType    关节运动类型
 * @retval: None
*/
void roboJoint_MotorBinding(robot_Joint * roboJoint,stepMotorClass * motor,float gear,roboARM_Movetype moveType)
{
    roboJoint->motor = motor;
    roboJoint->gear = gear;
    roboJoint->MoveType = moveType;
    roboJoint->angle = 0;
}

/**
 * @brief:  机械手关节以相对角度进行旋转
 * @param:  roboJoint       要操作的机械手对象
 * @param:  angle           相对上一位置要旋转的角度
 * @param:  dir             要旋转的方向
 * @param:  speed           要达到的最大速度
 * @param:  accelerate      加速度
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int roboJoint_Relative_AngleExecute(robot_Joint * roboJoint,float angle,stepMotorDir dir,float speed,uint8_t acceleratre)
{
    if(roboJoint->MoveType == rotation)
    {
        /*结算该关节转动到指定角度所需要的脉冲数*/
        uint32_t pulseCNT;

        /*限制角度范围在-180° ~ +180°*/
        angle_mapping(angle);

        /*脉冲数 = 角度与圈数的对应关系 * 当前电机在该细分下转动一圈所需脉冲数 * 电机细分数 * 机械关节齿数比 */
        pulseCNT = ( angle / 360.0f ) * ( 360.0f / roboJoint->motor->Public.param.DEG ) * 
                   (roboJoint->motor->Public.param.division) * ( roboJoint->gear );

        if( stepMotor_PulseExecute(roboJoint->motor,dir,speed,acceleratre,pulseCNT) == 0)
        {
            /*更新记录的角度值*/
            if(dir == dir_pos)
            {
                roboJoint->angle += angle;
                angle_mapping(roboJoint->angle);
            }
            else
            {
                roboJoint->angle -= angle;
                angle_mapping(roboJoint->angle);
            }

            return 0;
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  机械手关节以相对距离进行运动
 * @param:  roboJoint     要操作的机械手对象
 * @param:  distance      相对上一位置要走过的距离(mm)
 * @param:  dir           要前进的方向
 * @param:  speed         要达到的最大速度
 * @param:  accelerate    加速度
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int roboJoint_Relative_LineExecute(robot_Joint * roboJoint,float distance,stepMotorDir dir,float speed,uint8_t acceleratre)
{
    if(roboJoint->MoveType == line)
    {
        /*结算该关节转动到指定角度所需要的脉冲数*/
        uint32_t pulseCNT;

        /*脉冲数 = 距离与螺杆导程的对应关系 * 当前电机在该细分下转动一圈所需脉冲数 * 电机细分数 * 机械关节齿数比 */
        pulseCNT = ( distance / ROBOARM_HELICAL_PITCH ) * ( 360.0f / roboJoint->motor->Public.param.DEG ) 
                    * (roboJoint->motor->Public.param.division) * ( roboJoint->gear );

        if( stepMotor_PulseExecute(roboJoint->motor,dir,speed,acceleratre,pulseCNT) == 0)
        {
            return 0;
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  机械手关节以绝对角度进行旋转
 * @param:  roboJoint   要操作的机械手对象
 * @param:  angle       要旋转的角度
 * @param:  dir         要旋转的方向
 * @param:  speed       要达到的最大速度
 * @param:  accelerate  加速度
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int roboJoint_Absolute_AngleExecute(robot_Joint * roboJoint,float angle,float speed,uint8_t acceleratre)
{
    if(roboJoint->MoveType == rotation)
    {
        /*电机要调整的方向*/
        stepMotorDir dir;

        /*结算该关节转动到指定角度所需要的脉冲数*/
        uint32_t pulseCNT;

        /*通过可能的解决方案寻找最优解*/
        float angle_solution1,angle_solution2,angle_err;

        /*限制角度范围在-180° ~ +180°*/
        angle_mapping(angle);

        /*结算目标角度与当前角度的差值,即要走过的角度*/
        angle_solution1 = angle - roboJoint->angle;
        if( angle_solution1 > 0 )
        {
            angle_solution2 = angle_solution1 - 360;
        }
        else if( angle_solution1 < 0 )
        {
            angle_solution2 = angle_solution1 + 360;
        }

        /*对比得出的解决方案，获取最优解*/
        if( fabs(angle_solution1) < fabs(angle_solution2) )
        {
            angle_err = angle_solution1;
        }
        else
        {
            angle_err = angle_solution2;
        }

        /*确定旋转方向*/
        if(angle_err < 0)
        {
            dir = dir_neg;
        }
        else if(angle_err > 0)
        {
            dir = dir_pos;
        }
        else
        {
            return 0;
        }

        /*脉冲数 = 角度与圈数的对应关系 * 当前电机在该细分下转动一圈所需脉冲数 * 电机细分数 * 机械关节齿数比 */
        pulseCNT = ( angle_err / 360.0f ) * ( 360.0f / roboJoint->motor->Public.param.DEG ) * 
                   (roboJoint->motor->Public.param.division) * ( roboJoint->gear );

        if( stepMotor_PulseExecute(roboJoint->motor,dir,speed,acceleratre,pulseCNT) == 0)
        {
            /*更新记录的角度值*/
            roboJoint->angle += angle_err;
            angle_mapping(roboJoint->angle);
            return 0;
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  机械手关节以相对距离进行运动
 * @param:  roboJoint     要操作的机械手对象
 * @param:  distance      要走到的绝对距离(mm)
 * @param:  speed         要达到的最大速度
 * @param:  accelerate    加速度
 * @retval: 0:正常退出       -1:引用了错误的控制模式    -2:不合理的高度参数
*/
int roboJoint_Absolute_LineExecute(robot_Joint * roboJoint,float distance,float speed,uint8_t acceleratre)
{
    /*如果要移动到的高度大于最大高度*/
    if( distance > roboJoint->workspace_max || distance < roboJoint->workspace_min )
    {
        return -2;
    }

    /*电机要调整的方向*/
    stepMotorDir dir;

    if(roboJoint->MoveType == line)
    {
        /*结算该关节转动到指定角度所需要的脉冲数*/
        uint32_t pulseCNT;

        /*结算目标高度与当前高度的差值*/
        distance = distance - roboJoint->distance;

        if(distance < 0)
        {
            dir = ROBOARM_DOWN_DIR;
        }
        else if(distance > 0)
        {
            dir = ROBOARM_UP_DIR;
        }
        else
        {
            return 0;
        }

        /*脉冲数 = 距离与螺杆导程的对应关系 * 当前电机在该细分下转动一圈所需脉冲数 * 电机细分数 * 机械关节齿数比 */
        pulseCNT = ( distance / ROBOARM_HELICAL_PITCH ) * ( 360.0f / roboJoint->motor->Public.param.DEG ) 
                    * (roboJoint->motor->Public.param.division) * ( roboJoint->gear );

        if( stepMotor_PulseExecute(roboJoint->motor,dir,speed,acceleratre,pulseCNT) == 0)
        {
            /*更新记录的高度值*/
            roboJoint->distance += distance;

            return 0;
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief:  机器人运动学逆解
 * @param:  x
 * @param:  y
 * @param:  z
 * @param:  speed         各关节的运动速度
 * @param:  acceleration  加速度
*/
int robo_InverseMotion(float x,float y,float z,float speed,uint8_t acceleratre)
{
    float rads_bigARM,rads_smallARM;

    /*计算得到小臂角*/
    rads_smallARM = acos( ( pow(x,2) + pow(y,2) - pow(bigARM.length,2) - pow(smallARM.length,2)  ) 
                            / ( 2 * bigARM.length * smallARM.length ) );

    /*计算获得大臂角*/
    rads_bigARM = atan( (x / y) ) - atan( ( smallARM.length * sin(rads_smallARM) ) 
                        / ( bigARM.length + smallARM.length * cos(rads_smallARM) ) );

    /*控制大臂移动到绝对角度*/
    roboJoint_Absolute_AngleExecute(&bigARM,rads_to_angle(rads_bigARM),speed,acceleratre);

    /*控制小臂移动到绝对角度*/
    roboJoint_Absolute_AngleExecute(&smallARM,rads_to_angle(rads_smallARM),speed,acceleratre);
    
    /*控制上下臂走到绝对高度*/
    roboJoint_Absolute_LineExecute(&upDownJoint,z,speed,acceleratre);

    return 0;
}

/**
 * @brief:  夹抓使能/失能控制
 * @param:  NewState:   使能状态
 *          ENABLE:     夹抓使能
 *          DISABLE:    夹抓失能
 * @retval: None
*/
void Robot_ClampJawCmd(FunctionalState NewState)
{
    if(NewState)
    {
        CLAMP_JAW_TIMER->CR1 |= TIM_CR1_CEN;
    }
    else
    {
        CLAMP_JAW_TIMER->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
    }
}

/**
 * @brief:  机械夹抓夹紧
 * @param:  state:      夹紧状态
 *          true:       夹抓夹紧
 *          false:      夹抓松开
 * @retval: None
*/
void Robot_clampJaw_Catch(bool state)
{
    if(state)
    {
        CLAMP_JAW_CATCH();
        Robot_ClampJawCmd(ENABLE);
        Display_Logged("Clamp jaw Catch!\n");
    }
    else
    {
        CLAMP_JAW_RELEASE();
        Robot_ClampJawCmd(ENABLE);
        Display_Logged("Clamp jaw Release!\n");
    }
}

/**
 * @brief:  机械夹抓松开
 * @param:  state:      夹紧状态
 *          true:       夹抓松开
 *          false:      夹抓抓紧
 * @retval: None
*/
void Robot_clampJaw_Release(bool state)
{
    if(state)
    {
        CLAMP_JAW_RELEASE();
        Robot_ClampJawCmd(ENABLE);
        Display_Logged("Clamp jaw Release!\n");
    }
    else
    {
        CLAMP_JAW_CATCH();
        Robot_ClampJawCmd(ENABLE);
        Display_Logged("Clamp jaw Catch!\n");

    }
}

/**
 * @brief:  机械手关节紧急停止
 * @param:  roboJoint   停止的关节
 * @retval: None
*/
static void roboJoint_UrgentSTOP(robot_Joint * roboJoint) 
{
    stepMotor_SpeedExecute(roboJoint->motor,dir_neg,0,255);
}

/**
 * @brief: 限位开关中断
*/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line6) == SET)
	{
		roboJoint_UrgentSTOP(&upDownJoint);

        if( robot_InitFlag == false )
        {
            upDownJoint.distance = upDownJoint.workspace_max;
            robot_InitFlag = true;
        }

		EXTI_ClearITPendingBit(EXTI_Line6);					//清除中断标志位
	}
}

/************************************************************
*						End of File
************************************************************/
