#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "Init.h"
#include "Button.h"
#include "Menu.h"
#include "OLED.h"

int temp;

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
		temp_boot_switch();
		OLED_ShowNum(4,3,temp++,2);
		break;
	case MENU_TEMP_SPEED:
		temp_speed_switch();
		break;
	default:
		break;
	}
}

// int PrevState[3][16], CurState[3][16];


// /**
//  *	@brief	按钮的全自动识别
//  *	@note 	GPIO口我们默认上拉输入，所以低电平为按下，我们以低电平变高电平的瞬间为
//  *	@param	GPIOx : 不多解释
//  *	@param	Pin : 不多解释
//  *	@param	EVENT_ID : 到时候再说
//  */
// void Button_Check(GPIO_TypeDef * GPIOx , uint16_t Pin , uint16_t EVENT_ID){
	
// 	//计时器和状态记录
// 	static int Button_Counter = 0;
// 	int i_index = (GPIOx == GPIOA)?(0):(GPIOx == GPIOB)?1:2;
// 	int j_index = (Pin == GPIO_Pin_0)?0:(Pin == GPIO_Pin_1)?1:(Pin == GPIO_Pin_2)?2:(Pin == GPIO_Pin_3)?3:(Pin == GPIO_Pin_4)?4:(Pin == GPIO_Pin_5)?5:(Pin == GPIO_Pin_6)?6:(Pin == GPIO_Pin_7)?7:(Pin == GPIO_Pin_8)?8:(Pin == GPIO_Pin_9)?9:(Pin == GPIO_Pin_10)?10:(Pin == GPIO_Pin_11)?11:(Pin == GPIO_Pin_12)?12:(Pin == GPIO_Pin_13)?13:(Pin == GPIO_Pin_14)?14:(Pin == GPIO_Pin_15)?15:0;
	
// 	//计时器++
// 	Button_Counter++;
	
// 	//指定GPIO口的初始化
// 	GPIOx_Init(GPIOx , GPIO_Mode_IPU , Pin , GPIO_Speed_50MHz);
// 	OLED_ShowNum(4,15,Button_Counter,1);
// 	//10ms消抖
// 	if(Button_Counter >= 10){
// 		//状态读取和转移
// 		PrevState[i_index][j_index] = CurState[i_index][j_index];
// 		CurState[i_index][j_index] = GPIO_ReadInputDataBit(GPIOx , Pin);
		
// 		//状态判断：按钮松开
// 		if(PrevState[i_index][j_index] == RESET && CurState[i_index][j_index] == SET){
// 			EVENT(EVENT_ID);
// 		}
		
// 		//计时器归零
// 		Button_Counter = 0;
// 	}
// }
