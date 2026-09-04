#ifndef __TIM_Driver_H
#define __TIM_Driver_H

#include "stm32f1xx_hal.h"

/************************ 时基定时器(TIM4) ************************/
// 启动定时中断
void TIM4_Base_StartIT(void);
// 关闭定时中断
void TIM4_Base_StopIT(void);
// 获取控制节拍标志，并且清零
uint8_t TIM4_GetCtrlFlag(void); 

/************************ PWM定时器(TIM3)电机 ************************/
// 开启两路PWM通道
void TIM3_PWM_StartAll(void);
// 设置通道1占空比  0~ARR值
void TIM3_SetDutyCh1(uint16_t duty);
// 设置通道2占空比
void TIM3_SetDutyCh2(uint16_t duty);

/************************ 编码器定时器(TIM2) ************************/
void TIM2_Encoder_Start(void);
//读取计数值
int16_t TIM2_ReadEncoder(void);
//清零编码器计数器
void TIM2_ClearEncoder(void);

#endif
