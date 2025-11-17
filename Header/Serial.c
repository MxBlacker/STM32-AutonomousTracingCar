#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#include "Init.h"

/* ==============================================================================================
                                        全局变量定义
   ============================================================================================== */

/**
 * @brief 串口接收数据缓冲区
 * @note 虽然分配了6字节空间，但速度值范围-1000~1000实际需要5字节
 *       格式: "@数据%" 或 "@-数据%"
 */
uint8_t Serial_RxData[6];

/**
 * @brief 串口接收完成标志
 * @note 当标志为1时表示收到完整数据包，可读取Serial_RxData
 */
int Serial_RxFlag = 0;

/* ==============================================================================================
                                        USART1初始化函数
   ============================================================================================== */

/**
 * @brief USART1串口初始化
 * @note 配置波特率9600，8数据位，1停止位，无校验位
 *       使用GPIOA9(TX)和GPIOA10(RX)，启用接收中断
 */
void USART1_Serial_Init(void)
{
    /* ======================================================================
                               Step 1: 时钟使能
       ====================================================================== */
    
    // 使能USART1和GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* ======================================================================
                               Step 2: GPIO配置
       ====================================================================== */
    
    // PA9: USART1_TX - 复用推挽输出
    AutoInitGPIO(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_9, GPIO_Speed_50MHz);
    // PA10: USART1_RX - 上拉输入
    AutoInitGPIO(GPIOA, GPIO_Mode_IPU, GPIO_Pin_10, GPIO_Speed_50MHz);

    /* ======================================================================
                               Step 3: USART参数配置
       ====================================================================== */
    
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;                           // 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      // 收发模式
    USART_InitStructure.USART_Parity = USART_Parity_No;                  // 无校验位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;               // 1停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;          // 8数据位
    
    USART_Init(USART1, &USART_InitStructure);

    /* ======================================================================
                               Step 4: 中断配置
       ====================================================================== */
    
    // 使能接收缓冲区非空中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 配置NVIC中断
    AutoInitNVIC(NVIC_PriorityGroup_2, USART1_IRQn, 1, 0);
    
    /* ======================================================================
                               Step 5: 使能USART
       ====================================================================== */
    
    USART_Cmd(USART1, ENABLE);
}

/* ==============================================================================================
                                        数据发送函数
   ============================================================================================== */

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节数据
 */
void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    // 等待发送完成
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * @brief 发送字节数组
 * @param Array 要发送的数组指针
 * @param Length 数组长度
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    for (int index = 0; index < Length; index++) {
        Serial_SendByte(Array[index]);
    }
}

/**
 * @brief 发送字符串
 * @param String 要发送的字符串指针(以'\0'结尾)
 */
void Serial_SendString(char *String)
{
    for (int index = 0; String[index] != '\0'; index++) {
        Serial_SendByte(String[index]);
    }
}

/**
 * @brief 格式化输出函数(类似printf)
 * @param format 格式化字符串
 * @param ... 可变参数列表
 * 
 * @example 
 * Serial_Printf("Speed: %d, Angle: %d\r\n", speed, angle);
 */
void Serial_Printf(char *format, ...)
{
    char Temp_String[100];   // 临时格式化缓冲区
    
    // 处理可变参数
    va_list arg;
    va_start(arg, format);
    vsprintf(Temp_String, format, arg);
    va_end(arg);
    
    // 发送格式化后的字符串
    Serial_SendString(Temp_String);
}

/* ==============================================================================================
                                        数据接收函数
   ============================================================================================== */

/**
 * @brief 获取串口接收标志
 * @return 1: 收到新数据, 0: 无新数据
 * @note 读取后自动清除标志位，保证数据不重复处理
 */
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag == 1) {
        Serial_RxFlag = 0;   // 读取后清除标志
        return 1;
    }
    return 0;
}

/**
 * @brief 转换接收数据为整数值
 * @return 转换后的16位有符号整数
 * @note 支持负数格式，如"@-123%" 转换为 -123
 */
int16_t Transfer_RxData(void)
{    
    int index = 0, sign = 1;
    
    // 检查符号位
    if (Serial_RxData[index] == '-') {
        sign = -1; 
        index++;
    } 
    
    // 转换数字部分
    int16_t num_data = 0;
    for (; Serial_RxData[index] != '\0'; index++) {
        num_data = num_data * 10 + (Serial_RxData[index] - '0');
    }
    
    return num_data * sign;
}

/* ==============================================================================================
                                        中断服务函数
   ============================================================================================== */

/**
 * @brief USART1中断服务函数
 * @note 使用状态机解析数据包，格式: "@数据%"
 *       状态0: 等待包头'@'
 *       状态1: 接收数据，遇到'%'结束
 */
void USART1_IRQHandler(void) 
{
    static uint8_t Rx_State = 0;    // 接收状态机状态
    static uint8_t index = 0;       // 数据缓冲区索引
    
    // 检查接收中断标志
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        uint8_t RxData = USART_ReceiveData(USART1);
        
        switch (Rx_State) {
            case 0: // 状态0: 等待包头'@'
                if (RxData == '@') {
                    Rx_State = 1;   // 进入数据接收状态
                    index = 0;      // 重置缓冲区索引
                }
                break;
                
            case 1: // 状态1: 接收数据
                if (RxData == '%') { 
                    // 收到包尾，完成接收
                    Serial_RxData[index] = '\0';    // 添加字符串结束符
                    Rx_State = 0;                   // 返回等待状态
                    Serial_RxFlag = 1;              // 设置接收完成标志
                } 
                else if (index < sizeof(Serial_RxData) - 1) { 
                    // 正常数据，存入缓冲区
                    Serial_RxData[index++] = RxData;
                } 
                else { 
                    // 数据过长，重置状态机
                    Rx_State = 0;
                }
                break;
        }
        
        // 清除中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// 使用示例:
// if (Serial_GetRxFlag() == 1) {
//     int16_t speed = Transfer_RxData();
//     Serial_Printf("Received speed: %d\r\n", speed);
// }
