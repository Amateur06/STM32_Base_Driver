#ifndef __UART_DRIVER_H
#define __UART_DRIVER_H

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <string.h>


// 接收缓冲区大小
#define UART_RX_BUF_LEN    128

/*=== 串口接收结构体，每一个串口一个缓存 ====*/
typedef struct
{
    UART_HandleTypeDef *huart;          // 串口句柄
    uint8_t rx_buf[UART_RX_BUF_LEN];    // 接收缓冲区
    uint16_t rx_index;                  // 接收数据长度
    uint8_t rx_temp;                    // 临时接收变量
}UART_RxHandleTypeDef;

/* 函数声明 */
/* === 发送接口 === */
// 发送单字节
void UART_SendByte(UART_HandleTypeDef *huart, uint8_t data);
// 发送字符串
void UART_SendString(UART_HandleTypeDef *huart, char *str);
// 发送指定长度数据
void UART_SendData(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len);

/* === 接收接口 === */
// 获取接收到的数据长度
uint16_t UART_GetRxLen(UART_RxHandleTypeDef *rx_handle);
// 读取接收缓冲区
void UART_ReadRxBuf(UART_RxHandleTypeDef *rx_handle, uint8_t *outBuf, uint16_t *len);
// 清除接收缓冲区
void UART_ClearRxBuf(UART_RxHandleTypeDef *rx_handle);
// 开启中断接收
void UART_StartRxIT(UART_RxHandleTypeDef *rx_handle, UART_HandleTypeDef *huart);

#endif
