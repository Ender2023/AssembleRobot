#include <string.h>
#include "AssembleRobot.h"
#include "relay.h"
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
            static uint32_t delay_cnt = 0;

            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if(delay_cnt % 80 == 0)
                {
                    if( debugPage.select > 0 ) {debugPage.select --;}
                    UI_ShowPage(&debugPage);
                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if(delay_cnt % 80 == 0)
                {
                    if( debugPage.select < debugPage.itemCNT - 1 ) {debugPage.select ++;}
                    UI_ShowPage(&debugPage);
                }
            }     

            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                if(KEY3.PreSTA != KEY_STA_LPRESSED)
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
                if(KEY3.PreSTA != KEY_STA_LPRESSED)
                {
                    UI_ShowPage(&debugPage);
                }
            }
            break;
        }

        case UI_debug_Smallarm:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                if(KEY3.PreSTA != KEY_STA_LPRESSED)
                {
                    UI_ShowPage(&debugPage);
                }
            }
            break;
        }

        case UI_debug_UpDown:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                if(KEY3.PreSTA != KEY_STA_LPRESSED)
                {
                    UI_ShowPage(&debugPage);
                }
            }
            break;
        }

        case UI_debug_rotation:
        {
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                if(KEY3.PreSTA != KEY_STA_LPRESSED)
                {
                    UI_ShowPage(&debugPage);
                }
            }
            break;
        }

        case UI_debug_Conveyer:
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

        case UI_Message_Input:
        {
            static uint32_t delay_cnt = 0;

            delay_cnt ++ ;
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( value < value_highest )
                {
                    if(delay_cnt % 10 == 0)
                    {
                        value += 1.0f;
                        LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                        LCD_Printf("now:%.0f    ",value);
                    }

                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( value > value_lowest )
                {
                    if(delay_cnt % 10 == 0)
                    {
                        value -= 1.0f;
                        LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                        LCD_Printf("now:%.0f    ",value);
                    }
                }
            }
            if( strcmp(KEY->name,"KEY3") == 0 )
            {
                UI_routinue = UI_Pre_routinue;      //恢复至之前的页面
            
                switch(UI_routinue)
                {
                    case UI_debug:
                    {
                        UI_ShowPage(&debugPage);
                        break;
                    }

                    case UI_debug_Bigarm:
                    {
                        UI_ShowPage(&debugPage_Bigarm);
                        break;
                    }

                    case UI_debug_Smallarm:
                    {
                        UI_ShowPage(&debugPage_Smallarm);
                        break;
                    }

                    case UI_debug_UpDown:
                    {
                        UI_ShowPage(&debugPage_UpDown);
                        break;
                    }

                    case UI_debug_rotation:
                    {
                        UI_ShowPage(&debugPage_Rotation);
                        break;
                    }

                }

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
            static uint32_t delay_cnt = 0;

            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if(delay_cnt % 20 == 0)
                {
                    if( debugPage.select > 0 ) {debugPage.select --;}
                    UI_ShowPage(&debugPage);
                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if(delay_cnt % 20 == 0)
                {
                    if( debugPage.select < debugPage.itemCNT - 1 ) {debugPage.select ++;}
                    UI_ShowPage(&debugPage);
                }
            }            
                
        }

        case UI_info:
        {

            break;
        }

        case UI_members:
        {

            break;
        }

        case UI_Message_Input:
        {
            static uint32_t delay_cnt = 0;
            delay_cnt ++;

            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( value < value_highest )
                {
                    if(delay_cnt % 5 == 0)
                    {
                        value += 1.0f;
                        LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                        LCD_Printf("now:%.0f    ",value);
                    }
                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( value > value_lowest )
                {
                    if(delay_cnt % 5 == 0)
                    {
                        value -= 1.0f;
                        LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                        LCD_Printf("now:%.0f    ",value);
                    }
                }
            }
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
                    case 5: {UI_ShowPage(&debugPage_Conveyer);break;}
                    case 6:
                    {

                        break;
                    }
                    case 7:
                    {

                        break;
                    }
                    case 8:
                    {
                        roboJoint_Absolute_AngleExecute(&bigARM,0,5,200);
                        roboJoint_Absolute_AngleExecute(&smallARM,0,5,200);
                        roboJoint_Absolute_AngleExecute(&rotationJoint,0,5,200);
                        roboJoint_Absolute_LineExecute(&upDownJoint,0,5,200);
                        UI_ShowMessage_done("All Reset");
                        break;
                    }
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
                        UI_ShowMessage_done("Clamp Jaw catch");
                        break;
                    }
                    case 1: 
                    {
                        Robot_clampJaw_Graspe(true);
                        UI_ShowMessage_done("Clamp Jaw Grasped");
                        break;
                    }
                    case 2: 
                    {
                        Robot_clampJaw_Release(true);
                        UI_ShowMessage_done("Clamp Jaw Release");
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
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_Bigarm.select)
               {
                    case 0: 
                    {
                        roboJoint_Absolute_AngleExecute(&bigARM,0,5,200);
                        UI_ShowMessage_done("Big arm reset");
                        break;
                    }
                    case 1: 
                    {
                        UI_ShowMessage_input("please input value:\n-180 ~ +180",&bigARM);
                        break;
                    }
               }
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
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_Smallarm.select)
               {
                    case 0: 
                    {
                        roboJoint_Absolute_AngleExecute(&smallARM,0,5,200);
                        UI_ShowMessage_done("Small arm reset");
                        break;
                    }
                    case 1: 
                    {
                        UI_ShowMessage_input("please input value:\n-180 ~ +180",&smallARM);
                        break;
                    }
               }
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
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_UpDown.select)
               {
                    case 0: 
                    {
                        roboJoint_Absolute_LineExecute(&upDownJoint,0,5,200);
                        UI_ShowMessage_done("Up/Down joint reset");
                        break;
                    }
                    case 1: 
                    {
                        UI_ShowMessage_input("please input value:\n60 ~ 283",&upDownJoint);
                        break;
                    }
               }
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
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_Rotation.select)
               {
                    case 0: 
                    {
                        roboJoint_Absolute_AngleExecute(&rotationJoint,0,5,200);
                        UI_ShowMessage_done("Rotation joint reset");
                        break;
                    }
                    case 1: 
                    {
                        UI_ShowMessage_input("please input value:\n-180 ~ +180",&rotationJoint);
                        break;
                    }
               }
            }
            break;
        }

        case UI_debug_Conveyer :
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( debugPage_Conveyer.select > 0 ) {debugPage_Conveyer.select --;}
                UI_ShowPage(&debugPage_Conveyer);
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( debugPage_Conveyer.select < debugPage_Conveyer.itemCNT - 1 ) {debugPage_Conveyer.select ++;}
                UI_ShowPage(&debugPage_Conveyer);
            }
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
               switch(debugPage_Conveyer.select)
               {
                    case 0: 
                    {
                        Relay_actuation();
                        Display_Logged(LOG_RANK_WARNNING,"Conveyer running\n");
                        UI_ShowMessage_done("Conveyer running");
                        break;
                    }
                    case 1: 
                    {
                        Relay_Release();
                        Display_Logged(LOG_RANK_WARNNING,"Conveyer stopped\n");
                        UI_ShowMessage_done("Conveyer stopped");
                        break;
                    }
               }
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

        case UI_Message_Done:
        {
            UI_routinue = UI_Pre_routinue;      //恢复至之前的页面
            
            switch(UI_routinue)
            {
                case UI_debug:
                {
                    UI_ShowPage(&debugPage);
                    break;
                }

                case UI_debug_ClampJaw:
                {
                    UI_ShowPage(&debugPage_ClampJaw);
                    break;
                }

                case UI_debug_Bigarm:
                {
                    UI_ShowPage(&debugPage_Bigarm);
                    break;
                }

                case UI_debug_Smallarm:
                {
                    UI_ShowPage(&debugPage_Smallarm);
                    break;
                }

                case UI_debug_UpDown:
                {
                    UI_ShowPage(&debugPage_UpDown);
                    break;
                }

                case UI_debug_rotation:
                {
                    UI_ShowPage(&debugPage_Rotation);
                    break;
                }

                case UI_debug_Conveyer:
                {
                    UI_ShowPage(&debugPage_Conveyer);
                    break;
                }
            }

            break;
        }

        case UI_Message_Input:
        {
            if( strcmp(KEY->name,"KEY1") == 0 )
            {
                if( value < value_highest )
                {
                    value += 1.0f;
                    LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                    LCD_Printf("now:%.0f    ",value);
                }
            }
            else if( strcmp(KEY->name,"KEY2") == 0 )
            {
                if( value > value_lowest )
                {
                    value -= 1.0f;
                    LCD_Set_Printfmt(40,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
                    LCD_Printf("now:%.0f    ",value);
                }
            }
            else if( strcmp(KEY->name,"KEY4") == 0 )
            {
                if(tmp_joint->MoveType == rotation)
                {
                    roboJoint_Absolute_AngleExecute(tmp_joint,value,5,200);
                }
                else
                {
                    roboJoint_Absolute_LineExecute(tmp_joint,value,5,200);   
                }
            }
            
        }

    }    
}
