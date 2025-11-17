
/**
 *	@brief TIM的初始化和TIM的功能设置
 */

#ifndef __TIMER_FUCK
#define __TIMER_FUCK

enum TIM_MODE{
	IC_MODE,
	OC_MODE,
	ENCODER_MODE,
	INTERRUPT_MODE
};

void TIMx_Init(TIM_TypeDef * TIMx , uint16_t Period , uint16_t Prescaler , uint8_t mode , uint8_t channel);
void Set_OC_value(TIM_TypeDef * TIMx ,uint8_t channel , int CCR_value);
	
#endif
