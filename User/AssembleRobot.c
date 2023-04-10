#include "AssembleRobot.h"

/**
 * @brief:  绑定机器人关节与对应电机对象
 * @param:  roboARM  要绑定的机器人关节对象
 * @param:  motor    要绑定的电机对象
 * @param:  gear     齿数比
 * @param:  moveType 关节运动类型
 * @retval: None
*/
void roboARM_MotorBinding(robot_ARM * roboARM,stepMotorClass * motor,float gear,roboARM_Movetype moveType)
{
    roboARM->motor = motor;
    roboARM->gear = gear;
    roboARM->MoveType = moveType;
}

/**
 * @brief:  机械手关节以相对角度进行旋转
 * @param:  roboARM     要操作的机械手对象
 * @param:  angle       相对上一位置要旋转的角度
 * @param:  dir         要旋转的方向
 * @param:  speed       要达到的最大速度
 * @param:  accelerate  加速度
 * @retval: 0:正常退出       -1:引用了错误的控制模式
*/
int roboARM_AngleExecute(robot_ARM * roboARM,float angle,stepMotorDir dir,float speed,uint8_t acceleratre)
{
    if(roboARM->MoveType == rotation)
    {
        /*结算该关节转动到指定角度所需要的脉冲数*/
        uint32_t pulseCNT;

        /*脉冲数 = 角度与圈数的对应关系 * 当前电机在该细分下转动一圈所需脉冲数 * 电机细分数 * 机械关节齿数比 */
        pulseCNT = ( angle / 360.0f ) * ( 360.0f / roboARM->motor->Public.param.DEG ) * 
                   (roboARM->motor->Public.param.division) * ( roboARM->gear );

        if( stepMotor_PulseExecute(roboARM->motor,dir,speed,acceleratre,pulseCNT) == 0)
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

