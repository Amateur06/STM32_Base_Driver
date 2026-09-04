#ifndef __I2C_DRIVER_H
#define __I2C_DRIVER_H

#include "stm32f1xx_hal.h"

/*======== I2C总线定义 ========*/
// 清晰I2C总线定义
#define I2C_SCL_PORT GPIOB
#define I2C_SCL_PIN GPIO_PIN_8

#define I2C_SDA_PORT GPIOB
#define I2C_SDA_PIN GPIO_PIN_9

// I2C总线操作宏
#define I2C_SCL_HIGH()      HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET)
#define I2C_SCL_LOW()       HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET)

#define I2C_SDA_HIGH()      HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET)
#define I2C_SDA_LOW()       HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_RESET)
#define I2C_SDA_READ()      HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)
/*======== 函数配置层 ========*/

// I2C初始化
void I2C_Init(void);
// I2C起始信号
void I2C_Start(void);
// I2C停止信号
void I2C_Stop(void);
// I2C发送字节
uint8_t I2C_SendByte(uint8_t Byte);
// I2C发送相应
void I2C_SendAck(uint8_t ack);
// I2C接收字节
uint8_t I2C_ReadByte(uint8_t ack);
// I2C总线复位
void I2C_BusReset(void);

#endif
