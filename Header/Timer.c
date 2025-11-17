#include "stm32f10x.h"                  // Device header
#include "Initialization.h"
#include "Button.h"
#include "Motor.h"
#include "Serial.h"
#include "OLED.h"

/*
	TIMx_Init(TIMx , Period , Prescaler , TIM_MODE , channel)
	用于自动初始化TIMx端口
*/

enum TIM_MODE{
	IC_MODE,
	OC_MODE,
	ENCODER_MODE,
	INTERRUPT_MODE
};

void TIMx_Init(TIM_TypeDef * TIMx , uint16_t Period , uint16_t Prescaler , uint8_t mode , uint8_t channel){
	
	if(TIMx == TIM1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if (TIMx == TIM2) 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (TIMx == TIM3) 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (TIMx == TIM4) 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//时基和时钟无论如何都要开启的
	TIM_InternalClockConfig(TIMx);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = Period - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = Prescaler - 1;	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIMx , &TIM_TimeBaseInitStructure);
	
	switch(mode){
		case IC_MODE:
			
			//不用就暂时不写了，累死我了
			break;
		
		case OC_MODE:
			{
			//OC_MODE和IC_MODE理应来讲时可以覆盖在INTERRUPT_MODE上的
			//TImx_Init(TIM2 , 1000 , 72 , OC_MODE , 3);
			//TImx_Init(TIM2 , 1000 , 72 , OC_MODE , 4);
			
			AutoInitGPIO(GPIOA , GPIO_Mode_AF_PP , GPIO_Pin_2 , GPIO_Speed_50MHz);
			AutoInitGPIO(GPIOA , GPIO_Mode_AF_PP , GPIO_Pin_3 , GPIO_Speed_50MHz);
				
			TIM_OCInitTypeDef TIM_OCInitStructure;
			TIM_OCStructInit(&TIM_OCInitStructure); //因为有很多通用计时器用不到的，所以这里都给个默认值，不然值不确定
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //输出比较模式
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = 0; //设置CCR (16位) 的值
		
            switch(channel) {
				case 1: TIM_OC1Init(TIMx, &TIM_OCInitStructure); break;
                case 2: TIM_OC2Init(TIMx, &TIM_OCInitStructure); break;
                case 3: TIM_OC3Init(TIMx, &TIM_OCInitStructure); break;
                case 4: TIM_OC4Init(TIMx, &TIM_OCInitStructure); break;
			}
			break;
			
			}
		case ENCODER_MODE:
			{
			//TIMx_Init(TIM3 , 65535 , 1 , ENCODER_MODE) 
			//TIMx_Init(TIM4 , 65535 , 1 , ENCODER_MODE) 	
		
			if(TIMx == TIM2){
				AutoInitGPIO(GPIOA , GPIO_Mode_IPU , GPIO_Pin_0 , GPIO_Speed_50MHz);
				AutoInitGPIO(GPIOA , GPIO_Mode_IPU , GPIO_Pin_1 , GPIO_Speed_50MHz);
			}else if(TIMx == TIM3){
				AutoInitGPIO(GPIOA , GPIO_Mode_IPU , GPIO_Pin_6 , GPIO_Speed_50MHz);
				AutoInitGPIO(GPIOA , GPIO_Mode_IPU , GPIO_Pin_7 , GPIO_Speed_50MHz);
			}else if(TIMx == TIM4){
				AutoInitGPIO(GPIOB , GPIO_Mode_IPU , GPIO_Pin_6 , GPIO_Speed_50MHz);
				AutoInitGPIO(GPIOB , GPIO_Mode_IPU , GPIO_Pin_7 , GPIO_Speed_50MHz);
			}
	
			TIM_ICInitTypeDef TIM_ICInitStructure;
			TIM_ICStructInit(&TIM_ICInitStructure); //编码器模式不需要全部配置，后面那些分频器啥的就不用配置了
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //CH1
			TIM_ICInitStructure.TIM_ICFilter = 0xF;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
			TIM_ICInit(TIMx , &TIM_ICInitStructure);

			TIM_ICStructInit(&TIM_ICInitStructure); //配置Channel和2，两个都要去到编码器
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; //CH2
			TIM_ICInitStructure.TIM_ICFilter = 0xF;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
			TIM_ICInit(TIMx , &TIM_ICInitStructure);

			TIM_EncoderInterfaceConfig(TIMx , TIM_EncoderMode_TI12 , TIM_ICPolarity_Rising , TIM_ICPolarity_Rising);
			
			IRQn_Type IRQn;
			if (TIMx == TIM3) 
				IRQn = TIM3_IRQn;
			else if (TIMx == TIM4) 
				IRQn = TIM4_IRQn;
			
			NVIC_InitTypeDef NVIC_InitStructure;
			NVIC_InitStructure.NVIC_IRQChannel = IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			
			break;
			}
		case INTERRUPT_MODE:
			{
			//TIMx_Init(TIM2 , 100 , 720 , INTERRUPT_MODE)
		
			TIM_ClearITPendingBit(TIMx , TIM_IT_Update);
			TIM_ITConfig(TIMx , TIM_IT_Update,ENABLE); //开启中断输出控制

			IRQn_Type IRQn;
			if (TIMx == TIM1) 
				IRQn = TIM1_CC_IRQn;
			else if (TIMx == TIM2) 
				IRQn = TIM2_IRQn;
			else if (TIMx == TIM3) 
				IRQn = TIM3_IRQn;
			else if (TIMx == TIM4) 
				IRQn = TIM4_IRQn;
			
			AutoInitNVIC(NVIC_PriorityGroup_2 , IRQn , 2 , 2);
			break;
			}
	}
	
	TIM_Cmd(TIMx , ENABLE);
}

