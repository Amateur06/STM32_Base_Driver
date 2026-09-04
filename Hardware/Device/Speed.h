#ifndef __SPEED_H
#define __SPEED_H

#include "stm32f1xx_hal.h"
#include "TIM_Driver.h"

/*======== 硬件参数，根据实际修改 ========*/
#define ENCODER_PPR             100U    // 编码器每转脉冲数
#define WHEEL_RADIUS_M          0.03f   // 轮子半径（米）
#define SAMPLE_PERIOD_S         0.005f  // 采样周期 5ms
#define ENCODER_FREQ_COEFF      4U      // 4倍频

// 保存左右轮速度
typedef struct
{
    float Left;
    float Right;
} Chassis_Speed_t;

// 初始化测速
void Speed_Init(void);
// 一次采样计算，放在TIM4 5ms节拍里面调用
void Speed_Sample(void);
// 获取当前左右轮速度（m/s）
Chassis_Speed_t Speed_Get(void);

#endif
