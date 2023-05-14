#include <math.h>
#include "display.h"
#include "turnplate.h"

static stepMotorClass motor_turnPlate;
TurnplateType turnplate;

/**
 * @brief:  转盘步进电机引脚配置初始化
*/
static void Turnplate_PinConfig(void)
{
    stepMotorInitTypeDef stepMotorInitStruct;

    stepMotorInitStruct.CTRL_MODE = HARD_CTRL;
    stepMotorInitStruct.IO.COM_PORT = MotorIOX;
    stepMotorInitStruct.IO.DIR_PORT = MotorIOG;
    stepMotorInitStruct.IO.DIR_PIN = GPIO_Pin_13;
    stepMotorInitStruct.IO.ENA_PORT = MotorIOG;
    stepMotorInitStruct.IO.ENA_PIN = GPIO_Pin_11;
    stepMotorInitStruct.IO.STP_PORT = MotorIOG;
    stepMotorInitStruct.IO.STP_PIN = GPIO_Pin_12;

    stepMotorInitStruct.IO.ENA_Level = LOW_LEVEL;
    stepMotorInitStruct.param.DEG = 1.80f;
    stepMotorInitStruct.param.dir = dir_pos;
    stepMotorInitStruct.param.division = 4;
    stepMotorInitStruct.param.freq_max = 3000;
    stepMotorInitStruct.param.freq_min = 1000;
    stepMotorInitStruct.param.speed = 15;

    StepMotor_Init(turnplate.motor,&stepMotorInitStruct);
}

/**
 * @brief:  转盘初始化
*/
void Turnplate_Init(void)
{
    Display_Logged(LOG_RANK_INFO,"Init turnplate...\n");

    /*初始化容器池*/

    /*容器1*/
    turnplate.contentPool[0].angle = 0.0f;
    turnplate.contentPool[0].shape = shapeSquare;
    turnplate.contentPool[0].isEmpty = true;
    /*容器2*/
    turnplate.contentPool[1].angle = 45.0f;
    turnplate.contentPool[1].shape = shapeTriangle;
    turnplate.contentPool[1].isEmpty = true;
    /*容器3*/
    turnplate.contentPool[2].angle = 90.0f;
    turnplate.contentPool[2].shape = shapeCircle;
    turnplate.contentPool[2].isEmpty = true;
    /*容器4*/
    turnplate.contentPool[3].angle = 135.0f;
    turnplate.contentPool[3].shape = shapeSquare;
    turnplate.contentPool[3].isEmpty = true;
    /*容器5*/
    turnplate.contentPool[4].angle = 180.0f;
    turnplate.contentPool[4].shape = shapeTriangle;
    turnplate.contentPool[4].isEmpty = true;
    /*容器6*/
    turnplate.contentPool[5].angle = 225.0f;
    turnplate.contentPool[5].shape = shapeSquare;
    turnplate.contentPool[5].isEmpty = true;
    /*容器7*/
    turnplate.contentPool[6].angle = 270.0f;
    turnplate.contentPool[6].shape = shapeCircle;
    turnplate.contentPool[6].isEmpty = true;
    /*容器8*/
    turnplate.contentPool[7].angle = 315.0f;
    turnplate.contentPool[7].shape = shapeTriangle;
    turnplate.contentPool[7].isEmpty = true;

    /*默认从0°开始旋转*/
    turnplate.angle = 0.0f;

    turnplate.gear = TURNPLATE_GEAR;        //给定转盘大小盘之间的齿数比
    turnplate.motor = &motor_turnPlate;      //绑定转盘所用的电机对象

    Turnplate_PinConfig();
    Turnplate_Cmd(ENABLE);

    Display_Logged(LOG_RANK_OK,"Turnplate init done!\n");
}

/**
 * @brief:  转盘使能控制
 * @param:  NewState:   
 *          ENABLE:使能
 *          DISABLE:失能
 * @retval: None
*/
void Turnplate_Cmd(FunctionalState NewState)
{
    StepMotor_Cmd(turnplate.motor,ENABLE);
}

/**
 * @brief:  转盘改变转动方向
 * @param:  dir
 * @retval: None 
*/
void Turnplate_DirChange(stepMotorDir dir)
{
    stepMotor_DirChange(turnplate.motor,dir);
}

/**
 * @breif:  转盘转到特定角度
 * @param:  angle:  转盘所需走过的角度
*/
void Turnplate_toAngle(float angle)
{
    /*通过可能的解决方案寻找最优解*/
    float angle_solution1,angle_solution2,angle_err;

    /*限制角度范围在-180° ~ +180°*/
    angle_mapping(angle);

    /*结算目标角度与当前角度的差值,即要走过的角度*/
    angle_solution1 = angle - turnplate.angle;
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
        Turnplate_DirChange(dir_neg);
    }
    else if(angle_err > 0)
    {
        Turnplate_DirChange(dir_pos);
    }

    /*解析角度相对运动所需脉冲*/
    stepMotor_AngleFoward(turnplate.motor,( turnplate.gear * angle_err ));

    /*执行调度*/
    steoMotor_TaskExcute();

    /*打印日志*/
    Display_Logged(LOG_RANK_WARNNING,"Turnplate: to %.0f\n",angle);

    /*更新记录的角度值*/
    turnplate.angle += angle_err;
    angle_mapping(turnplate.angle);
}

/**
 * @brief:  调整转盘至相应的容器
 * @param:  shape:  所需存放物块的形状
 * @retval: 0:      正常存放
 *          -1:     未找到空闲的相应容器
*/
int Turnplate_toContent(Contentshape shape)
{
    uint8_t ctr;
    float angle;

    /*查找对应形状的剩余容器所处的位置*/
    for(ctr = 0; ctr < 7; ctr ++)
    {
        /*查找相应形状的容器*/
        if(turnplate.contentPool[ctr].shape == shape)
        {
            /*判断该容器是否已被占用*/
            if(turnplate.contentPool[ctr].isEmpty == true)
            {
                angle = turnplate.contentPool[ctr].angle;       //获取该容器的角度
                Turnplate_toAngle(angle);                       //调整至相应的容器所在角度位置
                turnplate.contentPool[ctr].isEmpty = false;     //占用该容器
                
                return 0;
            }
        }
    }

    return -1;
}

/************************************************************
*						End of File
************************************************************/
