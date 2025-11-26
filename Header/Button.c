#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "Init.h"
#include "Button.h"
#include "Menu.h"

void EVENT(uint16_t EVENT_ID){
	switch (EVENT_ID){
	case MENU_UP:
		up();
		break;
	case MENU_DOWN:
		down();
		break;
	case MENU_FORWARD:
		confirm();
		break;
	case MENU_BACKWARD:
		backward();
		break;
	case MENU_TEMP_BOOT:
		
	default:
		break;
	}
}

/**
 *	@brief	按钮的全自动识别
 *	@note 	GPIO口我们默认上拉输入，所以低电平为按下，我们以低电平变高电平的瞬间为
 *	@param	GPIOx : 不多解释
 *	@param	Pin : 不多解释
 *	@param	EVENT_ID : 到时候再说
 */

void Button_Check(GPIO_TypeDef * GPIOx , uint16_t Pin , uint16_t EVENT_ID){
	
	//计时器和状态记录
	static int Button_Counter = 0;
	static int PrevState = SET , CurState = SET;
	
	//计时器++
	Button_Counter++;
	
	//指定GPIO口的初始化
	GPIOx_Init(GPIOA , GPIO_Mode_IPU , GPIO_Pin_0 , GPIO_Speed_50MHz);
	
	//10ms消抖
	if(Button_Counter >= 10){
		//状态读取和转移
		PrevState = CurState;
		CurState = GPIO_ReadInputDataBit(GPIOx , Pin);
		
		//状态判断：按钮松开
		if(PrevState == RESET && CurState == SET){
			EVENT(EVENT_ID);
		}
		
		//计时器归零
		Button_Counter = 0;
	}
}
