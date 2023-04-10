#include "system.h"

/**
 * @brief:  系统整体初始化
*/
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

}
