#include "task.h"
#include "Key.h"
#include "Camera.h"
#include "bsp_serial.h"
#include "AssembleRobot.h"
#include "relay.h"
#include "display.h"
#include "Laser.h"
#include "bsp_delay.h"

#define UP_SPEED                                30
#define DOWN_SPEED                              20
#define BIGARM_SPEED                            6

/*起始位置*/
#define START_BIGARM                            0
#define START_SMALLARM                          -154
#define START_ROTATION                          25

/*判断位置*/
#define JUDGE_HEIGHT                            230
#define JUDGE_ROTATE                            5

/*夾取位置*/
#define CATCH_HEIGHT                            130

/*传输位置*/
#define TRANSPORT_HEIGHT                        150

/*释放位置*/
#define RELEASE_BIGARM                          182
#define RELEASE_SMALLARM                        -180
#define RELEASE_HEIGHT                          70
#define RELEASE_ROTATION                        0

/*微调角度*/
#define LITTLE_ADJUST                           5

/*是否在放置时松开一段区间进行微调*/
#define USE_LOOSE                               1
#define UNUSE_LOOSE                             0
#define NEED_LOOSE                              USE_LOOSE

enum_TaskStage taskPreStage = stage_begin;      //前一阶段
enum_TaskStage taskStage = stage_begin;         //当前阶段
int16_t task_delay = 0;                         //任务延时计数器

extern uint8_t receive_time;                    //接收次数
extern bool motor_execute_done;                 //电机执行完毕标志位

/*任务驱动表*/
void (*task[MAX_STAGE_SIZE])(void);

/**
 * @brief:  机器人任务延时
 * @param:  X50ms   要延时的时间为50ms的x倍
 * @retval: None
*/
void Robot_Sleep(int16_t X50ms)
{
    task_delay = X50ms;
    taskPreStage = taskStage;   //记录上一阶段
    taskStage = stage_sleep;    //进入sleep阶段
}

/**
 * @brief:  任务开始阶段
*/
void Robot_TaskBegin(void)
{
    Display_Logged(LOG_RANK_OK,"Robot:Task begin\n");
    
    if( KEY3.STA == KEY_STA_RELEASED)
    {
        return ;
    }
    else
    {
        taskPreStage = stage_begin;
        taskStage = stage1;
        Display_Logged(LOG_RANK_OK,"Robot:In stage1\n");
    }
}

/**
 * @brief:  任务阶段1，吸合继电器，启动传送带,同时大臂走到0°，小臂走至-148°等待识别
*/
void Robot_stage1(void)
{
    roboJoint_Absolute_AngleExecute(&smallARM,START_SMALLARM,5,200);             //小臂走到初始状态
    if( taskPreStage == stage_begin )
    {
        delay_s(3);
    }
    roboJoint_Absolute_AngleExecute(&rotationJoint,START_ROTATION,5,200);        //旋转关节走到初始状态
    Relay_actuation();                                                           //吸合继电器，启动传送带

    /*执行阶段2*/
    taskPreStage = stage1;
    taskStage = stage2;
    Display_Logged(LOG_RANK_OK,"Robot:In stage2\n");
}

/**
 * @brief:  任务阶段2，等待物块遮挡激光光束，并向摄像头发出识别请求
*/
void Robot_stage2(void)
{
    /*单次执行标志位*/
    static bool flag;
    int retval;                                         //机械手执行返回值

    if( taskPreStage == stage1 ){flag = false;}
    taskPreStage = stage2;

    /*如果没有检测到物块，则一直等待*/
    if( !Laser_GetStatus() )
    {
        return;
    }
    else
    {
        /*检测到物块后,释放夹抓,并下降到指定高度,准备识别*/
        if(!flag)
        {
            Robot_clampJaw_Release(ENABLE);
            retval = roboJoint_Absolute_LineExecute(&upDownJoint,JUDGE_HEIGHT,DOWN_SPEED,200);
            receive_time = 0;
            motor_execute_done = false;
            flag = true;
        }
        
        /*等待上下关节就位*/
        if(motor_execute_done || retval == 1)
        {
            delay_ms(1500);                             //适当延时，避免过早识别方形误识别为圆
            Camera_startSample();                       //向摄像头传输采样命令

            taskPreStage = stage2;
            taskStage = stage3;
            Display_Logged(LOG_RANK_OK,"Robot:In stage3\n");
        }
    }
}

