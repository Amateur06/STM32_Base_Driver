/*
 * @file    TIM_Driver.c
 * @brief   STM32 HAL 通用定时器底层驱动
 * @author  Amateur
 * @date    2026.09.02
 * @version V1.0
 * 
 * @工程分层：Hardware -> Peripheral 通用外设层
 * @功能概述：
 * 1. 封装定时器时基中断功能
 *    支持定时周期中断、定时任务调度，通过传入 TIM_HandleTypeDef 句柄适配任意定时器
 *
 * 2. 封装定时器PWM输出功能
 *    支持单通道/多通道PWM输出、占空比动态修改，可独立控制各路输出通道
 *
 * 3. 封装定时器正交编码器测速功能
 *    定时器硬件编码器接口读取脉冲计数值，软件换算转速与位移，每个编码器实例数据相互隔离
 *
 * @使用方式：
 * - 预先在CubeMX完成定时器时钟、通道、模式基础配置
 * - 根据实际工程需求，开启/注释掉不需要的功能模块代码
 * - main.c 定义对应功能实例，传入对应的定时器句柄与通道编号，调用对应API即可使用
 */

#include "tim.h"

// 引用CubeMX生成的定时器句柄
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

// 中断标志，实现中断置1，主循环读取后置0
static volatile uint8_t Ctrl_TickFlag = 0;

/*======== TIM4 时基中断 ========*/
// 启动定时器4，并且开启中断
void TIM4_Base_StartIT(void)
{
  HAL_TIM_Base_Start_IT(&htim4);
}

// 关闭定时器4中断
void TIM4_Base_StopIT(void)
{
  HAL_TIM_Base_Stop_IT(&htim4);
}

// 判断中断标志有没有呗定时器中断置1，如果置1则返回1，并且清零标志位
uint8_t TIM4_GetCtrtlFlag(void)
{
    uint8_t flag = 0;
    if(Ctrl_TickFlag == 1)
    {
        flag = 1;
        Ctrl_TickFlag = 0; // 读取后清零
    }
    return flag;
}

/*======== TIM3 PWM控制 ========*/
// 统一启动PWM通道
void TIM3_PWM_StartAll(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

// 设置通道1占空比
void TIM3_SetDutyCh1(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
}

// 设置通道2占空比
void TIM3_SetDutyCh2(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, duty);
}

/*======== TIM2 编码器测速 ========*/
// 启动编码器模式
void TIM2_Encoder_Start(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1 | TIM_CHANNEL_2);
}

// 读取编码器计数值
int16_t TIM2_ReadEncoder(void)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim2);
}

// 清零编码器计数器
void TIM2_ClearEncoder(void)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}

/*======== 定时器回调函数 ========*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4) // 判断是否是定时器4中断
    {
        Ctrl_TickFlag = 1; // 设置中断标志位
    }
}
