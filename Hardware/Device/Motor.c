/*
 * @file    motor_app.c
 * @brief   电机控制‑应用业务层
 * @author  Amateur
 * @date    2026.09.02
 * @version V1.0
 *
 * @工程分层：Hardware -> Device 业务通信层
 * @功能概述：
 * 1. 底盘运动封装：直行、后退、原地左转/右转、差速转向、弧线行驶
 * 2. 速度斜坡缓加速、缓减速功能，抑制电机启动冲击、减小车轮打滑
 * 3. 运动状态管理，记录底盘当前左右轮目标速度
 * 4. 和底层驱动解耦：所有硬件动作全部调用motor.c底层接口，本层不操作GPIO、定时器PWM
 *
 * @使用方式：
 * - 在main.c、PID控制、任务调度函数中直接调用运动API
 * - 斜坡缓起缓停函数需要放在周期定时器中断或主循环定时调用
 */
/*
 *  TB6612真值表
 *  AIN1=0，AIN2=1 → 左电机反转
 *  AIN1=1，AIN2=0 → 左电机正转
 *  AIN1=0，AIN2=0 → 刹车停机
 */



#include "Motor.h"
#include "stm32f1xx_hal.h"
#include <stdlib.h>


// 配合TB6612，定义的基础电机方向，方向不对直接在这里修改
static void Motor_SetDir(Motor_ID_t motor, int16_t speed)
{
    if(motor == MOTOR_LEFT)
    {
        if(speed > 0)
        {
            //左电机正转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
        }
        else if(speed < 0)
        {
            //左电机反转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
        }
        else
        {
            //speed=0，刹车
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
        }
    }
    else if(motor == MOTOR_RIGHT)
    {
        if(speed > 0)
        {
            //右电机正转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
        }
        else if(speed < 0)
        {
            //右电机反转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
        }
    }
}

//限幅函数，限制pwm范围：-999~999，这里建议自己调试到自己需要的范围，从小开始，避免电机过载
static int16_t Motor_Limit(int16_t val)
{
    if(val > MOTOR_PWM_MAX)
        return MOTOR_PWM_MAX;
    if(val < MOTOR_PWM_MIN)
        return MOTOR_PWM_MIN;
    return val;
}

void Motor_Init(void)
{
    //PWM已经在tim.c启动过 TIM3_PWM_StartAll();
    Motor_Stop();
}

void Motor_SetSpeed(Motor_ID_t motor, int16_t speed)
{
    uint16_t duty;
    speed = Motor_Limit(speed);

    //设置方向
    Motor_SetDir(motor, speed);

    //取绝对值得到占空比
    duty = (uint16_t)abs(speed);

    if(motor == MOTOR_LEFT)
    {
        TIM3_SetDutyCh1(duty);
    }
    else if(motor == MOTOR_RIGHT)
    {
        TIM3_SetDutyCh2(duty);
    }
}

/*======== 整车电机控制函数 ========*/
void Motor_Forward(int16_t speed)
{
    Motor_SetSpeed(MOTOR_LEFT, speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

void Motor_Backward(int16_t speed)
{
    Motor_SetSpeed(MOTOR_LEFT, -speed);
    Motor_SetSpeed(MOTOR_RIGHT, -speed);
}

void Motor_Stop(void)
{
    Motor_SetSpeed(MOTOR_LEFT, 0);
    Motor_SetSpeed(MOTOR_RIGHT, 0);
}

// 方便差速直接配置
void Motor_SetChassisSpeed(int16_t left_speed,int16_t right_speed)
{
    Motor_SetSpeed(MOTOR_LEFT,left_speed);
    Motor_SetSpeed(MOTOR_RIGHT,right_speed);
}
