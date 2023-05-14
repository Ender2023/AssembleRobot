#ifndef	__BSP_KEYS_H
#define	__BSP_KEYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

/************************************************************
*					����״̬�л��ж�
************************************************************/
#define KEY_EXTI_LINEX				EXTI_Line0
#define KEY_IRQHANDLER				EXTI0_IRQHandler

/************************************************************
*						�������ŷֲ�
************************************************************/
/*KEY1*/
#define KEY1_PORT					GPIOD
#define KEY1_PIN					GPIO_Pin_3
#define KEY1_VALID					1
#define KEY1_PRESSED_THRESHOLD		4
#define KEY1_LPRESSED_THRESHOLD		50 + KEY1_PRESSED_THRESHOLD
#define KEY1_LLPRESSED_THRESHOLD	300 + KEY1_LPRESSED_THRESHOLD

/*KEY2*/
#define KEY2_PORT					GPIOD
#define KEY2_PIN					GPIO_Pin_4
#define KEY2_VALID					1
#define KEY2_PRESSED_THRESHOLD		4
#define KEY2_LPRESSED_THRESHOLD		50 + KEY2_PRESSED_THRESHOLD
#define KEY2_LLPRESSED_THRESHOLD	300 + KEY2_LPRESSED_THRESHOLD

/*KEY3*/
#define KEY3_PORT					GPIOD
#define KEY3_PIN					GPIO_Pin_5
#define KEY3_VALID					1
#define KEY3_PRESSED_THRESHOLD		4
#define KEY3_LPRESSED_THRESHOLD		50 + KEY3_PRESSED_THRESHOLD
#define KEY3_LLPRESSED_THRESHOLD	300 + KEY3_LPRESSED_THRESHOLD

/*KEY4*/
#define KEY4_PORT					GPIOD
#define KEY4_PIN					GPIO_Pin_6
#define KEY4_VALID					1
#define KEY4_PRESSED_THRESHOLD		4
#define KEY4_LPRESSED_THRESHOLD		50 + KEY4_PRESSED_THRESHOLD
#define KEY4_LLPRESSED_THRESHOLD	300 + KEY4_LPRESSED_THRESHOLD

/************************************************************
*					����״̬�����
************************************************************/
typedef enum
{
	KEY_STA_FORBIDEN = -1,
	KEY_STA_RELEASED = 0,
	KEY_STA_WAITTING = 1,
	KEY_STA_PRESSED = 2,
	KEY_STA_LPRESSED = 3,
	KEY_STA_LLPRESSED = 4,
}enum_KEY_STA;

/*���尴����*/
typedef struct
{
	const char * 	name;					//����
	enum_KEY_STA	PreSTA;					//��ǰ��״̬
	enum_KEY_STA	STA;					//��¼������ǰ��״̬

	uint8_t			Valid_Level;			//��������Ч������ƽ
	GPIO_TypeDef * 	PORT;					//�����Ĺ����˿�
	uint16_t		Pin;					//�����Ĺ�������

	uint16_t		Pressed_Threshold;		//������ֵ
	uint16_t		LPressed_Threshold;		//������ֵ
	uint16_t		LLPressed_Threshold;	//��������ֵ
	uint16_t		Wait_ctr;				//��Ĭ������


}Key_Class;

/************************************************************
*				�û�API�ӿڼ������ⲿ��������
************************************************************/
void Keys_Init(void);
uint8_t KEY_GetPinValue(Key_Class * KEY);
int8_t Update_Keys_State(Key_Class * KEY);
void Keys_forbidden(Key_Class * KEY);
void Keys_free(Key_Class * KEY);
void Keys_PressedEventHandler(Key_Class * KEY);
void Keys_LPressedEventHandler(Key_Class * KEY);
void Keys_LLPressedEventHandler(Key_Class * KEY);
void Keys_ReleasedEventHandler(Key_Class * KEY);

extern Key_Class KEY1,KEY2,KEY3,KEY4;

#ifdef __cplusplus
}
#endif
#endif

/************************************************************
*						End of File
************************************************************/
