#include "stm32f10x.h"                  // Device header
#include "Timer.h"
#include "Motor.h"
#include "Serial.h"

/* ==============================================================================================
                                        电机参数定义
   ============================================================================================== */

#define PULSES_PER_REV          2496    // 编码器每转脉冲数(11线编码器 * 4倍频 * 51减速比)
#define MAX_RPS                 9.9f    // 电机最大转速(转/秒)，实测9.22RPS
#define MAX_PWM_VALUE           1000    // PWM最大值(对应100%占空比)
#define SPEED_CALC_INTERVAL_MS  10      // 速度计算间隔(毫秒)

// 方向控制引脚定义(已在头文件中定义，此处为使用说明)
// 左电机: PB12(DIR1), PB13(DIR2) - 右电机: PB14(DIR1), PB15(DIR2)

/* ==============================================================================================
                                        电机基础函数
   ============================================================================================== */

/**
 * @brief 获取电机编码器频率(计数值)
 * @param Motor 电机结构体指针
 * @return 编码器当前计数值
 */
uint16_t Motor_Get_Frequency(MotorTypeDef *Motor)
{
    return TIM_GetCounter(Motor->TIMx);
}

/**
 * @brief 设置电机目标速度
 * @param Motor 电机结构体指针
 * @param Target 目标速度值
 */
void Motor_Set_Target_Speed(MotorTypeDef *Motor, float Target)
{
    Motor->Target_Speed = (int)Target;
}

/* ==============================================================================================
                                        速度计算函数
   ============================================================================================== */

/**
 * @brief 计算电机当前速度
 * @param Motor 电机结构体指针
 * @note 基于编码器脉冲差值计算实际转速，处理16位计数器溢出
 */
void Cal_Current_Speed(MotorTypeDef *Motor)
{
    // 保存上一次计数值并读取当前值
    Motor->Prev_Count = Motor->Cur_Count;
    Motor->Cur_Count = TIM_GetCounter(Motor->TIMx);
    
    // 计算脉冲差值，处理16位计数器溢出情况
    int32_t pulse_diff = (int32_t)Motor->Cur_Count - (int32_t)Motor->Prev_Count;
    
    // 溢出处理：当差值超过16位有符号数范围时进行调整
    if (pulse_diff > 32767) {
        pulse_diff -= 65536;    // 正向溢出修正
    } else if (pulse_diff < -32768) {
        pulse_diff += 65536;    // 负向溢出修正
    }
        
    // 转换为实际转速(RPS -> 转/秒)
    // 计算逻辑: (脉冲差值 / 每转脉冲数) * (1000ms / 计算间隔10ms) / 最大转速归一化 * 1000
    Motor->Cur_Speed = (((float)pulse_diff * 100 / PULSES_PER_REV) / MAX_RPS) * 1000;
        
    // 调试输出(需要时取消注释)
    // Serial_Printf("Pulses: %d, Speed: %.2f RPS\r\n", pulse_diff, Motor->Cur_Speed);
}

/* ==============================================================================================
                                        PID控制函数
   ============================================================================================== */

/**
 * @brief PID控制算法
 * @param Motor 电机结构体指针
 * @param PID PID控制器结构体指针
 * @return PID计算出的控制输出值
 */
float PID_Control(MotorTypeDef *Motor, PIDTypeDef *PID)
{
    // 计算当前误差(目标速度 - 实际速度)
    PID->error = Motor->Target_Speed - Motor->Cur_Speed;
    
    /* ==============================
              比例项(P)计算
       ============================== */
    // 比例项：与当前误差成正比，主要控制响应速度
    float proportional_score = PID->KP * PID->error;
    
    /* ==============================
              积分项(I)计算  
       ============================== */
    // 积分项：累计历史误差，消除稳态误差
    PID->I += PID->error;
    
    // 积分限幅：防止积分饱和
    if (PID->I > PID->I_Lim) {
        PID->I = PID->I_Lim;
    } else if (PID->I < -PID->I_Lim) {
        PID->I = -PID->I_Lim;
    }
    float integral_score = PID->KI * PID->I;
    
    /* ==============================
              微分项(D)计算
       ============================== */
    // 微分项：基于误差变化率，提供阻尼作用，抑制超调
    float derivative_score = PID->KD * (PID->error - PID->prev_error);
    
    /* ==============================
              PID输出合成
       ============================== */
    float output = proportional_score + integral_score + derivative_score;
    
    // 输出限幅：确保输出在合理范围内
    if (output > PID->Output_Lim) {
        output = PID->Output_Lim;
    } else if (output < -PID->Output_Lim) {
        output = -PID->Output_Lim;
    }
    
    // 保存当前误差供下次微分计算使用
    PID->prev_error = PID->error;
    
    return output;
}

