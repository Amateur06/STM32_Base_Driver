/*
 * @file    Display_Page.c
 * @brief   OLED页面菜单与显示业务驱动
 * @author  Amateur
 * @date    2026.09.04
 * @version V1.0
 * 
 * @工程分层：Hardware -> Device 人机交互层
 * @功能概述：
 * 1. 实现两级页面显示框架：主菜单列表页 + 详情数据页
 *    主菜单提供光标上下选择，选中后跳转至对应详情页面
 *    详情页包含PID观测、VOFA串口调试、传感器数据三大监控界面
 *
 * 2. 摇杆按键事件业务处理
 *    接收摇杆扫描输出的按键事件，切换光标、页面跳转与返回
 *    通过全局显示模式变量Sys_Mode区分当前处于菜单页/详情页
 *
 * 3. 数据解耦设计
 *    显示数据全部从全局数据池Data_Pool读取，绘图函数仅负责渲染，
 *    不参与控制逻辑、数据采集，实现数据层与显示层分离
 * 
 * @依赖驱动：OLED.c / OLED.h 、JoyStick摇杆驱动、Data_Pool.h全局数据池
 * @硬件外设：0.96寸I2C‑OLED显示屏
 *
 * @使用方式：
 * - 调用Display_Init()初始化页面状态
 * - 主循环检测摇杆按键事件后调用 Display_ProcessEvent()处理交互
 * - 根据Sys_Mode页面模式，周期性调用RenderMenuList() / RenderDetailPage()刷新画面
 * - 页面刷新放在主循环，禁止在中断内执行OLED刷屏函数
 */

#include "Display_Page.h"
#include "OLED.h"
#include "Data_Pool.h"

uint8_t Menu_Cursor = 0;
SysDisplayMode_t Sys_Mode = SYS_MENU_LIST;
DetailPageId_t Cur_Detail_Page;

static const MenuListItem_t Menu_List[] =
{
    {"PID Data View",      DETAIL_PAGE_PID},
    {"VOFA Debug",         DETAIL_PAGE_VOFA},
    {"Sensor Data",        DETAIL_PAGE_SENSOR}
};
#define MENU_ITEM_COUNT  (sizeof(Menu_List)/sizeof(MenuListItem_t))


void RenderMenuList(void)
{
    uint8_t i;
    uint16_t y;
    OLED_Clear();
    OLED_ShowString(0, 0,"====MENU LIST====",OLED_8X16);
    for(i = 0; i < MENU_ITEM_COUNT; i++)
    {
        y = 16 + i*16;
        if(i == Menu_Cursor)
        {
            OLED_ShowString(0, y, "> ", OLED_8X16);
            OLED_ShowString(16, y, Menu_List[i].item_name, OLED_8X16);
        }
        else
        {
            OLED_ShowString(0, y, "  ", OLED_8X16);
            OLED_ShowString(16, y, Menu_List[i].item_name, OLED_8X16);
        }
    }
    OLED_Update();
}

void RenderDetailPage(void)
{
    OLED_Clear();
    switch(Cur_Detail_Page)
    {
        case DETAIL_PAGE_PID:
            OLED_ShowString(0, 0,  "====PID MONITOR====",OLED_6X8);
            OLED_ShowString(0, 8,  "Target:",OLED_6X8);
            OLED_ShowFloatNum(30,8, PID_mon.Target,4,2,OLED_6X8);

            OLED_ShowString(0, 16, "Actual: ",OLED_6X8);
            OLED_ShowFloatNum(30,16,PID_mon.Actual,4,2,OLED_6X8);

            OLED_ShowString(0, 24, "Output:",OLED_6X8);
            OLED_ShowFloatNum(30,24,PID_mon.Output,4,2,OLED_6X8);

            OLED_ShowString(0, 32, "P:",OLED_6X8);
            OLED_ShowFloatNum(36,32,PID_mon.P,4,2,OLED_6X8);

            OLED_ShowString(64,32,"I:",OLED_6X8);
            OLED_ShowFloatNum(100,32,PID_mon.I,4,2,OLED_6X8);

            OLED_ShowString(0, 40, "D:",OLED_6X8);
            OLED_ShowFloatNum(36,40,PID_mon.D,4,2,OLED_6X8);

            OLED_ShowString(0,56,"[OK]Back",OLED_6X8);
            break;

        case DETAIL_PAGE_VOFA:
            OLED_ShowString(0, 0, "====VOFA DEBUG====",OLED_6X8);
            OLED_ShowString(0,8, "Valid:",OLED_6X8);
            if(Vofa_rx_mon.valid)
            {
                OLED_ShowString(42,8,"YES",OLED_6X8);
            }
            else
            {
                OLED_ShowString(42,8,"NO ",OLED_6X8);
            }

            OLED_ShowString(0,16,"RX_BUF:",OLED_6X8);
            OLED_ShowString(0,24,Vofa_rx_mon.str,OLED_6X8);

            OLED_ShowString(0,56,"[OK]Back",OLED_6X8);
            break;

        case DETAIL_PAGE_SENSOR:
            OLED_ShowString(0, 0, "====SENSOR DATA====",OLED_6X8);
            OLED_ShowString(0, 8, "Speed:  ",OLED_6X8);
            OLED_ShowFloatNum(48,8, Sensor_mon.speed,4,2,OLED_6X8);

            OLED_ShowString(0,16, "Voltage:",OLED_6X8);
            OLED_ShowFloatNum(48,16,Sensor_mon.voltage,4,2,OLED_6X8);

            OLED_ShowString(0,24, "Angle:  ",OLED_6X8);
            OLED_ShowFloatNum(48,24,Sensor_mon.angle,4,2,OLED_6X8);

            // 预留，如果你后续要加更多传感器可以继续往下y=32,40...
            OLED_ShowString(0,56,"[OK]Back",OLED_6X8);
            break;

        default:
            OLED_ShowString(0,0,"UNKNOWN PAGE",OLED_6X8);
            OLED_ShowString(0,8,"[OK]Back",OLED_6X8);
            break;
    }
    OLED_Update();
}

void Display_ProcessEvent(KeyEvent_t evt)
{
    if(Sys_Mode == SYS_MENU_LIST)
    {
        switch(evt)
        {
            case KEY_EVENT_UP:
                if(Menu_Cursor > 0)
                    Menu_Cursor--;
                break;
            case KEY_EVENT_DOWN:
                if(Menu_Cursor < MENU_ITEM_COUNT-1)
                    Menu_Cursor++;
                break;
            case KEY_EVENT_OK:
                Cur_Detail_Page = Menu_List[Menu_Cursor].detail_page;
                Sys_Mode = SYS_VIEW_DETAIL;
                break;
            default: break;
        }
    }
    else if(Sys_Mode == SYS_VIEW_DETAIL)
    {
        if(evt == KEY_EVENT_OK)
        {
            Sys_Mode = SYS_MENU_LIST;
        }
    }
}

void Display_Init(void)
{
    Sys_Mode = SYS_MENU_LIST;
    Menu_Cursor = 0;
}
