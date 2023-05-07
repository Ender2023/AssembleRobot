#include "system.h"
#include "bsp_delay.h"
#include "bsp_serial.h"
#include "AssembleRobot.h"
#include "RF.h"
#include "relay.h"
#include "Key.h"
#include "Camera.h"
#include "turnplate.h"

int main(void)
{
    /*系统初始化*/
    System_Init();
    
    // FLASH_Unlock();

    // if(*(uint32_t *)(0x08000000 + 2 * 1024 * 255) == 1 )
	// {
    //     Robot_clampJaw_Release(true);
    //     FLASH_ErasePage(0x08000000 + 2 * 1024 * 255);
	//     FLASH_ProgramWord(0x08000000 + 2 * 1024 * 255,0);
    // }
    // else
    // {
    //     Robot_clampJaw_Catch(true);
    //     FLASH_ErasePage(0x08000000 + 2 * 1024 * 255);
	//     FLASH_ProgramWord(0x08000000 + 2*1024*255,1);
    // }
	// FLASH_Lock();

    while(1);
}


