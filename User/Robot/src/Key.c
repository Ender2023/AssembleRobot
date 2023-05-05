#include "Key.h"

void Key_Init(void)
{
    Display_Logged("Init Keys...\n");
    Keys_Init();
    Display_Logged("Keys init done!\n");
}

/**
 * @brief:按键按下事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_PressedEventHandler(Key_Class * KEY)
{
    Display_Logged("%s:pressed!\n",KEY->name);
}

/**
 * @brief:按键长按事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_LPressedEventHandler(Key_Class * KEY)
{
    Display_Logged("%s:Lpressed!\n",KEY->name);
}

/**
 * @brief:按键超长按事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_LLPressedEventHandler(Key_Class * KEY)
{
    Display_Logged("%s:LLpressed!\n",KEY->name);
}

/**
 * @brief:按键释放事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_ReleasedEventHandler(Key_Class * KEY)
{
    Display_Logged("%s:released!\n",KEY->name);
}
