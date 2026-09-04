#include "ADC_Driver.h"

// 引用全局ADC句柄
extern ADC_HandleTypeDef hadc1;

/*======== ADC读取代码 ========*/
// ADC校准初始化
void ADC_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
}

// 单词软件触发采样
uint16_t ADC_Driver_GetValue(uint32_t ch)
{
    uint16_t ADC_Raw = 0U;

    ADC_ChannelConfTypeDef Channel_Cfg = {0};

    // 配置规则组通道，每次采样动态修改，兼容多路轮询采样
    Channel_Cfg.Channel = ch;
    Channel_Cfg.Rank = ADC_REGULAR_RANK_1;
    Channel_Cfg.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

    if(HAL_ADC_ConfigChannel(&hadc1, &Channel_Cfg) != HAL_OK)
    {
        return 0U;
    }

    // 软件启动一次ADC转换
    if(HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return 0U;
    }

    // 阻塞等待转换完成，超时10ms防止程序卡死
    if(HAL_ADC_PollForConversion(&hadc1, 10U) == HAL_OK)
    {
        ADC_Raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);

    return ADC_Raw;
}
