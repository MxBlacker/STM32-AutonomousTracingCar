#include "stm32f10x.h"                  // Device header
#include "Timer.h"
#include "Menu.h"
#include "OLED.h"
#include "Init.h"

int main(void){
	
    //OLED初始化
    OLED_Init();
    OLED_Clear();
    //菜单初始化
    interface_init();
    show_interface();

    //以防万一，开一下时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    //电机方向控制初始化
    GPIOx_Init(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_12, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_13, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_14, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB, GPIO_Mode_Out_PP, GPIO_Pin_15, GPIO_Speed_50MHz);

    //TIM2_OC初始化，驱动左右轮电机
    TIMx_Init(TIM2, 1000, 720, OC_MODE, 1); // 到时候就是0~1000的速度范围
    TIMx_Init(TIM2, 1000, 720, OC_MODE, 2);
    GPIOx_Init(GPIOA, GPIO_Mode_Out_PP, GPIO_Pin_1, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA, GPIO_Mode_Out_PP, GPIO_Pin_3, GPIO_Speed_50MHz);

    //TIM3,TIM4 Encoder初始化，读取左右轮电机PID
    TIMx_Init(TIM3, 65535, 1, ENCODER_MODE, 1);
    TIMx_Init(TIM4, 65535, 1, ENCODER_MODE, 1);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB,  GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB,  GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz);

    //按钮口初始化
    GPIOx_Init(GPIOB,  GPIO_Mode_IPU, GPIO_Pin_1, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOB,  GPIO_Mode_IPU, GPIO_Pin_11, GPIO_Speed_50MHz);

    //红外对射口初始化
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_8, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_9, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_10, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_11, GPIO_Speed_50MHz);
    GPIOx_Init(GPIOA,  GPIO_Mode_IPU, GPIO_Pin_12, GPIO_Speed_50MHz);

    while(1){

    }

}