/*
	Set_OC_value(TIMx , channel , CCR_value);
	其实是设置电机速度的
*/

void Set_OC_value(TIM_TypeDef * TIMx ,uint8_t channel , int CCR_value){
	
	switch(channel){
		case 1: TIM_SetCompare1(TIMx , CCR_value); break;
		case 2: TIM_SetCompare2(TIMx , CCR_value); break;
		case 3: TIM_SetCompare3(TIMx , CCR_value); break;
		case 4: TIM_SetCompare4(TIMx , CCR_value); break; 
	}
	
}

/*
	以下为中断函数
*/

uint8_t Freq_Counter = 0;
extern int TASK_MODE;

MotorTypeDef Left_Motor,Right_Motor;
PIDTypeDef Left_PID,Right_PID;

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		Freq_Counter++;
		Button_Check(GPIOA , GPIO_Pin_0 , 0);
		
		if(TASK_MODE == 0){																				//TASK1	
			
			Left_Motor.Counter++;
			Set_Right_Motor_Speed(0);
			
			if(Left_Motor.Counter >= 10){
				
				Cal_Current_Speed(&Left_Motor , TIM3);													//看看左电机多块
				
				float Delta_Speed = PID_Control(&Left_Motor , &Left_PID);								//如果和目标速度一样就维持，不一样就想想要往哪边动，动多少
				Set_Left_Motor_Speed(Left_Motor.Target_Speed + Delta_Speed);							//和Target_Speed的偏差主要是为了补偿阻力！
				
				//Serial_Printf("Current: %d Delta: %.1f",(int)Left_Motor.Cur_Speed, Delta_Speed);
				
				Left_Motor.Counter = 0;	
			}
			
		}else{																							//TASK2
			
			Right_Motor.Counter++;
			Set_Left_Motor_Speed(0);
			
			if(Right_Motor.Counter >= 10){
				
				Cal_Current_Speed(&Left_Motor , TIM3);
				Cal_Current_Speed(&Right_Motor , TIM4);
			
				Motor_Set_Target_Speed(&Right_Motor , Left_Motor.Cur_Speed);							//同上
				float Delta_Speed = PID_Control(&Right_Motor , &Right_PID);
				Set_Right_Motor_Speed(Right_Motor.Target_Speed + Delta_Speed);
			
				Serial_Printf("LC: %d RC: %d\r\n",(int)Left_Motor.Cur_Speed, (int)Right_Motor.Cur_Speed);
				
				Right_Motor.Counter = 0;	
			}
		}
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
