#include "RF.h"

/**
 * @breif:  系统RF初始化
*/
void RF_Init(void)
{
    nRf24L01_Init();
    Display_Logged("Init RF...\n");
    
    if(nRf24L01_ReadREG(RF_CH) == 0)
    {
        Display_Logged("RF init failed!\n");
    }
    else
    {
        Display_Logged("RF init done!\n");
    }
}

/************************************************************
*						End of File
************************************************************/
