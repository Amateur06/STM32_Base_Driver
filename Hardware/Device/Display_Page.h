#ifndef __DISPLAY_PAGE_H
#define __DISPLAY_PAGE_H
#include "main.h"
#include "JoyStick.h"

typedef enum
{
    DETAIL_PAGE_PID,
    DETAIL_PAGE_VOFA,
    DETAIL_PAGE_SENSOR,
    DETAIL_PAGE_MAX
}DetailPageId_t;

typedef struct
{
    const char* item_name;
    DetailPageId_t detail_page;
}MenuListItem_t;

typedef enum
{
    SYS_MENU_LIST,
    SYS_VIEW_DETAIL
}SysDisplayMode_t;

void Display_Init(void);
void Display_ProcessEvent(KeyEvent_t evt);
void RenderMenuList(void);
void RenderDetailPage(void);

extern uint8_t Menu_Cursor;
extern SysDisplayMode_t Sys_Mode;
extern DetailPageId_t Cur_Detail_Page;

#endif
