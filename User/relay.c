#include "relay.h"

/**
 * @brief:  继电器控制引脚配置
*/
void Relay_PinConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RELAY_CTRL_RCC_PERIPH, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RELAY_CTRL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RELAY_CTRL_PORT, &GPIO_InitStructure);
    
    /*默认上电拉低*/
    RELAY_CTRL_PORT->BRR = RELAY_CTRL_PIN;
}

/**
 * @brief:  继电器释放
*/
__inline void Relay_Release(void)
{
    RELAY_CTRL_PORT->BRR = RELAY_CTRL_PIN;
}

/**
 * @brief:  继电器吸合
*/
__inline void Relay_actuation(void)
{
    RELAY_CTRL_PORT->BSRR = RELAY_CTRL_PIN;
}
