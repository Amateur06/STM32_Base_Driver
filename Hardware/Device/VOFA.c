/*
 * @file    VOFA_Driver.c
 * @brief   VOFA+ 上位机通信业务驱动
 * @author  Amateur
 * @date    2026.09.01
 * @version V1.0
 * 
 * @工程分层：Hardware -> Device 业务通信层
 * @功能概述：
 * 1. 实现VOFA+ FireWater协议浮点数据流发送
 *    帧格式：数据1,数据2,数据3\n，末尾换行符作为帧结束标志
 *    完全复用USART_Driver底层串口发送接口，不直接调用HAL库发送函数
 *
 * 2. 上位机下发指令解析
 *    依托底层串口单字节中断接收框架
 *    在主循环轮询读取接收缓冲区，解析文本指令
 *    指令解析放在业务层，不阻塞串口中断，保证接收实时性
 * 
 * @依赖驱动：USART_Driver.c / USART_Driver.h
 * @串口默认参数：波特率115200，8‑N‑1
 *
 * @使用方式：
 * - 初始化串口后调用 UART_StartRxIT() 开启底层中断接收
 * - 主循环调用 VOFA_SendFireWater() 向上位机发送浮点波形数据
 * - 主循环轮询调用 VOFA_RxProcess() 处理上位机下发指令
 */
#include "VOFA.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief  发送FireWater协议浮点数据流给VOFA+
 * @param  huart:串口句柄
 * @param  data:浮点数组
 * @param  num:变量个数
 * @retval none
 */
void VOFA_SendFireWater(UART_HandleTypeDef *huart, float *data, uint8_t num)
{
    char tx_buf[128];
    uint16_t i;
    uint16_t str_len = 0;

    for(i = 0; i < num; i++)
    {
        str_len += sprintf(tx_buf + str_len, "%.3f", data[i]);  // 这里直接使用指针加法运算，从之前写入的末尾继续写入新的数据，避免覆盖 
        // 把数字格式化，转换成字符串，写入到字符数组中，返回写入的字符数个数
        if(i != num - 1)
        {
            tx_buf[str_len++] = ',';
        }
    }
    // FireWater帧结束换行符
    tx_buf[str_len++] = '\n';
    tx_buf[str_len] = '\0';

    // 调用你自己封装好的底层发送字符串接口，这里由于输入的数据不固定长度，所以使用字符串发送接口，避免手动计算长度
    UART_SendString(huart, tx_buf);
}

/**
 * @brief 解析VOFA上位机下发指令
 * @note  在主循环轮询调用
 * @param  rx_handle:你封装的串口接收句柄
 */
void VOFA_RxProcess(UART_RxHandleTypeDef *rx_handle)
{
    uint16_t rx_len;
    uint8_t temp_buf[VOFA_RX_BUF_SIZE];

    rx_len = UART_GetRxLen(rx_handle);
    if(rx_len == 0)
    {
        return;
    }

    // 将收到的数据拷贝出来
    UART_ReadRxBuf(rx_handle, temp_buf, &rx_len);
    // 手动添加字符串结束符，strstr才能正常解析
    temp_buf[rx_len] = '\0';

    /* ======== 指令解析示例 ======== */ 
    if(strstr((char*)temp_buf, "led_on") != NULL)
    {
        // 打开LED
    }
    else if(strstr((char*)temp_buf, "led_off") != NULL)
    {
        // 关闭LED
    }

    // 清空接收缓冲区，准备下一帧
    UART_ClearRxBuf(rx_handle);
}
