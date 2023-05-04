#include "bsp_keys.h"

Key_Class KEY1,KEY2,KEY3,KEY4;

/************************************************************
*@brief:	�������ų�ʼ��
************************************************************/
void Keys_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	/*��������*/
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
*@brief:	��ȡ���󰴼����������ƽ
*@param:	[I](Key_Class * KEY)Ҫ��ѯ�İ�������
*@retval:	���󰴼������������ƽ
*********************************************************/
__inline uint8_t KEY_GetPinValue(Key_Class * KEY)
{
	return GPIO_ReadInputDataBit(KEY->PORT,KEY->Pin);
}

/*********************************************************
*@brief:	����״̬���
*@param:	[I](Key_Class * KEY)Ҫ��ѯ�İ�������
*@retval:	���󰴼��ĵ�ǰ״̬,-2Ϊ�������������
*@note:		Ϊʵʱ��ȡ������״̬,�ú������鱻system.c��
*			System_Timer_EventHandlerѭ������
*********************************************************/
int8_t Update_Keys_State(Key_Class * KEY)
{
	switch(KEY->STA)
	{
		case KEY_STA_RELEASED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//��ⰴ����ǰ��ƽ״̬
			{
				KEY->STA=KEY_STA_WAITTING;						//����ȴ�����״̬
				break;
			}
			else
				goto RELEASED;									//�����ͷ�
		}

		case KEY_STA_WAITTING :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//��֤�����Դ�����Ч��ƽ
			{
				if(++ KEY->Wait_ctr >= KEY->Pressed_Threshold)	//�������
				{
					KEY->STA=KEY_STA_PRESSED;					//���밴��״̬
					break;
				}
				else
				{
					KEY->STA=KEY_STA_WAITTING;					//�Դ��ڵȴ�״̬
					break;
				}
			}
			else
				goto RELEASED;									//��������
		}

		case KEY_STA_PRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//��֤�����Դ�����Ч��ƽ
			{
				if(++ KEY->Wait_ctr >= KEY->LPressed_Threshold)	//�������
				{
					KEY->STA=KEY_STA_LPRESSED;					//���볤��״̬
					break;
				}
				else
				{
					KEY->STA=KEY_STA_PRESSED;					//��������
					break;
				}
			}
			else
				goto RELEASED;									//�������½���
		}

		case KEY_STA_LPRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//��֤�����Դ�����Ч��ƽ
			{
				if(++ KEY->Wait_ctr >= KEY->LLPressed_Threshold)//�������
				{
					KEY->STA=KEY_STA_LLPRESSED;					//���볬����״̬
					break;
				}
				else
				{
					KEY->STA=KEY_STA_LPRESSED;					//��������
					break;
				}
			}
			else
				goto RELEASED;									//������������
		}

		case KEY_STA_LLPRESSED :
		{
			if( KEY_GetPinValue(KEY) == KEY->Valid_Level )		//��֤�����Դ�����Ч��ƽ
			{
					KEY->STA=KEY_STA_LLPRESSED;					//��Ȼ���ڳ�����
					break;
			}
			else
				goto RELEASED;									//��������������
		}

		case KEY_STA_FORBIDEN :
		{
			KEY->STA=KEY_STA_FORBIDEN;							//��Ϊ����
			/*�����������*/
			if( KEY_GetPinValue(KEY) == KEY_STA_RELEASED )
			{
				KEY->STA = KEY_STA_RELEASED;
			}

			break;
		}

		default: return -2;										//�������״ֵ̬
	}

	return KEY->STA;											//����״̬ 

RELEASED:
		KEY->STA = KEY_STA_RELEASED;
		KEY->Wait_ctr = 0;
		return KEY_STA_RELEASED;
}

/************************************************************
*						End of File
************************************************************/
