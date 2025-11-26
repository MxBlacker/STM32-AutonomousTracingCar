#include "stm32f10x.h"                  // Device header
#include "Init.h"
#include "Button.h"
#include "Motor.h"
#include "Serial.h"
#include "OLED.h"
#include "Sensor.h"

/* ==============================================================================================
                                       枚举类型定义
   ============================================================================================== */

/**
 * @brief 定时器工作模式枚举
 */
enum TIM_MODE {
    IC_MODE,            // 输入捕获模式
    OC_MODE,            // 输出比较模式
    ENCODER_MODE,       // 编码器模式
    INTERRUPT_MODE      // 定时中断模式
};

/* ==============================================================================================
                                       定时器初始化函数
   ============================================================================================== */

/**
 * @brief 定时器多功能初始化函数
 * @param TIMx 定时器指针，如TIM1、TIM2、TIM3、TIM4
 * @param Period 自动重装载值ARR
 * @param Prescaler 预分频器值PSC
 * @param mode 工作模式：IC_MODE, OC_MODE, ENCODER_MODE, INTERRUPT_MODE
 * @param channel 通道号(1-4)，某些模式下不需要此参数
 * 
 * @note  注意一下，如果开启了ENCODER_MODE，那么它的时基就被占用了，但是IC和OC是不会占用的，所以可以同时进行定时中断
*		  这里的IC和OC我直接包含了定时中断的设置，避免出现上次一样的情况
 *
 * @example 
 * TIMx_Init(TIM2, 1000, 72, OC_MODE, 3);     // PWM输出，通道3
 * TIMx_Init(TIM3, 65535, 1, ENCODER_MODE, 0); // 编码器模式
 * TIMx_Init(TIM2, 100, 720, INTERRUPT_MODE, 0); // 纯定时中断
 */
