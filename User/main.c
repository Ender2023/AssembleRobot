#include "main.h"

stepMotorClass motor1,motor2;

void MotorInit(void)
{
    stepMotorInitTypeDef stepMotorInitStruct;

    stepMotorInitStruct.IO.DIR_PORT = MotorIOB;
    stepMotorInitStruct.IO.COM_PORT = MotorIOB;
    stepMotorInitStruct.IO.ENA_PORT = MotorIOB;
    stepMotorInitStruct.IO.STP_PORT = MotorIOB;

    stepMotorInitStruct.IO.DIR_PIN = GPIO_Pin_3;
    stepMotorInitStruct.IO.COM_PIN = GPIO_Pin_4;
    stepMotorInitStruct.IO.ENA_PIN = GPIO_Pin_5;
    stepMotorInitStruct.IO.STP_PIN = GPIO_Pin_6;

    stepMotorInitStruct.param.division = div_4;
    stepMotorInitStruct.param.DEG = 1.80f;
    stepMotorInitStruct.param.freq_min = 100;
    stepMotorInitStruct.param.freq_max = 4500;
    stepMotorInitStruct.param.dir = dir_pos;
    stepMotorInitStruct.param.speed = 100;

    StepMotor_Init(&motor1,&stepMotorInitStruct);

    stepMotorInitStruct.IO.DIR_PORT = MotorIOC;
    stepMotorInitStruct.IO.COM_PORT = MotorIOC;
    stepMotorInitStruct.IO.ENA_PORT = MotorIOC;
    stepMotorInitStruct.IO.STP_PORT = MotorIOC;

    stepMotorInitStruct.IO.DIR_PIN = GPIO_Pin_0;
    stepMotorInitStruct.IO.COM_PIN = GPIO_Pin_1;
    stepMotorInitStruct.IO.ENA_PIN = GPIO_Pin_2;
    stepMotorInitStruct.IO.STP_PIN = GPIO_Pin_3;

    stepMotorInitStruct.param.speed = 100;
    stepMotorInitStruct.param.dir = dir_neg;
    stepMotorInitStruct.param.division = div_4;
    StepMotor_Init(&motor2,&stepMotorInitStruct);
}


int main(void)
{
    MotorInit();

    StepMotor_Brake(&motor1,DISABLE);
    StepMotor_Cmd(&motor2,ENABLE);

    //TIM_Cmd(STEP_MOTOR_TIM,ENABLE);                     //启动定时器进行任务调度
    
    // float i = 0.0;
    // int flag = 1;


    while(1)
    {

        stepMotor_AngleFoward(&motor1,);
        //stepMotor_StepFoward(&motor1,50);
        TIM_Cmd(STEP_MOTOR_TIM,ENABLE);                     //启动定时器进行任务调度
        delay_ms(500);

    //     motor1.Private.pulse_cnt = 0;
    //     motor2.Private.pulse_cnt = 0;

    //    if(i >= 100){flag = 0;}
    //    else if (i <= 5) {flag = 1;}

    //     if(flag)
    //     {
    //         i += 0.1;
    //         stepMotor_DirChange(&motor1,dir_pos);
    //     }
    //     else
    //     {
    //         i -= 0.1;
    //         stepMotor_DirChange(&motor1,dir_neg);
    //     }
        
    //    stepMotor_SpeedChange(&motor1,i);
    //    delay_ms(10);

    }
}


