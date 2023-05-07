#ifndef __UI_H
#define __UI_H

#include "system.h"

#define UI_MAX_ITEM_NUM     16

/*当前所处UI界面*/
typedef enum{UI_homepage = 0,UI_debug,UI_info,UI_members,UI_debug_ClampJaw,
             UI_debug_Bigarm,UI_debug_Smallarm,UI_debug_UpDown,UI_debug_rotation,
             UI_MessageBox,
             }UI_NAME;

typedef struct
{
    UI_NAME             name;                  //所处界面
    const char *        select_Symbol;         //选中符号
    const char **       itemlist;              //初始化条目列表信息

}UI_pageInitTypedef;

typedef struct
{
    UI_pageInitTypedef  info;                  //界面信息
    uint8_t             itemCNT;               //条目数量
    const char *        item[UI_MAX_ITEM_NUM]; //条目信息
    uint8_t             select;                //选中条目
}UI_page;


/*页面对象声明*/
extern UI_page  homePage,debugPage,infoPage,memberPage,
                debugPage_ClampJaw,debugPage_Bigarm,debugPage_Smallarm,
                debugPage_UpDown,debugPage_Rotation;

extern UI_NAME UI_routinue,UI_Pre_routinue;

void UI_Setup(void);
void UI_ShowPage(UI_page * page);
void UI_ShowMessageBox(void);

#endif
