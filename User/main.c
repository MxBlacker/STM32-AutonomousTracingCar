#include "stm32f10x.h"                  // Device header
#include "Initialization.h"
#include "Serial.h"
#include "Timer.h"
#include "Motor.h"
#include "OLED.h"
#include "Delay.h"

extern uint8_t Serial_RxData[];
extern uint8_t Freq_Counter;
extern MotorTypeDef Left_Motor,Right_Motor;
extern PIDTypeDef Left_PID,Right_PID;

int TASK_MODE = 0;
uint16_t Motor_Left_Speed;
uint16_t Motor_Right_Speed;

int main(void){
	
	//控制电机方向的口
	AutoInitGPIO(GPIOB,GPIO_Mode_Out_PP,GPIO_Pin_12,GPIO_Speed_50MHz);
	AutoInitGPIO(GPIOB,GPIO_Mode_Out_PP,GPIO_Pin_13,GPIO_Speed_50MHz);
	AutoInitGPIO(GPIOB,GPIO_Mode_Out_PP,GPIO_Pin_14,GPIO_Speed_50MHz);
	AutoInitGPIO(GPIOB,GPIO_Mode_Out_PP,GPIO_Pin_15,GPIO_Speed_50MHz);
	
	//USART初始化
	USART1_Serial_Init();
	
	//TIM2~TIM4初始化，其中TIM2用于中断，1,2口用来干啥我忘了，3，4口输出方波
	//TIM3，TIM4Encoder接速度
	TIMx_Init(TIM2 , 1000 , 72 , OC_MODE , 3);				//输出左轮
	TIMx_Init(TIM2 , 1000 , 72 , OC_MODE , 4);				//右轮
	TIMx_Init(TIM2 , 1000 , 72 , INTERRUPT_MODE , 0);		//启动中断
	TIMx_Init(TIM3 , 65535 , 1 , ENCODER_MODE , 0);			//读左轮
	TIMx_Init(TIM4 , 65535 , 1 , ENCODER_MODE , 0);			//读右轮
	
	//OLED调试用的
	OLED_Init();
	OLED_Clear();
	
	//Left参数
	Left_PID.KP = 2;
	Left_PID.KI = 1;
	Left_PID.KD = 1;
	Left_PID.I = 0;
	Left_PID.I_Lim = 1000;
	Left_PID.Output_Lim = 1000;
	
	//Right参数
	Right_PID.KP = 2;
	Right_PID.KI = 3;
	Right_PID.KD = 1;
	Right_PID.I = 0;
	Right_PID.I_Lim = 1000;
	Right_PID.Output_Lim = 1000;
	
	//一些基本的初始化
	Left_Motor.Prev_Count = 0; 
	Left_Motor.Cur_Count = 0; 
	Left_Motor.Target_Speed = 0;
	Right_Motor.Prev_Count = 0; 
	Right_Motor.Cur_Count = 0; 
	
	while(1){
		
		Delay_ms(10);														//更稳定一点，太快了会有点问题
	
		if(TASK_MODE == 0){													//TASK1
			OLED_ShowNum(1,15,1,1);
			if(Freq_Counter >= 10){											//上传	
				Freq_Counter = 0;		
				Motor_Left_Speed = (int16_t)Motor_Get_Left_Frequency();
				Motor_Right_Speed = (int16_t)Motor_Get_Right_Frequency();
							
				OLED_ShowNum(1,1,(int)Left_Motor.Target_Speed,5);
				OLED_ShowNum(2,1,(int)Motor_Get_Left_Frequency(),5);
				Serial_Printf("Cur_Speed : %.1f\n",Left_Motor.Cur_Speed);	//FireWater协议是这样的= =
			}
		
			if(Serial_GetRxFlag() == 1){									//接收到数据了
				Left_PID.I = 0;
				Motor_Set_Target_Speed(&Left_Motor,Transfer_RxData());
				//Serial_Printf("True = %d",Left_Motor.Target_Speed);
			}
		}
		else if(TASK_MODE == 1){											//TASK2
			OLED_ShowNum(1,15,2,1);
		}
	}
}
