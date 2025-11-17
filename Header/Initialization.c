#include "stm32f10x.h"                  // Device header
#include <stdio.h>

/*
	AutoInitGPIO
	用于自动初始化GPIOx端口
	输入格式为:
	AutoInitGPIO(GPIOx , Mode , Pin , Speed)
	
	这里我只写了ABC三个端口，因为我STM32就这么多端口赫赫
*/

void AutoInitGPIO(GPIO_TypeDef* GPIOx , GPIOMode_TypeDef Mode , uint16_t Pin , GPIOSpeed_TypeDef Speed){
	if(GPIOx == NULL) return;
	
	if(GPIOx == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(GPIOx == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else if(GPIOx == GPIOC)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = Mode; 
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Speed = Speed;
	GPIO_Init(GPIOx , &GPIO_InitStructure);
}

/*
	AutoInitNVIC
	用于自动初始化GPIOx端口
	输入格式为:
	AutoInitGPIO(GPIOx , Mode , Pin , Speed)
	
	这里我只写了ABC三个端口，因为我STM32就这么多端口赫赫
*/

void AutoInitNVIC(uint32_t NVIC_PriorityGroup_x, uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority){
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_x);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_Init(&NVIC_InitStructure);

}
