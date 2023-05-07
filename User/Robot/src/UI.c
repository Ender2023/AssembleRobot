#include <string.h>
#include "bsp_rgblcd.h"
#include "UI.h"

UI_page homePage,debugPage,infoPage,memberPage,

        debugPage_ClampJaw,debugPage_Bigarm,debugPage_Smallarm,
        debugPage_UpDown,debugPage_Rotation;

UI_NAME UI_routinue = UI_info;              //记录当前所在页面，默认系统启动后处于信息界面
UI_NAME UI_Pre_routinue = UI_homepage;      //记录之前所在页面

const char * homePage_item[] = {"Debug Mode","Info","Members","Reboot","\0"};
const char * debugPage_item[] = {"Clamp jaw","Big arm","Small arm","Up/down","Rotation joint",
                                "Start task","Stop task","Reset","\0"};

const char * debugPage_ClampJaw_item[] = {"Catch","Release","\0"};
const char * debugPage_Bigarm_item[] = {"Reset","Move angle","\0"};
const char * debugPage_Smallarm_item[] = {"Reset","Move angle","\0"};
const char * debugPage_UpDown_item[] = {"Reset","Move distance","\0"};
const char * debugPage_Rotation_item[] = {"Reset","Move angle","\0"};

/**
 * @brief:  UI界面初始化
 * @param:  要初始化的页面地址
 * @retval: None
*/
static void UI_Page_Init(UI_page * page,UI_pageInitTypedef * UI_pageInitStruct)
{
    uint8_t ctr = 0;

    page->info.name = UI_pageInitStruct->name;
    page->info.select_Symbol = UI_pageInitStruct->select_Symbol;

    if(UI_pageInitStruct->itemlist == NULL)
    {
        return ;
    }

    while(  **((UI_pageInitStruct->itemlist) + ctr )  != '\0' )
    {
        ctr ++;
    }

    page->itemCNT = ctr ;

    memcpy(page->item,UI_pageInitStruct->itemlist,sizeof(const char *) * ( page->itemCNT + 1 ) );
}

/**
 * @brief:  建立UI界面
*/
void UI_Setup(void)
{
    UI_pageInitTypedef UI_pageInitStruct;

    Display_Logged(LOG_RANK_OK,"Setup UI!\n");

    /*主页面初始化*/
    UI_pageInitStruct.name = UI_homepage;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = homePage_item;
    UI_Page_Init(&homePage,&UI_pageInitStruct);

    /*调试界面初始化*/
    UI_pageInitStruct.name = UI_debug;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_item;
    UI_Page_Init(&debugPage,&UI_pageInitStruct);

    /*调试-夹抓界面初始化*/
    UI_pageInitStruct.name = UI_debug_ClampJaw;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_ClampJaw_item;
    UI_Page_Init(&debugPage_ClampJaw,&UI_pageInitStruct);

    /*调试-大臂界面初始化*/
    UI_pageInitStruct.name = UI_debug_Bigarm;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_Bigarm_item;
    UI_Page_Init(&debugPage_Bigarm,&UI_pageInitStruct);

    /*调试-小臂界面初始化*/
    UI_pageInitStruct.name = UI_debug_Smallarm;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_Smallarm_item;
    UI_Page_Init(&debugPage_Smallarm,&UI_pageInitStruct);

    /*调试-上下关节界面初始化*/
    UI_pageInitStruct.name = UI_debug_UpDown;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_UpDown_item;
    UI_Page_Init(&debugPage_UpDown,&UI_pageInitStruct);
    
    /*调试-旋转关节界面初始化*/
    UI_pageInitStruct.name = UI_debug_rotation;
    UI_pageInitStruct.select_Symbol = "*";
    UI_pageInitStruct.itemlist = debugPage_Rotation_item;
    UI_Page_Init(&debugPage_Rotation,&UI_pageInitStruct);

    /*调试信息输出界面初始化*/
    UI_pageInitStruct.name = UI_info;
    UI_pageInitStruct.itemlist = NULL;
    UI_Page_Init(&infoPage,&UI_pageInitStruct);

    /*制作人员信息输出界面初始化*/
    UI_pageInitStruct.name = UI_members;
    UI_Page_Init(&memberPage,&UI_pageInitStruct);
}

