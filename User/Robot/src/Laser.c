#include <stdbool.h>
#include "Laser.h"
#include "display.h"
#include "relay.h"

/**
 * @breif:  激光限位初始化
*/
void Laser_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    Display_Logged(LOG_RANK_INFO,"Init laser...\n");

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = LASER_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(LASER_PORT,&GPIO_InitStruct);

    Display_Logged(LOG_RANK_OK,"Laser init done\n");
}

/**
 * @brief:  激光探头物块检测
*/
void Laser_BlockDetect(void)
{
    static bool Logged_flag = false;

    /*如果为高*/
    if( LASER_PORT->IDR & LASER_PIN )
    {
        Relay_Release();

        if(Logged_flag == true)
        {
            Display_Logged(LOG_RANK_WARNNING,"Laser:block detected!\n");
            Logged_flag = false; 
        }
    }
    else
    {
        Logged_flag = true;
    }
}

/**
 * @brief:  获取激光探测状态
 * @param:  None
 * @retval: true:有物块遮挡
 *          false:无物块遮挡
*/
__inline bool Laser_GetStatus(void)
{
    return ( LASER_PORT->IDR & LASER_PIN );
}
