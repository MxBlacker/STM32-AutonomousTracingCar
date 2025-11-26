#include "stm32f10x.h"
#include "Init.h"
#include "Delay.h"

/**
 * @brief 自动化配置外部中断函数
 * @note 一站式配置GPIO、外部中断线和NVIC，实现快速中断设置
 *       比GTNH的流水线强3千万倍(bushi)
 * 
 * @param GPIOx GPIO端口，如GPIOA, GPIOB, GPIOC
 * @param Mode GPIO模式，如GPIO_Mode_IPU(上拉输入)
 * @param Pin GPIO引脚，如GPIO_Pin_14
 * @param Speed GPIO速度，如GPIO_Speed_50MHz
 * @param InterruptMode 中断模式，EXTI_Mode_Interrupt(中断)或EXTI_Mode_Event(事件)
 * @param TriggerMode 触发模式，EXTI_Trigger_Falling(下降沿)等
 * @param NVIC_PriorityGroup_x NVIC优先级分组，如NVIC_PriorityGroup_2
 * @param IRQChannel 中断通道，如EXTI15_10_IRQn
 * @param PreemptionPriority 抢占优先级
 * @param SubPriority 子优先级
 * 
 * @example 
 * EXTI_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_14, GPIO_Speed_50MHz, 
 *              EXTI_Mode_Interrupt, EXTI_Trigger_Falling, 
 *              NVIC_PriorityGroup_2, EXTI15_10_IRQn, 1, 1);
 */
void EXTIx_Init(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef Mode, uint16_t Pin, 
                  GPIOSpeed_TypeDef Speed, EXTIMode_TypeDef InterruptMode, 
                  EXTITrigger_TypeDef TriggerMode, uint32_t NVIC_PriorityGroup_x, 
                  uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority)
{
    /* ======================================================================
                               Step 1: 时钟使能
       ====================================================================== */
    
    // 使能AFIO时钟，用于GPIO重映射和外部中断配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    // 调用自动化GPIO初始化函数配置引脚
    GPIOx_Init(GPIOx, Mode, Pin, Speed);
    
    /* ======================================================================
                       Step 2: GPIO端口源映射配置
       ====================================================================== */
    
    uint8_t GPIO_PortSourceGPIOx = 0x00;
    
    // 根据GPIO端口选择对应的端口源
    if (GPIOx == GPIOA) {
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOA;
    } else if (GPIOx == GPIOB) {
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOB;
    } else if (GPIOx == GPIOC) {
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOC;
    }
    // 可根据需要继续扩展GPIOD, GPIOE等
    
    /* ======================================================================
                       Step 3: GPIO引脚源映射配置
       ====================================================================== */
    
    uint8_t GPIO_PinSourcex = 0;
    
    // 将GPIO_Pin_x转换为GPIO_PinSourcex格式
    switch (Pin) {
        case GPIO_Pin_0:  GPIO_PinSourcex = GPIO_PinSource0;  break;
        case GPIO_Pin_1:  GPIO_PinSourcex = GPIO_PinSource1;  break;
        case GPIO_Pin_2:  GPIO_PinSourcex = GPIO_PinSource2;  break;
        case GPIO_Pin_3:  GPIO_PinSourcex = GPIO_PinSource3;  break;
        case GPIO_Pin_4:  GPIO_PinSourcex = GPIO_PinSource4;  break;
        case GPIO_Pin_5:  GPIO_PinSourcex = GPIO_PinSource5;  break;
        case GPIO_Pin_6:  GPIO_PinSourcex = GPIO_PinSource6;  break;
        case GPIO_Pin_7:  GPIO_PinSourcex = GPIO_PinSource7;  break;
        case GPIO_Pin_8:  GPIO_PinSourcex = GPIO_PinSource8;  break;
        case GPIO_Pin_9:  GPIO_PinSourcex = GPIO_PinSource9;  break;
        case GPIO_Pin_10: GPIO_PinSourcex = GPIO_PinSource10; break;
        case GPIO_Pin_11: GPIO_PinSourcex = GPIO_PinSource11; break;
        case GPIO_Pin_12: GPIO_PinSourcex = GPIO_PinSource12; break;
        case GPIO_Pin_13: GPIO_PinSourcex = GPIO_PinSource13; break;
        case GPIO_Pin_14: GPIO_PinSourcex = GPIO_PinSource14; break;
        case GPIO_Pin_15: GPIO_PinSourcex = GPIO_PinSource15; break;
        default: break;
    }
    
    /* ======================================================================
                       Step 4: 外部中断线配置
       ====================================================================== */
    
    // 配置GPIO引脚与外部中断线的映射关系
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);
    
    /* ======================================================================
                       Step 5: 外部中断参数配置
       ====================================================================== */
    
    EXTI_InitTypeDef EXTI_InitStructure;
    
    // 设置外部中断线（注意：这里需要将uint16_t转换为uint32_t）
    EXTI_InitStructure.EXTI_Line = (uint32_t)Pin;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;        // 使能中断线
    EXTI_InitStructure.EXTI_Mode = InterruptMode;    // 中断模式
    EXTI_InitStructure.EXTI_Trigger = TriggerMode;   // 触发模式
    
    // 初始化外部中断
    EXTI_Init(&EXTI_InitStructure);
    
    /* ======================================================================
                       Step 6: NVIC中断控制器配置
       ====================================================================== */
    
    // 配置NVIC优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_x);

    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;                 // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  // 使能中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;     // 子优先级
    
    // 初始化NVIC
    NVIC_Init(&NVIC_InitStructure);
    
    // 至此，外部中断配置完成，可以在对应的中断服务函数中处理中断事件
}
