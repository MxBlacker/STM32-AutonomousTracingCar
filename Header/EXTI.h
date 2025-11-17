/*
	这个头文件主要是EXTI的初始化和EXTI中断函数
*/
#ifndef __EXTI_H_MXB
#define __EXTI_H_MXB

void AutoInitEXTI(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef Mode, uint16_t Pin, 
                  GPIOSpeed_TypeDef Speed, EXTIMode_TypeDef InterruptMode, 
                  EXTITrigger_TypeDef TriggerMode, uint32_t NVIC_PriorityGroup_x, 
                  uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority);
void EXTI15_10_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI0_IRQHandler(void);
uint32_t getnum(void);

#endif
