#include "I2C_Driver.h"

// I2C简单延时
static void I2C_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 120; i++);
}

// SDA切换输出模式
static void I2C_SDA_OutMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
}

// SDA切换输入模式
static void I2C_SDA_InMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
}
// I2C初始化
void I2C_Init(void)
{
    uint32_t i,j;

    //这里添加上电稳定延时
    for(i = 0; i < 1000; i++)
    {
        for(j = 0; j < 1000; j++);
    }

    // 空闲状态，总线全部拉高
    I2C_SDA_OutMode();

    I2C_SCL_HIGH();
    I2C_SDA_HIGH();
}

// I2C起始信号
void I2C_Start(void)
{
    I2C_SDA_OutMode();

    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    I2C_Delay();
    
    I2C_SDA_LOW();
    I2C_Delay();

    I2C_SCL_LOW();
    I2C_Delay();
}

// I2C停止信号
void I2C_Stop(void)
{
    I2C_SDA_OutMode();

    I2C_SDA_LOW();
    I2C_SCL_HIGH();
    I2C_Delay();

    I2C_SDA_HIGH();
    I2C_Delay();
}

// I2C发送一个字节
uint8_t I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if(Byte & 0x80)     // 发送高位
        {
            I2C_SDA_HIGH();
        }
        else                // 发送低位
        {
            I2C_SDA_LOW();
        }
        I2C_Delay();

        I2C_SCL_HIGH();     // 拉高时钟线，数据被接收
        I2C_Delay();

        I2C_SCL_LOW();      // 拉低时钟线，准备发送下一位
        I2C_Delay();

        Byte <<= 1;         // 左移一位，准备发送下一位
    }

    I2C_SDA_HIGH();
    I2C_SDA_InMode();
    I2C_Delay();
    I2C_SCL_HIGH();
    I2C_Delay();

    uint8_t ACK = I2C_SDA_READ(); // 读取ACK信号

    I2C_SCL_LOW();
    I2C_Delay();
    I2C_SDA_OutMode();
    return ACK; // 返回ACK信号
}

// 主机发送应答
void I2C_SendAck(uint8_t ACK)
{
    I2C_SDA_OutMode();
    if(ACK)
    {
        I2C_SDA_HIGH(); // 发送NACK
    }
    else
    {
        I2C_SDA_LOW();  // 发送ACK
    }
    I2C_Delay();

    I2C_SCL_HIGH();
    I2C_Delay();

    I2C_SCL_LOW();
    I2C_Delay();
} 

// I2C接收一个字节
uint8_t I2C_ReadByte(uint8_t ACK)
{
    uint8_t i;
    uint8_t Byte = 0;

    I2C_SDA_InMode(); // 切换为输入模式

    for(i = 0; i < 8; i++)
    {
        Byte <<= 1; // 左移一位，为接收下一位做准备
        I2C_SCL_LOW();
        I2C_Delay();
        I2C_SCL_HIGH(); // 拉高时钟线，准备接收数据
        I2C_Delay();
        if(I2C_SDA_READ()) // 读取SDA线的状态
        {
            Byte |= 0x01; // 如果SDA为高电平，设置Byte的最低位为1
        }

        I2C_SCL_LOW(); // 拉低时钟线，准备接收下一位
        I2C_Delay(); 
        I2C_SendAck(ACK);
        return Byte;
    }
}

// I2C总线软件复位，干扰卡死时调用
void I2C_BusReset(void)
{
    I2C_SDA_OutMode();
    I2C_SCL_HIGH();
    I2C_SDA_HIGH();
    I2C_Delay();

    uint8_t i;
    for( i = 0; i < 9; i++)
    {
        I2C_SCL_LOW();
        I2C_Delay();
        I2C_SCL_HIGH();
        I2C_Delay();
    }
    I2C_Stop();
}