void TIMx_Init(TIM_TypeDef *TIMx, uint16_t Period, uint16_t Prescaler, uint8_t mode, uint8_t channel)
{
    /* ======================================================================
                               Step 1: 时钟使能
       ====================================================================== */
    
    // 根据定时器类型使能对应的时钟
    if (TIMx == TIM1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    } else if (TIMx == TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    } else if (TIMx == TIM3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    } else if (TIMx == TIM4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    }
    
    /* ======================================================================
                               Step 2: 时基初始化
       ====================================================================== */
    
    // 配置定时器使用内部时钟
    TIM_InternalClockConfig(TIMx);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = Period - 1;               // 自动重装载值ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = Prescaler - 1;         // 预分频器PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // 重复计数(高级定时器)
    
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);
    
    /* ======================================================================
                               Step 3: 模式配置
       ====================================================================== */
    
    switch (mode) {
        case IC_MODE:
            /* ==============================
                    输入捕获模式配置
               ============================== */
            {
                // 配置GPIO为输入模式
                if (TIMx == TIM2) {
                    switch (channel) {
                        case 1: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_0, GPIO_Speed_50MHz); break;
                        case 2: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_1, GPIO_Speed_50MHz); break;
                        case 3: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_2, GPIO_Speed_50MHz); break;
                        case 4: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_3, GPIO_Speed_50MHz); break;
                    }
                } else if (TIMx == TIM3) {
                    switch (channel) {
                        case 1: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz); break;
                        case 2: GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz); break;
                        case 3: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_0, GPIO_Speed_50MHz); break;
                        case 4: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_1, GPIO_Speed_50MHz); break;
                    }
                } else if (TIMx == TIM4) {
                    switch (channel) {
                        case 1: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz); break;
                        case 2: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz); break;
                        case 3: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_8, GPIO_Speed_50MHz); break;
                        case 4: GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_9, GPIO_Speed_50MHz); break;
                    }
                }
                
                // 输入捕获参数配置
                TIM_ICInitTypeDef TIM_ICInitStructure;
                TIM_ICStructInit(&TIM_ICInitStructure);  // 初始化为默认值
                
                TIM_ICInitStructure.TIM_Channel = (channel == 1) ? TIM_Channel_1 : 
                                                 (channel == 2) ? TIM_Channel_2 : 
                                                 (channel == 3) ? TIM_Channel_3 : TIM_Channel_4;
                TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    // 输入捕获预分频
                TIM_ICInitStructure.TIM_ICFilter = 0x0F;                 // 输入滤波器(0-0xF)
                TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 捕获极性：上升沿
                TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直接模式
                
                TIM_ICInit(TIMx, &TIM_ICInitStructure);
                
                // 使能捕获中断
                TIM_ITConfig(TIMx, (channel == 1) ? TIM_IT_CC1 : 
                                   (channel == 2) ? TIM_IT_CC2 : 
                                   (channel == 3) ? TIM_IT_CC3 : TIM_IT_CC4, ENABLE);
                
                // 配置NVIC中断
                IRQn_Type IRQn;
                if (TIMx == TIM1) IRQn = TIM1_CC_IRQn;
                else if (TIMx == TIM2) IRQn = TIM2_IRQn;
                else if (TIMx == TIM3) IRQn = TIM3_IRQn;
                else if (TIMx == TIM4) IRQn = TIM4_IRQn;
                
                AutoInitNVIC(NVIC_PriorityGroup_2, IRQn, 1, 1);
            }
            break;
            
        case OC_MODE:
            /* ==============================
                    输出比较模式配置
               ============================== */
            {
                // 配置GPIO为复用推挽输出
                if (TIMx == TIM2) {
                    switch (channel) {
                        case 1: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_0, GPIO_Speed_50MHz); break;
                        case 2: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_1, GPIO_Speed_50MHz); break;
                        case 3: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_2, GPIO_Speed_50MHz); break;
                        case 4: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_3, GPIO_Speed_50MHz); break;
                    }
                } else if (TIMx == TIM3) {
                    switch (channel) {
                        case 1: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_6, GPIO_Speed_50MHz); break;
                        case 2: GPIOx_Init(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_7, GPIO_Speed_50MHz); break;
                        case 3: GPIOx_Init(GPIOB, GPIO_Mode_AF_PP, GPIO_Pin_0, GPIO_Speed_50MHz); break;
                        case 4: GPIOx_Init(GPIOB, GPIO_Mode_AF_PP, GPIO_Pin_1, GPIO_Speed_50MHz); break;
                    }
                }
                
                // 输出比较参数配置
                TIM_OCInitTypeDef TIM_OCInitStructure;
                TIM_OCStructInit(&TIM_OCInitStructure);  // 初始化为默认值
                
                TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        // PWM模式1
                TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性高
                TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
                TIM_OCInitStructure.TIM_Pulse = 0;                       // 初始CCR值
                
                // 根据通道初始化对应的输出比较单元
                switch (channel) {
                    case 1: TIM_OC1Init(TIMx, &TIM_OCInitStructure); break;
                    case 2: TIM_OC2Init(TIMx, &TIM_OCInitStructure); break;
                    case 3: TIM_OC3Init(TIMx, &TIM_OCInitStructure); break;
                    case 4: TIM_OC4Init(TIMx, &TIM_OCInitStructure); break;
                }
            }
            break;
            
        case ENCODER_MODE:
            /* ==============================
                    编码器模式配置
               ============================== */
            {
                // 配置编码器GPIO引脚
                if (TIMx == TIM2) {
                    GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_0, GPIO_Speed_50MHz);  // TIM2_CH1
                    GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_1, GPIO_Speed_50MHz);  // TIM2_CH2
                } else if (TIMx == TIM3) {
                    GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz);  // TIM3_CH1
                    GPIOx_Init(GPIOA, GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz);  // TIM3_CH2
                } else if (TIMx == TIM4) {
                    GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_6, GPIO_Speed_50MHz);  // TIM4_CH1
                    GPIOx_Init(GPIOB, GPIO_Mode_IPU, GPIO_Pin_7, GPIO_Speed_50MHz);  // TIM4_CH2
                }
                
                // 配置通道1为输入捕获
                TIM_ICInitTypeDef TIM_ICInitStructure;
                TIM_ICStructInit(&TIM_ICInitStructure);
                
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;     // 通道1
                TIM_ICInitStructure.TIM_ICFilter = 0xF;              // 滤波器
                TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 极性
                TIM_ICInit(TIMx, &TIM_ICInitStructure);
                
                // 配置通道2为输入捕获
                TIM_ICStructInit(&TIM_ICInitStructure);
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;     // 通道2
                TIM_ICInitStructure.TIM_ICFilter = 0xF;              // 滤波器
                TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 极性
                TIM_ICInit(TIMx, &TIM_ICInitStructure);
                
                // 配置编码器接口模式
                TIM_EncoderInterfaceConfig(TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
                
                // 配置编码器中断
                IRQn_Type IRQn;
                if (TIMx == TIM3) {
                    IRQn = TIM3_IRQn;
                } else if (TIMx == TIM4) {
                    IRQn = TIM4_IRQn;
                }
                
                NVIC_InitTypeDef NVIC_InitStructure;
                NVIC_InitStructure.NVIC_IRQChannel = IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
            }
            break;
            
        case INTERRUPT_MODE:
            /* ==============================
                    定时中断模式配置
               ============================== */
            {
                // 清除更新中断标志
                TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
                // 使能更新中断
                TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
                
                // 配置NVIC中断
                IRQn_Type IRQn;
                if (TIMx == TIM1) {
                    IRQn = TIM1_UP_IRQn;
                } else if (TIMx == TIM2) {
                    IRQn = TIM2_IRQn;
                } else if (TIMx == TIM3) {
                    IRQn = TIM3_IRQn;
                } else if (TIMx == TIM4) {
                    IRQn = TIM4_IRQn;
                }
                
                AutoInitNVIC(NVIC_PriorityGroup_2, IRQn, 2, 2);
            }
            break;
    }
    
    /* ======================================================================
                               Step 4: 使能定时器
       ====================================================================== */
    
    TIM_Cmd(TIMx, ENABLE);
    
    // 如果是高级定时器(TIM1)，还需要使能主输出
    if (TIMx == TIM1) {
        TIM_CtrlPWMOutputs(TIM1, ENABLE);
    }
}

