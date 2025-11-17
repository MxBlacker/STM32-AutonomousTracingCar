#ifndef __Initialzation_
#define __Initialzation_

void AutoInitGPIO(GPIO_TypeDef* GPIOx , GPIOMode_TypeDef Mode , uint16_t Pin , GPIOSpeed_TypeDef Speed);
void AutoInitNVIC(uint32_t NVIC_PriorityGroup_x, uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority);


#endif
