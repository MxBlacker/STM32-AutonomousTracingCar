#include "stm32f10x.h"                  // Device header
#include <stdio.h>

/* ==============================================================================================
                                        GPIO自动初始化函数
   ============================================================================================== */

/**
 * @brief GPIO端口自动初始化函数
 * @param GPIOx GPIO端口，支持GPIOA、GPIOB、GPIOC
 * @param Mode GPIO工作模式，如GPIO_Mode_Out_PP(推挽输出)、GPIO_Mode_IPU(上拉输入)等
 * @param Pin GPIO引脚，如GPIO_Pin_0、GPIO_Pin_1等，支持位或操作(|)配置多个引脚
 * @param Speed GPIO输出速度，如GPIO_Speed_50MHz、GPIO_Speed_2MHz等
 * 
 * @note 自动使能对应端口的时钟，简化GPIO初始化流程
 *       目前支持GPIOA、GPIOB、GPIOC三个端口
 * 
 * @example 
 * // 初始化PA1为上拉输入，50MHz速度
 * AutoInitGPIO(GPIOA, GPIO_Mode_IPU, GPIO_Pin_1, GPIO_Speed_50MHz);
 * 
 * // 初始化PB5、PB6为推挽输出，50MHz速度  
 * AutoInitGPIO(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_5 | GPIO_Pin_6, GPIO_Speed_50MHz);
 */
void AutoInitGPIO(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef Mode, uint16_t Pin, GPIOSpeed_TypeDef Speed)
{
    // 参数有效性检查
    if (GPIOx == NULL) return;
    
    /* ======================================================================
                               Step 1: 时钟使能
       ====================================================================== */
    
    // 根据GPIO端口使能对应的时钟
    if (GPIOx == GPIOA) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    } else if (GPIOx == GPIOB) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    } else if (GPIOx == GPIOC) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    // 可根据需要继续扩展GPIOD、GPIOE等端口
    
    /* ======================================================================
                               Step 2: GPIO参数配置
       ====================================================================== */
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置GPIO工作模式(输入/输出/复用/模拟等)
    GPIO_InitStructure.GPIO_Mode = Mode;
    // 配置GPIO引脚(支持单个或多个引脚)
    GPIO_InitStructure.GPIO_Pin = Pin;
    // 配置GPIO输出速度(仅输出模式有效)
    GPIO_InitStructure.GPIO_Speed = Speed;
    
    // 初始化GPIO
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/* ==============================================================================================
                                        NVIC自动初始化函数
   ============================================================================================== */

/**
 * @brief NVIC中断控制器自动初始化函数
 * @param NVIC_PriorityGroup_x 优先级分组，如NVIC_PriorityGroup_2
 * @param IRQChannel 中断通道，如USART1_IRQn、EXTI0_IRQn等
 * @param PreemptionPriority 抢占优先级(0-3，取决于分组)
 * @param SubPriority 子优先级(0-3，取决于分组)
 * 
 * @note 优先级分组说明:
 *       - NVIC_PriorityGroup_0: 0位抢占优先级, 4位子优先级
 *       - NVIC_PriorityGroup_1: 1位抢占优先级, 3位子优先级  
 *       - NVIC_PriorityGroup_2: 2位抢占优先级, 2位子优先级
 *       - NVIC_PriorityGroup_3: 3位抢占优先级, 1位子优先级
 *       - NVIC_PriorityGroup_4: 4位抢占优先级, 0位子优先级
 * 
 * @example
 * // 配置USART1中断，优先级分组2，抢占优先级1，子优先级0
 * AutoInitNVIC(NVIC_PriorityGroup_2, USART1_IRQn, 1, 0);
 * 
 * // 配置EXTI0中断，优先级分组2，抢占优先级0，子优先级1  
 * AutoInitNVIC(NVIC_PriorityGroup_2, EXTI0_IRQn, 0, 1);
 */
void AutoInitNVIC(uint32_t NVIC_PriorityGroup_x, uint8_t IRQChannel, 
                  uint8_t PreemptionPriority, uint8_t SubPriority)
{
    /* ======================================================================
                               Step 1: 优先级分组配置
       ====================================================================== */
    
    // 配置NVIC优先级分组(影响整个系统的中断优先级结构)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_x);

    /* ======================================================================
                               Step 2: 中断通道配置
       ====================================================================== */
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 设置中断通道(具体外设的中断号)
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
    // 使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // 设置抢占优先级(数值越小优先级越高)
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    // 设置子优先级(同一抢占优先级内，数值越小优先级越高)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    
    // 初始化NVIC中断
    NVIC_Init(&NVIC_InitStructure);
}

// ==============================================================================================
//                                        常用配置示例
// ==============================================================================================

/*
// GPIO配置示例:
// 1. LED输出配置
AutoInitGPIO(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_0, GPIO_Speed_50MHz);

// 2. 按键输入配置  
AutoInitGPIO(GPIOA, GPIO_Mode_IPU, GPIO_Pin_0, GPIO_Speed_50MHz);

// 3. USART引脚配置
AutoInitGPIO(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_9, GPIO_Speed_50MHz);  // TX
AutoInitGPIO(GPIOA, GPIO_Mode_IPU, GPIO_Pin_10, GPIO_Speed_50MHz);   // RX

// NVIC配置示例:
// 1. 串口中断配置
AutoInitNVIC(NVIC_PriorityGroup_2, USART1_IRQn, 1, 0);

// 2. 外部中断配置
AutoInitNVIC(NVIC_PriorityGroup_2, EXTI0_IRQn, 0, 1);

// 3. 定时器中断配置
AutoInitNVIC(NVIC_PriorityGroup_2, TIM2_IRQn, 2, 0);
*/
