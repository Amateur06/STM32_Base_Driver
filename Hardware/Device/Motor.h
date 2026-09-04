#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f1xx_hal.h"
#include "tim.h"

//PWM最大值，和你TIM3的ARR=999保持一致
#define MOTOR_PWM_MAX     999
#define MOTOR_PWM_MIN    -999

//电机编号
typedef enum
{
    MOTOR_LEFT  = 0,
    MOTOR_RIGHT = 1
}Motor_ID_t;

/*======== 对外函数声明 ========*/
//初始化电机
void Motor_Init(void);
//单个电机设置速度  speed: -999 ~ +999，正数正转，负数反转，0停止
void Motor_SetSpeed(Motor_ID_t motor, int16_t speed);
//整车直行
void Motor_Forward(int16_t speed);
//整车后退
void Motor_Backward(int16_t speed);
//整车停止
void Motor_Stop(void);
//差速转向，左速度、右速度
void Motor_SetChassisSpeed(int16_t left_speed,int16_t right_speed);

#endif
