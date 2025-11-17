#include "stm32f10x.h"
#include "Initialization.h"
#include "Delay.h"

/*
	请见证！有史以来最自动化的超级自动化流水线！比GTNH的流水线强3千万倍(bu
	
	// 配置PB14为外部中断，下降沿触发
	AutoInitEXTI(GPIOB, GPIO_Mode_IPU, GPIO_Pin_14, GPIO_Speed_50MHz, 
             EXTI_Mode_Interrupt, EXTI_Trigger_Falling, NVIC_PriorityGroup_2,
             EXTI15_10_IRQn, 1, 1);
	
	mc懒人包作者之麦香包在此
	赫赫赫赫
*/
void AutoInitEXTI(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef Mode, uint16_t Pin, 
                  GPIOSpeed_TypeDef Speed, EXTIMode_TypeDef InterruptMode, 
                  EXTITrigger_TypeDef TriggerMode, uint32_t NVIC_PriorityGroup_x, 
                  uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    AutoInitGPIO(GPIOx, Mode, Pin, Speed);
    
    uint8_t GPIO_PortSourceGPIOx = 0x00;
    if (GPIOx == GPIOA)
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOA;
    else if (GPIOx == GPIOB)
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOB;
    else if (GPIOx == GPIOC)
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIOC;
    
    uint8_t GPIO_PinSourcex = 0;
    switch (Pin) {
        case GPIO_Pin_0:  GPIO_PinSourcex = GPIO_PinSource0; break;
        case GPIO_Pin_1:  GPIO_PinSourcex = GPIO_PinSource1; break;
        case GPIO_Pin_2:  GPIO_PinSourcex = GPIO_PinSource2; break;
        case GPIO_Pin_3:  GPIO_PinSourcex = GPIO_PinSource3; break;
        case GPIO_Pin_4:  GPIO_PinSourcex = GPIO_PinSource4; break;
        case GPIO_Pin_5:  GPIO_PinSourcex = GPIO_PinSource5; break;
        case GPIO_Pin_6:  GPIO_PinSourcex = GPIO_PinSource6; break;
        case GPIO_Pin_7:  GPIO_PinSourcex = GPIO_PinSource7; break;
        case GPIO_Pin_8:  GPIO_PinSourcex = GPIO_PinSource8; break;
        case GPIO_Pin_9:  GPIO_PinSourcex = GPIO_PinSource9; break;
        case GPIO_Pin_10: GPIO_PinSourcex = GPIO_PinSource10; break;
        case GPIO_Pin_11: GPIO_PinSourcex = GPIO_PinSource11; break;
        case GPIO_Pin_12: GPIO_PinSourcex = GPIO_PinSource12; break;
        case GPIO_Pin_13: GPIO_PinSourcex = GPIO_PinSource13; break;
        case GPIO_Pin_14: GPIO_PinSourcex = GPIO_PinSource14; break;
        case GPIO_Pin_15: GPIO_PinSourcex = GPIO_PinSource15; break;
        default: break;
    }
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);
    
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = (uint32_t)Pin;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = InterruptMode;
    EXTI_InitStructure.EXTI_Trigger = TriggerMode;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_x);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_Init(&NVIC_InitStructure);
}
