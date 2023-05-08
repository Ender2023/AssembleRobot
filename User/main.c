#include "system.h"
#include "bsp_delay.h"
#include "bsp_serial.h"
#include "AssembleRobot.h"
#include "RF.h"
#include "relay.h"
#include "Key.h"
#include "Camera.h"
#include "turnplate.h"

int main(void)
{
    /*系统初始化*/
    System_Init();

    // roboJoint_Absolute_AngleExecute(&bigARM,30,5,200);
    // roboJoint_Absolute_AngleExecute(&smallARM,-180,5,200);
    // roboJoint_Absolute_AngleExecute(&rotationJoint,30,5,200);
    // roboJoint_Absolute_LineExecute(&upDownJoint,30,5,200);
    while(1);
}


