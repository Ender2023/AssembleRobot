#include "AssembleRobot.h"

/**
 * @brief:  �󶨻����˹ؽ����Ӧ�������
 * @param:  roboARM  Ҫ�󶨵Ļ����˹ؽڶ���
 * @param:  motor    Ҫ�󶨵ĵ������
 * @param:  gear     ������
 * @param:  moveType �ؽ��˶�����
 * @retval: None
*/
void roboARM_MotorBinding(robot_ARM * roboARM,stepMotorClass * motor,float gear,roboARM_Movetype moveType)
{
    roboARM->motor = motor;
    roboARM->gear = gear;
    roboARM->MoveType = moveType;
}

/**
 * @brief:  ��е�ֹؽ�����ԽǶȽ�����ת
 * @param:  roboARM     Ҫ�����Ļ�е�ֶ���
 * @param:  angle       �����һλ��Ҫ��ת�ĽǶ�
 * @param:  dir         Ҫ��ת�ķ���
 * @param:  speed       Ҫ�ﵽ������ٶ�
 * @param:  accelerate  ���ٶ�
 * @retval: 0:�����˳�       -1:�����˴���Ŀ���ģʽ
*/
int roboARM_AngleExecute(robot_ARM * roboARM,float angle,stepMotorDir dir,float speed,uint8_t acceleratre)
{
    if(roboARM->MoveType == rotation)
    {
        /*����ùؽ�ת����ָ���Ƕ�����Ҫ��������*/
        uint32_t pulseCNT;

        /*������ = �Ƕ���Ȧ���Ķ�Ӧ��ϵ * ��ǰ����ڸ�ϸ����ת��һȦ���������� * ���ϸ���� * ��е�ؽڳ����� */
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

