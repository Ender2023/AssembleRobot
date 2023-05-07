#include <string.h>
#include "AssembleRobot.h"
#include "Key.h"
#include "UI.h"

void Key_Init(void)
{
    Display_Logged(LOG_RANK_INFO,"Init Keys...\n");
    Keys_Init();
    Keys_forbidden(&KEY1);
    Keys_forbidden(&KEY2);
    Keys_forbidden(&KEY3);
    Keys_forbidden(&KEY4);
    Display_Logged(LOG_RANK_OK,"Keys init done!\n");
}

/**
 * @brief:按键按下事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_PressedEventHandler(Key_Class * KEY)
{
    // if( KEY->STA != KEY->PreSTA )
    // {
    //     Display_Logged(LOG_RANK_INFO,"%s:pressed!\n",KEY->name);
    // }

    switch(UI_routinue)
    {
        case UI_homepage:
        {

            break;
        }

        case UI_debug:
        {

        }

        case UI_info:
        {


            break;
        }

        case UI_members:
        {

            break;
        }

    }
        
}

/**
 * @brief:按键长按事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_LPressedEventHandler(Key_Class * KEY)
{
    // if( KEY->STA != KEY->PreSTA )
    // {
    //     Display_Logged(LOG_RANK_INFO,"%s:Lpressed!\n",KEY->name);
    // }

    switch(UI_routinue)
    {
        case UI_homepage:
        {
            break;
        }

        case UI_debug:
        {
            if(KEY3.PreSTA != KEY_STA_LPRESSED)
            {
                if( strcmp(KEY->name,"KEY3") == 0 )
                {
                    UI_ShowPage(&homePage);
                }
            }

            break;
        }

        case UI_debug_ClampJaw:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&debugPage);
            }
            break;
        }

        case UI_debug_Bigarm:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&debugPage);
            }
            break;
        }

        case UI_debug_Smallarm:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&debugPage);
            }
            break;
        }

        case UI_debug_UpDown:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&debugPage);
            }
            break;
        }

        case UI_debug_rotation:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&debugPage);
            }
            break;
        }

        case UI_info:
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( LOG_now >= DISPLAY_FIFO_ROW )
                {
                    LOG_now --;
                    Display_CheckLog(LOG_now);
                }


            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( LOG_now < LOG_count )
                {
                    LOG_now ++;
                    Display_CheckLog(LOG_now);
                }

            }

            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&homePage);
            }
            break;
        }

        case UI_members:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_ShowPage(&homePage);
            }
            break;
        }
    }
}

/**
 * @brief:按键超长按事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_LLPressedEventHandler(Key_Class * KEY)
{
    // if( KEY->STA != KEY->PreSTA )
    // {
    //     Display_Logged(LOG_RANK_INFO,"%s:LLpressed!\n",KEY->name);
    // }
    
    switch(UI_routinue)
    {
        case UI_homepage:
        {

            break;
        }

        case UI_debug:
        {

        }

        case UI_info:
        {

            break;
        }

        case UI_members:
        {

            break;
        }

    }
}

/**
 * @brief:按键释放事件处理函数
 * @param: KEY：按键对象
 * @retval:	None
*/
void Keys_ReleasedEventHandler(Key_Class * KEY)
{
    // if( KEY->STA != KEY->PreSTA )
    // {
    //     Display_Logged(LOG_RANK_INFO,"%s:released!\n",KEY->name);   
    // }

    switch(UI_routinue)
    {
        case UI_homepage:
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( homePage.select > 0 ) {homePage.select --;}
                UI_ShowPage(&homePage);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( homePage.select < homePage.itemCNT - 1 ) {homePage.select ++;}
                UI_ShowPage(&homePage);
            }
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(homePage.select)
               {
                    case 0: {UI_ShowPage(&debugPage);break;}
                    case 1: {UI_ShowPage(&infoPage);break;}
                    case 2: {UI_ShowPage(&memberPage);break;}
                    case 3: {NVIC_SystemReset();}
               }
            }

            break;
        }

        case UI_debug:
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage.select > 0 ) {debugPage.select --;}
                UI_ShowPage(&debugPage);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage.select < debugPage.itemCNT - 1 ) {debugPage.select ++;}
                UI_ShowPage(&debugPage);
            }
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage.select)
               {
                    case 0: {UI_ShowPage(&debugPage_ClampJaw);break;}
                    case 1: {UI_ShowPage(&debugPage_Bigarm);break;}
                    case 2: {UI_ShowPage(&debugPage_Smallarm);break;}
                    case 3: {UI_ShowPage(&debugPage_UpDown);break;}
                    case 4: {UI_ShowPage(&debugPage_Rotation);break;}
               }
            }
            break;
        }

        case UI_debug_ClampJaw :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_ClampJaw.select > 0 ) {debugPage_ClampJaw.select --;}
                UI_ShowPage(&debugPage_ClampJaw);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_ClampJaw.select < debugPage_ClampJaw.itemCNT - 1 ) {debugPage_ClampJaw.select ++;}
                UI_ShowPage(&debugPage_ClampJaw);
            }
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_ClampJaw.select)
               {
                    case 0: 
                    {
                        Robot_clampJaw_Catch(true);
                        UI_ShowMessageBox();
                        break;
                    }
                    case 1: 
                    {
                        Robot_clampJaw_Release(true);
                        UI_ShowMessageBox();
                        break;
                    }
               }
            }
            break;
        }

        case UI_debug_Bigarm :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_Bigarm.select > 0 ) {debugPage_Bigarm.select --;}
                UI_ShowPage(&debugPage_Bigarm);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_Bigarm.select < debugPage_Bigarm.itemCNT - 1 ) {debugPage_Bigarm.select ++;}
                UI_ShowPage(&debugPage_Bigarm);
            }
            break;
        }

        case UI_debug_Smallarm :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_Smallarm.select > 0 ) {debugPage_Smallarm.select --;}
                UI_ShowPage(&debugPage_Smallarm);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_Smallarm.select < debugPage_Smallarm.itemCNT - 1 ) {debugPage_Smallarm.select ++;}
                UI_ShowPage(&debugPage_Smallarm);
            }
            break;
        }

        case UI_debug_UpDown :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_UpDown.select > 0 ) {debugPage_UpDown.select --;}
                UI_ShowPage(&debugPage_UpDown);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_UpDown.select < debugPage_UpDown.itemCNT - 1 ) {debugPage_UpDown.select ++;}
                UI_ShowPage(&debugPage_UpDown);
            }
            break;
        }

        case UI_debug_rotation :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_Rotation.select > 0 ) {debugPage_Rotation.select --;}
                UI_ShowPage(&debugPage_Rotation);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_Rotation.select < debugPage_Rotation.itemCNT - 1 ) {debugPage_Rotation.select ++;}
                UI_ShowPage(&debugPage_Rotation);
            }
            break;
        }

        case UI_info:
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( LOG_now >= DISPLAY_FIFO_ROW )
                {
                    LOG_now --;
                    Display_CheckLog(LOG_now);
                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( LOG_now < LOG_count )
                {
                    LOG_now ++;
                    Display_CheckLog(LOG_now);
                }
            }

            break;
        }

        case UI_members:
        {

            break;
        }

        case UI_MessageBox:
        {
            UI_routinue = UI_Pre_routinue;      //恢复至之前的页面
            
            switch(UI_routinue)
            {
                case UI_debug_ClampJaw:
                {
                    UI_ShowPage(&debugPage_ClampJaw);
                    break;
                }
            }

            break;
        }

    }
    
}
