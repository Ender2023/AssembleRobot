#include "main.h"

stepMotorClass motor1,motor2;
robot_ARM bigARM;

void Robot_Init(void)
{
    stepMotorInitTypeDef stepMotorInitStruct;

    stepMotorInitStruct.CTRL_MODE = SOFT_CTRL;

    stepMotorInitStruct.IO.addr = 1;
    stepMotorInitStruct.IO.ttySx = ttyS1;
    stepMotorInitStruct.param.DEG = 1.8f;
    stepMotorInitStruct.param.dir = dir_pos;
    stepMotorInitStruct.param.division = div_16;

    StepMotor_Init(&motor1,&stepMotorInitStruct);

    stepMotorInitStruct.IO.addr = 2;
    StepMotor_Init(&motor2,&stepMotorInitStruct);

    roboARM_MotorBinding(&bigARM,&motor1,ROBOARM_BIGARM_GEAR,rotation);

}

int main(void)
{
    System_Init();
    serialDebugInit();
    Robot_Init();

    delay_ms(50);

    while(1)
    {
        roboARM_AngleExecute(&bigARM,90,dir_neg,5,200);
        delay_s(3);
        roboARM_AngleExecute(&bigARM,90,dir_pos,5,200);
        delay_s(3);
    }
}