/**
 * @brief:  任务阶段3，等待摄像头返回物块形状与角度偏差，若为可夹持姿态，直接夹持，否则调整
*/
void Robot_stage3(void)
{
    /*单次执行标志位*/
    static bool flag;

    if( taskPreStage == stage2 ){flag = false;}
    taskPreStage = stage3;

    /*若接收到了有效的数据*/
    if(cameraValidData == true)
    {
        /*如果物块已经为正对着的*/
        if( cameraData.cmd == justCarry )
        {
            if(!flag)
            {
                roboJoint_Absolute_LineExecute(&upDownJoint,CATCH_HEIGHT,DOWN_SPEED,200);
                receive_time = 0;
                motor_execute_done = false;
                flag = true;
            }

            /*等待上下关节就位*/
            if(motor_execute_done)
            {
                /*三角形用紧夹持*/
               if(cameraData.shape == shapeTriangle)
               {
                    Robot_clampJaw_Graspe(true);
               }
               else
               {
                    Robot_clampJaw_Catch(true);
               }

                taskPreStage = stage3;
                taskStage = stage5;
                Display_Logged(LOG_RANK_OK,"Robot:In stage5\n");
            }
        }
        /*否则还需要旋转夹抓进行校正*/
        else
        {
            Display_Logged(LOG_RANK_OK,"Robot:In stage4\n");
            taskPreStage = stage3;
            taskStage = stage4;
        }
    }
    else
    {
        Camera_startSample();       //再次向摄像头传输采样命令，防止读到的数据为空
        Display_Logged(LOG_RANK_OK,"Robot:In sleep\n");
        Robot_Sleep(2);             //睡眠100ms
    }
}

/**
 * @brief:  任务阶段4，重复询问摄像头物块姿态并及时校准
*/
void Robot_stage4(void)
{
    /*单次执行标志位*/
    static bool flag;

    if( taskPreStage == stage3 ){flag = false;}

    /*每次调整5°*/
    if( !flag )
    {
        roboJoint_Relative_AngleExecute(&rotationJoint,JUDGE_ROTATE,dir_pos,10,200);
        flag = true;
        Display_Logged(LOG_RANK_OK,"Robot:In sleep\n");
        Robot_Sleep(20);
        return ;
    }
    
    /*如果是从休眠状态中醒来*/
    if(taskPreStage == stage_sleep)
    {
        cameraValidData = false;
        Camera_startSample();                               //向摄像头传输采样命令
        Display_Logged(LOG_RANK_OK,"Robot:Back stage3\n");
        taskPreStage = stage4;
        taskStage = stage3;                                 //回到阶段3
    }

    taskPreStage = stage4;
}

/**
 * @brief:  任务阶段5，转盘转至指定角度
*/
void Robot_stage5(void)
{
    cameraValidData = false;
    Turnplate_toContent(cameraData.shape);
    taskPreStage = stage5;
    taskStage = stage6;
    Display_Logged(LOG_RANK_OK,"Robot:In stage6\n");
}

/**
 * @brief:  任务阶段6，上升并搬运物块至转盘上方
*/
void Robot_stage6(void)
{
    static bool flag;                                                   //单次执行标志位
    static bool flag2;                                                  //单次执行标志位

    if( taskPreStage == stage5 ){flag = false;flag2 = false;}
    taskPreStage = stage6;

    /*上升*/
    if(!flag)
    {
        roboJoint_Absolute_LineExecute(&upDownJoint,TRANSPORT_HEIGHT,UP_SPEED,200);
        receive_time = 0;
        motor_execute_done = false;
        flag = true;
    }

    /*等待上下关节就位*/
    if(motor_execute_done)
    {
        if(!flag2)
        {
            roboJoint_Absolute_AngleExecute(&bigARM,RELEASE_BIGARM,BIGARM_SPEED,150);
            receive_time = 0;
            motor_execute_done = false;
            flag2 = true;
        }
        
        if(motor_execute_done)
        {
            taskPreStage = stage6;
            taskStage = stage7;
            Display_Logged(LOG_RANK_OK,"Robot:In stage7\n");
        }
    }
}

/**
 * @brief:  任务阶段7，调整小臂至可放入槽内
*/
void Robot_stage7(void)
{
    static bool flag;                                                   //单次执行标志位

    if( taskPreStage == stage6 ){flag = false;}
    taskPreStage = stage7;

    /*调整小臂至物块可嵌入槽内*/
    if(!flag)
    {
        roboJoint_Absolute_AngleExecute(&smallARM,RELEASE_SMALLARM,5,200);
        receive_time = 0;
        motor_execute_done = false;
        flag = true;
    }

    /*等待调整完毕*/
    if(motor_execute_done)
    {
        roboJoint_Absolute_AngleExecute(&rotationJoint,RELEASE_ROTATION,10,200);
        delay_s(1);
        taskPreStage = stage7;
        taskStage = stage8;
        Display_Logged(LOG_RANK_OK,"Robot:In stage8\n");
    }
}

