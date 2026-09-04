#ifndef __ADC_DRIVER_H
#define __ADC_DRIVER_H

#include "stm32f1xx_hal.h"

/*=== 通道宏定义 ===*/
#define ADC_CH_0            ADC_CHANNEL_0
#define ADC_CH_1            ADC_CHANNEL_1
#define ADC_CH_2            ADC_CHANNEL_2
#define ADC_CH_3            ADC_CHANNEL_3

// 驱动初始化，仅执行ADC校准
void ADC_Init(void);

// 阻塞式读取指定ADC通道单次采样原始值
uint16_t ADC_Driver_GetValue(uint32_t ch);

#endif