/**
 * @brief:  展现条目列表
 * @param:  页面对象
 * @retval: None
*/
void UI_DrawListItem(UI_page * page)
{
    uint8_t cnt = 0;                    //条目计数器
    const char * pItem;                 //指向当前条目

    while( **(page->item + cnt) != '\0' )
    {
        pItem = *(page->item + cnt);    //更新条目指向

        if(page->select == cnt)         //如果选中该条目
        {
            LCD_Printf("[%s] %s\n",page->info.select_Symbol,pItem);
        }
        else
        {
            LCD_Printf("[ ] %s\n",pItem);
        }

        cnt++;
    }
    
    LCD_Set_Printfmt(0,104,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
    if( page->info.name != UI_homepage )
    {
        LCD_Printf("K1-Up\t\tK2-Down\nK3[L]Back\tK4-Enter");
    }
    else
    {
        LCD_Printf("K1-Up\nK2-Down\tK4-Enter");
    }

}

/**
 * @brief:  展现页面
 * @param:  要显示的页面对象
 * @retval: None
*/
void UI_ShowPage(UI_page * page)
{
    clr;                    //清屏
    LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);

    switch(page->info.name)
    {
        case UI_homepage : 
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug : 
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug_ClampJaw :
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug_Bigarm :
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug_Smallarm :
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug_UpDown :
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_debug_rotation :
        {
            UI_DrawListItem(page);
            break;
        }

        case UI_members:
        {
            LCD_Printf("Author List:");

            LCD_Set_Printfmt(30,42,LCD_COLOR_GREEN,LCD_COLOR_BLACK,12,false);
            LCD_Printf("Pan ZhiKang");
            LCD_Set_Printfmt(30,54,LCD_COLOR_GREEN,LCD_COLOR_BLACK,12,false);
            LCD_Printf("Lin XingWei");
            LCD_Set_Printfmt(30,66,LCD_COLOR_GREEN,LCD_COLOR_BLACK,12,false);
            LCD_Printf("Ye MingChao");
            LCD_Set_Printfmt(0,102,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
            LCD_Printf("Thank you all!");
            LCD_Set_Printfmt(0,116,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
            LCD_Printf("K3[L]-Back");
            break;
        }

        case UI_info:
        {
            LOG_now = LOG_count;        //每次进入信息页面，总是指向最新的记录
            Display_CheckLog(LOG_now);  //查询记录

            break;
        }
    }

    /*确定当前所处的UI位置*/
    UI_routinue = page->info.name;

}

/**
 * @brief:  UI弹窗提示
*/
void UI_ShowMessageBox(void)
{
    UI_Pre_routinue = UI_routinue;                                              //记录之前所在页面
    UI_routinue = UI_MessageBox;                                                //切换到消息栏模态

    /*绘制消息框*/
    LCD_Set_Printfmt(0,30,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
    LCD_Printf("\t+------------+\n\t|\t\t\t|\n\t|\t\t\t|\n\t|\t\t\t|\n\t|\t\t\t|\n\t+------------+\n");
    
    LCD_Set_Printfmt(55,55,LCD_COLOR_YELLOW,LCD_COLOR_BLACK,12,false);
    LCD_Printf("Done!");
    LCD_Set_Printfmt(60,80,LCD_COLOR_BLACK,LCD_COLOR_YELLOW,12,false);
    LCD_Printf("OK");

    /*绘制按键提示信息*/
    LCD_FillColor(0,104,128,128,LCD_COLOR_BLACK);
    LCD_Set_Printfmt(0,116,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
    LCD_Printf("K4-OK");
}

                        