/* ==============================================================================================
                                        电机速度设置函数
   ============================================================================================== */

/**
 * @brief 设置电机速度(统一函数)
 * @param isLeftMotor 电机选择: 1-左电机, 0-右电机
 * @param Speed 速度值(-1000 ~ +1000，正负表示方向)
 * 
 * @note 速度控制逻辑:
 *       - 正速度: DIR1=1, DIR2=0 (正转)
 *       - 负速度: DIR1=0, DIR2=1 (反转) 
 *       - 速度绝对值限制在0-1000范围内
 */
void Set_Motor_Speed(uint8_t isLeftMotor, float Speed)
{
    GPIO_TypeDef* gpio_port = GPIOB;  // 方向控制引脚都在GPIOB
    uint16_t dir1_pin, dir2_pin;
    uint8_t pwm_channel;
    
    // 根据电机选择配置对应的引脚和PWM通道
    if (isLeftMotor) {
        // 左电机配置
        dir1_pin = LEFT_MOTOR_DIR1_PIN;
        dir2_pin = LEFT_MOTOR_DIR2_PIN;
        pwm_channel = LEFT_MOTOR_PWM_CHANNEL;
    } else {
        // 右电机配置
        dir1_pin = RIGHT_MOTOR_DIR1_PIN;
        dir2_pin = RIGHT_MOTOR_DIR2_PIN;
        pwm_channel = RIGHT_MOTOR_PWM_CHANNEL;
    }
    
    /* ==============================
              方向控制逻辑
       ============================== */
    if (Speed < 0) {
        // 负速度：反转方向
        GPIO_WriteBit(gpio_port, dir1_pin, Bit_RESET);  // DIR1 = 0
        GPIO_WriteBit(gpio_port, dir2_pin, Bit_SET);    // DIR2 = 1
        Speed = -Speed;  // 取绝对值用于PWM计算
    } else {
        // 正速度：正转方向
        GPIO_WriteBit(gpio_port, dir1_pin, Bit_SET);    // DIR1 = 1
        GPIO_WriteBit(gpio_port, dir2_pin, Bit_RESET);  // DIR2 = 0
    }
    
    /* ==============================
              PWM值限制
       ============================== */
    // 确保速度值在有效范围内(0 ~ 1000)
    if (Speed > MAX_PWM_VALUE) {
        Speed = MAX_PWM_VALUE;
    }
    
    // 调试输出(需要时取消注释)
    // Serial_Printf("Motor: %s, Speed: %.2f, Channel: %d\r\n", 
    //              isLeftMotor ? "Left" : "Right", Speed, pwm_channel);
    
    /* ==============================
              PWM输出设置
       ============================== */
    Set_OC_value(TIM2, pwm_channel, (int)Speed);
}

extern MotorTypeDef LEFT_MOTOR;
extern MotorTypeDef RIGHT_MOTOR;
void Set_Car_Speed(float Speed_1, float Speed_2){
    Motor_Set_Target_Speed(&LEFT_MOTOR, Speed_1);
    Motor_Set_Target_Speed(&RIGHT_MOTOR, Speed_2);
}

// ==============================================================================================
//                                        使用示例
// ==============================================================================================

/*
// 电机初始化示例：
MotorTypeDef left_motor = {TIM3, 0, 0, 0.0f, 0};
MotorTypeDef right_motor = {TIM4, 0, 0, 0.0f, 0};

// PID参数初始化示例：
PIDTypeDef pid = {1.0f, 0.1f, 0.05f, 100.0f, 0.0f, 1000.0f, 0.0f, 0.0f};

// 设置电机速度示例：
Set_Motor_Speed(1, 500);   // 左电机正转，速度500
Set_Motor_Speed(0, -300);  // 右电机反转，速度300
Set_Motor_Speed(1, 0);     // 左电机停止
*/
