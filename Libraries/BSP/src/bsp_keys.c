#include "bsp_keys.h"

Key_Class KEY1,KEY2,KEY3,KEY4;

/************************************************************
*@brief:	按键引脚初始化
************************************************************/
void Keys_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	/*共有特征*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	/*KEY1*/
	KEY1.PORT = KEY1_PORT;
	KEY1.Pin = KEY1_PIN;
	KEY1.Valid_Level = KEY1_VALID;
	KEY1.Pressed_Threshold = KEY1_PRESSED_THRESHOLD;
	KEY1.LPressed_Threshold = KEY1_LPRESSED_THRESHOLD;
	KEY1.LLPressed_Threshold = KEY1_LLPRESSED_THRESHOLD;

	GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
	GPIO_Init(KEY1_PORT,&GPIO_InitStruct);

	/*KEY2*/
	KEY2.PORT = KEY2_PORT;
	KEY2.Pin = KEY2_PIN;
	KEY2.Valid_Level = KEY2_VALID;
	KEY2.Pressed_Threshold = KEY2_PRESSED_THRESHOLD;
	KEY2.LPressed_Threshold = KEY2_LPRESSED_THRESHOLD;
	KEY2.LLPressed_Threshold = KEY2_LLPRESSED_THRESHOLD;

	GPIO_InitStruct.GPIO_Pin = KEY2_PIN;
	GPIO_Init(KEY2_PORT,&GPIO_InitStruct);

	/*KEY3*/
	KEY3.PORT = KEY3_PORT;
	KEY3.Pin = KEY3_PIN;
	KEY3.Valid_Level = KEY3_VALID;
	KEY3.Pressed_Threshold = KEY3_PRESSED_THRESHOLD;
	KEY3.LPressed_Threshold = KEY3_LPRESSED_THRESHOLD;
	KEY3.LLPressed_Threshold = KEY3_LLPRESSED_THRESHOLD;

	GPIO_InitStruct.GPIO_Pin = KEY3_PIN;
	GPIO_Init(KEY3_PORT,&GPIO_InitStruct);

	/*KEY4*/
	KEY4.PORT = KEY4_PORT;
	KEY4.Pin = KEY4_PIN;
	KEY4.Valid_Level = KEY4_VALID;
	KEY4.Pressed_Threshold = KEY4_PRESSED_THRESHOLD;
	KEY4.LPressed_Threshold = KEY4_LPRESSED_THRESHOLD;
	KEY4.LLPressed_Threshold = KEY4_LLPRESSED_THRESHOLD;

	GPIO_InitStruct.GPIO_Pin = KEY4_PIN;
	GPIO_Init(KEY4_PORT,&GPIO_InitStruct);
}

/*********************************************************
*@brief:	读取对象按键引脚输入电平
*@param:	[I](Key_Class * KEY)要查询的按键对象
*@retval:	对象按键的引脚输入电平
*********************************************************/
__inline uint8_t KEY_GetPinValue(Key_Class * KEY)
{
	return GPIO_ReadInputDataBit(KEY->PORT,KEY->Pin);
}

/*********************************************************
*@brief:	按键状态检测
*@param:	[I](Key_Class * KEY)要查询的按键对象
*@retval:	对象按键的当前状态,-2为不允许发生的情况
*@note:		为实时获取按键的状态,该函数建议被system.c中
*			System_Timer_EventHandler循环调用
*********************************************************/
int8_t Update_Keys_State(Key_Class * KEY)
{
	switch(KEY->STA)
	{
		case KEY_STA_RELEASED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//检测按键当前电平状态
			{
				KEY->STA=KEY_STA_WAITTING;						//进入等待消抖状态
				break;
			}
			else
				goto RELEASED;									//视作释放
		}

		case KEY_STA_WAITTING :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//验证按键仍处于有效电平
			{
				if(++ KEY->Wait_ctr >= KEY->Pressed_Threshold)	//消抖完成
				{
					KEY->STA=KEY_STA_PRESSED;					//进入按下状态
					break;
				}
				else
				{
					KEY->STA=KEY_STA_WAITTING;					//仍处于等待状态
					break;
				}
			}
			else
				goto RELEASED;									//视作抖动
		}

		case KEY_STA_PRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//验证按键仍处于有效电平
			{
				if(++ KEY->Wait_ctr >= KEY->LPressed_Threshold)	//长按完成
				{
					KEY->STA=KEY_STA_LPRESSED;					//进入长按状态
					break;
				}
				else
				{
					KEY->STA=KEY_STA_PRESSED;					//视作按下
					break;
				}
			}
			else
				goto RELEASED;									//视作按下结束
		}

		case KEY_STA_LPRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//验证按键仍处于有效电平
			{
				if(++ KEY->Wait_ctr >= KEY->LLPressed_Threshold)//长按完成
				{
					KEY->STA=KEY_STA_LLPRESSED;					//进入超长按状态
					break;
				}
				else
				{
					KEY->STA=KEY_STA_LPRESSED;					//视作长按
					break;
				}
			}
			else
				goto RELEASED;									//视作长按结束
		}

		case KEY_STA_LLPRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//验证按键仍处于有效电平
			{
					KEY->STA=KEY_STA_LLPRESSED;					//仍然处于超长按
					break;
			}
			else
				goto RELEASED;									//视作超长按结束
		}

		case KEY_STA_FORBIDEN :
		{
			KEY->STA=KEY_STA_FORBIDEN;							//视为禁用
			/*解除按键禁用*/
			if( KEY_GetPinValue(KEY) == KEY_STA_RELEASED )
			{
				KEY->STA = KEY_STA_RELEASED;
			}

			break;
		}

		default: return -2;										//不允许的状态值
	}

	return KEY->STA;											//返回状态 

RELEASED:
		KEY->STA = KEY_STA_RELEASED;
		KEY->Wait_ctr = 0;
		return KEY_STA_RELEASED;
}

/************************************************************
*						End of File
************************************************************/