/* ==============================================================================================
                                       输出比较值设置函数
   ============================================================================================== */

/**
 * @brief 设置输出比较值(CCR)
 * @param TIMx 定时器指针
 * @param channel 通道号(1-4)
 * @param CCR_value 比较值，决定PWM占空比
 * 
 * @note 主要用于设置电机速度、LED亮度等PWM控制
 */

void Set_OC_value(TIM_TypeDef *TIMx, uint8_t channel, int CCR_value)
{
    switch (channel) {
        case 1: TIM_SetCompare1(TIMx, CCR_value); break;
        case 2: TIM_SetCompare2(TIMx, CCR_value); break;
        case 3: TIM_SetCompare3(TIMx, CCR_value); break;
        case 4: TIM_SetCompare4(TIMx, CCR_value); break;
    }
}

/* ==============================================================================================
                                       中断服务函数
   ============================================================================================== */

MotorTypeDef LEFT_MOTOR = {
    TIM2,
    0,
    0,
    0,
    0,
}
,RIGHT_MOTOR = {
    TIM3,
    0,
    0,
    0,
    0,
};

PIDTypeDef LEFT_PID = {
    1,                // 比例系数
    1,                   // 积分系数  
    1,                   // 微分系数
    5,                // 积分限幅
    0,                    // 积分项累计值
    1000,          // 输出限幅
    0,             // 当前误差
    0,           // 上一次误差
}, RIGHT_PID = {
    1,                // 比例系数
    1,                   // 积分系数  
    1,                   // 微分系数
    5,                // 积分限幅
    0,                    // 积分项累计值
    1000,          // 输出限幅
    0,             // 当前误差
    0,   
};

int Freq_Counter = 0;
/**
 * @brief TIM2中断服务函数
 * @note 需要根据实际应用添加具体的中断处理逻辑
 */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        Button_Check(GPIOB, GPIO_Pin_1, MENU_TEMP_BOOT);  
        Button_Check(GPIOB, GPIO_Pin_11, MENU_TEMP_SPEED);
        
        Freq_Counter++;
        if(Freq_Counter >= 10){
            Cal_Current_Speed(&LEFT_MOTOR);
            Cal_Current_Speed(&RIGHT_MOTOR);
            float Output_left = PID_Control(&LEFT_MOTOR, &LEFT_PID);
            float Output_right = PID_Control(&RIGHT_MOTOR, &RIGHT_PID);
            Set_Motor_Speed(1, LEFT_MOTOR.Target_Speed + Output_left);
            Set_Motor_Speed(0, RIGHT_MOTOR.Target_Speed + Output_right);

            TRACK();
            Freq_Counter = 0;
        }
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
