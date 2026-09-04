#ifndef __VOFA_H
#define __VOFA_H

#include "stm32f1xx_hal.h"
#include "Usart_Driver.h"   // 引入你自己串口底层头文件
#include <stdint.h>

/*==================== 配置区 ====================*/
// 接收缓冲区最大长度
#define VOFA_RX_BUF_SIZE        64
/*================================================*/

void VOFA_SendFireWater(UART_HandleTypeDef *huart, float *data, uint8_t num);
void VOFA_RxProcess(UART_RxHandleTypeDef *rx_handle);


#endif
