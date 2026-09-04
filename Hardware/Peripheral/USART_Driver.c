/*
 * @file    USART_Driver.c
 * @brief   STM32 HAL 通用串口底层驱动
 * @author  Amateur
 * @date    2026.09.01
 * @version V1.0
 * 
 * @工程分层：Hardware -> Peripheral 通用外设层
 * @功能概述：
 * 1. 封装串口基础发送功能（阻塞式）
 *    支持单字节、字符串、指定长度数据发送
 *    完全通用，通过传入 UART_HandleTypeDef 句柄适配多串口
 * 
 * 2. 封装串口中断接收体系
 *    使用自定义结构体管理接收缓存、计数、临时接收变量
 *    中断回调集成在本文件，驱动独立、结构集中、不依赖业务层
 * 
 * @使用方式：
 * - main.c 定义串口接收实例
 * - 调用 UART_StartRxIT() 开启中断接收
 * - 通过读缓存函数获取上位机数据
 */

#include "USART_Driver.h"

/**
 * @brief  发送单个字节(查询阻塞)
 */
void UART_SendByte(UART_HandleTypeDef *huart, uint8_t data)
{
    HAL_UART_Transmit(huart, &data, 1, 100);
}

/**
 * @brief  发送字符串，自动以'\0'判断结束
 */
void UART_SendString(UART_HandleTypeDef *huart, char *str)
{
    uint16_t len = strlen(str);
    if(len > 0)
    {
        HAL_UART_Transmit(huart, (uint8_t*)str, len, 100);
    }
}

/**
 * @brief  发送指定长度的字节数组
 */
void UART_SendData(UART_HandleTypeDef *huart,uint8_t *buf, uint16_t len)
{
    if(len > 0)
    {
        HAL_UART_Transmit(huart, buf, len, 100);
    }
}

/**
 * @brief 开启串口单字节中断接收
 */
void UART_StartRxIT(UART_RxHandleTypeDef *rx_handle, UART_HandleTypeDef *huart)
{
    rx_handle->huart = huart;
    rx_handle->rx_index = 0;
    HAL_UART_Receive_IT(huart, &rx_handle->rx_temp, 1);     // 开启串口中断接收，每次收到1字节就进入中断回调
}

/**
 * @brief 获取已经收到的数据长度
 */
uint16_t UART_GetRxLen(UART_RxHandleTypeDef *rx_handle)
{
    return rx_handle->rx_index;
}

/**
 * @brief 拷贝接收缓存到外部数组
 */
void UART_ReadRxBuf(UART_RxHandleTypeDef *rx_handle, uint8_t *outBuf, uint16_t *len)
{
    if(outBuf == NULL || len == NULL)
        return;

    *len = rx_handle->rx_index;
    memcpy(outBuf, rx_handle->rx_buf, rx_handle->rx_index); // 将接收缓冲区拷贝到outBuf，拷贝len字节 
    // 需要注意的是memcpy不会自动追加字符串结束符\0,所以如果是字符串需要打印手动添加\0
}

/**
 * @brief 清空接收缓冲区
 */
void UART_ClearRxBuf(UART_RxHandleTypeDef *rx_handle)
{
    rx_handle->rx_index = 0;
    memset(rx_handle->rx_buf, 0, UART_RX_BUF_LEN);          // 将缓冲内存区的数组都置0，也就是将rx_buf清空
}

/**
 * @brief 串口接收中断回调函数
 * 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        extern UART_RxHandleTypeDef uart1_rx_handle;

        // 存入缓冲区，防溢出
        if(uart1_rx_handle.rx_index < UART_RX_BUF_LEN)
        {
            uart1_rx_handle.rx_buf[uart1_rx_handle.rx_index++] = uart1_rx_handle.rx_temp;
        }
        // 重新开启下一次中断接收
        HAL_UART_Receive_IT(uart1_rx_handle.huart, &uart1_rx_handle.rx_temp, 1);
    }
}
