#include "bsp_delay.h"

/**
  * @brief  ΢�뼶��ʱ
  * @param  xus ��ʱʱ��
  * @retval None
  */
void delay_us(uint32_t xus)
{
	SysTick->LOAD = (CORE_FRECENCY_M * xus);	//���ö�ʱ����װֵ
	SysTick->VAL = 0x00;					    //��յ�ǰ����ֵ
	SysTick->CTRL = 0x00000005;				    //����ʱ��Դ��AHB�ţ���������ʱ��
	while(!(SysTick->CTRL & 0x00010000));	    //�ȴ�������0
	SysTick->CTRL = 0x00000004;				    //�رն�ʱ��
}

/**
  * @brief  ���뼶��ʱ
  * @param  xms ��ʱʱ��
  * @retval None
  */
void delay_ms(uint32_t xms)
{
	while(xms--)
	{
		delay_us(1000);
	}
}
 
/**
  * @brief  �뼶��ʱ
  * @param  xs ��ʱʱ��
  * @retval None
  */
void delay_s(uint32_t xs)
{
	while(xs --)
	{
		delay_ms(1000);
	}
}

/************************************************************
*						End of File
************************************************************/
