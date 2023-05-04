#include "Key.h"

void Key_Init(void)
{
    Display_Logged("Init Keys...\n");
    Keys_Init();
    Display_Logged("Keys init done!\n");
}