/**
 * @brief:  任务阶段8，下降，并将物块放置到对应槽中
*/
void Robot_stage8(void)
{
    static bool flag;                                                   //单次执行标志位

    if( taskPreStage == stage7 ){flag = false;}
    taskPreStage = stage8;

    /*下降至转盘高度*/
    if(!flag)
    {
        roboJoint_Absolute_LineExecute(&upDownJoint,RELEASE_HEIGHT,DOWN_SPEED,200);        //下降至转盘高度
        receive_time = 0;
        motor_execute_done = false;
        flag = true;
    }

    /*等待上下关节就位*/
    if(motor_execute_done)
    {

    /*如果需要微调*/
#if NEED_LOOSE == USE_LOOSE
        Robot_clampJaw_Loose();                                          //释放物块
        taskPreStage = stage8;
        taskStage = stage9;                                             //进入微调阶段
        Display_Logged(LOG_RANK_OK,"Robot:In stage9\n");

#elif NEED_LOOSE == UNUSE_LOOSE
        Robot_clampJaw_Release(ENABLE);                                 //释放物块
        taskPreStage = stage8;
        taskStage = stage10;                                            //进入直接放置阶段
        Display_Logged(LOG_RANK_OK,"Robot:In stage10\n");
#endif
    }
}

/**
 * @brief:  任务阶段9,微调物块放置角度
 */
void Robot_stage9(void)
{
    delay_ms(500);
    roboJoint_Absolute_AngleExecute(&rotationJoint,LITTLE_ADJUST,BIGARM_SPEED,150);
    delay_ms(800);
    roboJoint_Absolute_AngleExecute(&rotationJoint,-LITTLE_ADJUST,BIGARM_SPEED,150);
    delay_ms(500);
    Robot_clampJaw_Release(ENABLE);                                     //释放物块
    delay_ms(500);
    taskPreStage = stage9;
    taskStage = stage10;                                                //进入阶段10
    Display_Logged(LOG_RANK_OK,"Robot:In stage10\n");
}

/**
 * @brief:  任务阶段10，上升，大臂复位，进入下一个循环
*/
void Robot_stage10(void)
{
    static bool flag;                                                   //单次执行标志位
    static bool flag2;                                                  //单次执行标志位

    if( taskPreStage == stage8 || taskPreStage == stage9 ){flag = false;flag2 = false;}
    taskPreStage = stage10;

    /*上升*/
    if(!flag)
    {
        roboJoint_Absolute_LineExecute(&upDownJoint,JUDGE_HEIGHT,UP_SPEED,200);        //上升至等待识别高度
        receive_time = 0;
        motor_execute_done = false;
        flag = true;
    }

    /*等待上下关节就位*/
    if(motor_execute_done)
    {
        if(!flag2)
        {
            roboJoint_Absolute_AngleExecute(&bigARM,START_BIGARM,BIGARM_SPEED,150);
            receive_time = 0;
            motor_execute_done = false;
            flag2 = true;
        }
        
        delay_s(5);
        taskPreStage = stage10;
        taskStage = stage1;
        Display_Logged(LOG_RANK_OK,"Robot:In stage1\n");
    }
}

/**
 * @brief:  任务结休眠阶段
*/
void Robot_TaskEnd(void)
{
    if( taskPreStage != stage_end )
    {
        Display_Logged(LOG_RANK_OK,"Robot:Task end\n");
        taskPreStage = stage_end;
    }
}

/**
 * @brief:  任务延时阶段
*/
void Robot_TaskSleep(void)
{
    if(task_delay > 0)
    {
        task_delay --;
        return ;
    }
    else
    {
        taskStage = taskPreStage;
        taskPreStage = stage_sleep;
    }
}

/**
 * @brief:  任务初始化
*/
void Robot_TaskInit(void)
{
    Display_Logged(LOG_RANK_INFO,"Create task...\n");

    task[stage_begin] = Robot_TaskBegin;
    task[stage1] = Robot_stage1;
    task[stage2] = Robot_stage2;
    task[stage3] = Robot_stage3;
    task[stage4] = Robot_stage4;
    task[stage5] = Robot_stage5;
    task[stage6] = Robot_stage6;
    task[stage7] = Robot_stage7;
    task[stage8] = Robot_stage8;
    task[stage9] = Robot_stage9;
    task[stage10] = Robot_stage10;
    task[stage_end] = Robot_TaskEnd;
    task[stage_sleep] = Robot_TaskSleep;

    Display_Logged(LOG_RANK_OK,"Task created!\n");
}

/**
 * @brief:  任务分片与执行
*/
void Robot_TaskHandler(void)
{
    /*执行相应任务*/
    task[taskStage]();
}

