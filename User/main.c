#include "system.h"

int main(void)
{
    System_Init();              //系统初始化

    while(1)
    {
        OS_Scedule();           //执行任务调度
    }
}
