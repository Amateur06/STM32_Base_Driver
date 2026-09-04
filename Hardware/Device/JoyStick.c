/*
 * @file    JoyStick.c
 * @brief   模拟摇杆+按键扫描业务驱动
 * @author  Amateur
 * @date    2026.09.04
 * @version V1.0
 * 
 * @工程分层：Hardware -> Device 人机交互层
 * @功能概述：
 * 1. 实现双轴模拟摇杆ADC采样与方向判定
 *    设置摇杆死区阈值，过滤摇杆中位漂移；
 *    通过X/Y轴ADC采样值，识别上、下、左、右四个方向偏移
 *
 * 2. 摇杆与OK按键上升沿事件检测
 *    保存上一次扫描状态，仅在按键按下瞬间触发一次事件
 *    OK按键增加基于系统滴答定时器的软件消抖，防止机械抖动误触发
 * 
 * @依赖驱动：ADC_Driver.c / ADC_Driver.h
 * @硬件引脚：
 *    X轴模拟量  -> ADC_CH_0
 *    Y轴模拟量  -> ADC_CH_1
 *    OK确认按键 -> PA4 (低电平有效)
 *
 * @使用方式：
 * - ADC底层初始化完成后无需额外初始化函数
 * - 在主循环周期调用 Joystick_Scan() 完成摇杆扫描
 * - 根据返回的 KeyEvent_t 事件值执行对应的业务逻辑
 * - 禁止在串口/ADC中断内调用本扫描函数
 */

#include "JoyStick.h"
#include "ADC_Driver.h"
#include "stm32f1xx_hal.h"

#define JOY_DEAD_ZONE_MIN     1500U
#define JOY_DEAD_ZONE_MAX     2600U

static uint8_t Last_Up    = 0;
static uint8_t Last_Down  = 0;
static uint8_t Last_Left  = 0;
static uint8_t Last_Right = 0;
static uint8_t Last_Ok    = 0;

static uint32_t ok_debounce = 0;

KeyEvent_t Joystick_Scan(void)
{
    uint32_t now = HAL_GetTick();

    uint16_t ADC_X = ADC_Driver_GetValue(ADC_CH_0);
    uint16_t ADC_Y = ADC_Driver_GetValue(ADC_CH_1);

    uint8_t Current_Up    = 0;
    uint8_t Current_Down  = 0;
    uint8_t Current_Left  = 0;
    uint8_t Current_Right = 0;
    uint8_t Current_Ok    = 0;

    //Y轴
    if(ADC_Y < JOY_DEAD_ZONE_MIN)
        Current_Down = 1;
    else if(ADC_Y > JOY_DEAD_ZONE_MAX)
        Current_Up = 1;

    //X轴
    if(ADC_X < JOY_DEAD_ZONE_MIN)
        Current_Left = 1;
    else if(ADC_X > JOY_DEAD_ZONE_MAX)
        Current_Right = 1;

    //PA4机械按键
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET)
    {
        Current_Ok = 1;
    }

    KeyEvent_t Event = KEY_EVENT_NONE;

    if(Current_Up && !Last_Up)              Event = KEY_EVENT_UP;
    else if(Current_Down  && !Last_Down)    Event = KEY_EVENT_DOWN;
    else if(Current_Left  && !Last_Left)    Event = KEY_EVENT_LEFT;
    else if(Current_Right && !Last_Right)   Event = KEY_EVENT_RIGHT;

    if( (Current_Ok == 1) && (Last_Ok == 0) && ((now - ok_debounce) > 200) )
    {
        Event = KEY_EVENT_OK;
        ok_debounce = now;
    }

    Last_Up    = Current_Up;
    Last_Down  = Current_Down;
    Last_Left  = Current_Left;
    Last_Right = Current_Right;
    Last_Ok    = Current_Ok;

    return Event;
}
