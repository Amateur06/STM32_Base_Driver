/*
 * @file    Speed.c
 * @brief   旋转编码器底盘速度解算驱动
 * @author  Amateur
 * @date    2026.09.04
 * @version V1.0
 * 
 * @工程分层：Hardware -> Device 传感器业务层
 * @功能概述：
 * 1. 增量式编码器脉冲采样与里程计算
 *    定时周期读取TIM定时器编码器计数值，计算相邻采样间隔脉冲增量
 *    根据编码器线数、车轮半径，换算得到车轮行进距离
 *
 * 2. 轮速解算，输出物理速度(m/s)
 *    脉冲增量÷采样周期，得到左右车轮实时线速度，单位 m/s
 *    使用结构体保存双轮速度，对外提供只读获取接口
 *
 * 3. 数据隔离设计
 *    速度变量设为静态私有，外部只能通过Speed_Get读取，不能直接修改测速结果
 * 
 * @依赖驱动：TIM_Driver.c / TIM_Driver.h 编码器底层定时器驱动
 * @宏定义依赖(放在Speed.h)：
 *    ENCODER_PPR        编码器单圈脉冲数
 *    ENCODER_FREQ_COEFF 减速比
 *    WHEEL_RADIUS_M     车轮半径，单位米
 *    SAMPLE_PERIOD_S    测速采样周期，单位秒
 *
 * @使用方式：
 * - 定时器编码器模式初始化完成后调用 Speed_Init()清零历史计数
 * - 固定定时周期(如10ms)调用 Speed_Sample()执行一次速度采样计算
 * - 控制循环内调用 Speed_Get()获取最新左右轮速度
 * - Speed_Sample必须以固定时间间隔调用，否则速度计算结果出错
 */

#include "Speed.h"
#include "TIM_Driver.h"

// 编码器历史计数器
static int16_t Encoder_Last_Left = 0;
static int16_t Encoder_Last_Right = 0;

// 当前速度 m/s
static Chassis_Speed_t Chassis_Speed = {0.0f, 0.0f};

// 初始化测速，上电清零编码器
void Speed_Init(void)
{
    
    TIM2_ClearEncoder();
    Encoder_Last_Left = TIM2_ReadEncoder();
    Encoder_Last_Right = TIM2_ReadEncoder();
    Chassis_Speed.Left = 0.0f;
    Chassis_Speed.Right = 0.0f;
}

void Speed_Sample(void)
{
    int16_t Now_Cnt, Delta_Cnt;
    float Distance;

    // 左轮测速
    Now_Cnt = TIM2_ReadEncoder();
    Delta_Cnt = Now_Cnt - Encoder_Last_Left;
    Encoder_Last_Left = Now_Cnt;
    
    // 计算左轮速度
    Distance = (float)Delta_Cnt / (ENCODER_PPR * ENCODER_FREQ_COEFF) * 2.0f * 3.1415926f * WHEEL_RADIUS_M;
    Chassis_Speed.Left = Distance / SAMPLE_PERIOD_S;

    // 右轮测速
    Now_Cnt = TIM2_ReadEncoder();
    Delta_Cnt = Now_Cnt - Encoder_Last_Right;
    Encoder_Last_Right = Now_Cnt;

    // 计算右轮速度
    Distance = (float)Delta_Cnt / (ENCODER_PPR * ENCODER_FREQ_COEFF) * 2.0f * 3.1415926f * WHEEL_RADIUS_M;
    Chassis_Speed.Right = Distance / SAMPLE_PERIOD_S;
}

Chassis_Speed_t Speed_Get(void)
{
    return Chassis_Speed;
}
