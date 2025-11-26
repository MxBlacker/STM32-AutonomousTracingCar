/**
 *	@brief EXTI的初始化
 *	@note 现在这个比较废物
*/
#ifndef __EXTI_H_MXB
#define __EXTI_H_MXB

void EXTIx_Init(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef Mode, uint16_t Pin, 
                  GPIOSpeed_TypeDef Speed, EXTIMode_TypeDef InterruptMode, 
                  EXTITrigger_TypeDef TriggerMode, uint32_t NVIC_PriorityGroup_x, 
                  uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority);

#endif
