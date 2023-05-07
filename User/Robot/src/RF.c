#include "RF.h"

/**
 * @breif:  系统RF初始化
*/
void RF_Init(void)
{
    Display_Logged(LOG_RANK_INFO,"Init RF...\n");
    nRf24L01_Init();
    
    if(nRf24L01_ReadREG(RF_CH) == 0xff)
    {
        Display_Logged(LOG_RANK_WARNNING,"No RF module!\n");
    }
    else
    {
        Display_Logged(LOG_RANK_OK,"RF init done!\n");
    }
}

/************************************************************
*						End of File
************************************************************/
