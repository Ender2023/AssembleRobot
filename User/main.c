#include "application.h"

int main(void)
{
    System_Init();
    serialDebugInit();
    Robot_Init();
 
    FLASH_Unlock();

    if(*(uint32_t *)(0x08000000+2*1024*255) == 1 )
	{
        CLAMP_JAW_RELEASE();
        FLASH_ErasePage(0x08000000+2*1024*255);
	    FLASH_ProgramWord(0x08000000+2*1024*255,0);

    }
    else
    {
        CLAMP_JAW_CATCH();
        FLASH_ErasePage(0x08000000+2*1024*255);
	    FLASH_ProgramWord(0x08000000+2*1024*255,1);
    }
	FLASH_Lock();

    while(1);
}


