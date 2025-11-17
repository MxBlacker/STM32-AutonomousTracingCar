#ifndef MOTORBIKE_H
#define MOTORBIKE_H

#include "stm32f10x.h"                  // Device header
#include <stdint.h>        

/** 
 * @brief 电机控制结构体
 * @note 用于管理单个电机的状态和参数
 */
typedef struct {
    TIM_TypeDef *TIMx;          // 绑定的定时器(用于编码器计数)
    int Prev_Count;             // 上一次编码器计数值
    int Cur_Count;              // 当前编码器计数值
    float Cur_Speed;            // 当前计算出的速度(RPS)
    int Target_Speed;           // 目标速度
} MotorTypeDef;

/** 
 * @brief PID控制器结构体
 * @note 存储PID参数和状态变量
 */
typedef struct {
    float KP;                   // 比例系数
    float KI;                   // 积分系数  
    float KD;                   // 微分系数
    float I_Lim;                // 积分限幅
    float I;                    // 积分项累计值
    float Output_Lim;           // 输出限幅
    float error;                // 当前误差
    float prev_error;           // 上一次误差
} PIDTypeDef;

// 电机方向控制引脚定义
#define LEFT_MOTOR_DIR1_PIN     GPIO_Pin_12    // 左电机方向引脚1
#define LEFT_MOTOR_DIR2_PIN     GPIO_Pin_13    // 左电机方向引脚2
#define RIGHT_MOTOR_DIR1_PIN    GPIO_Pin_14    // 右电机方向引脚1  
#define RIGHT_MOTOR_DIR2_PIN    GPIO_Pin_15    // 右电机方向引脚2

// 电机PWM输出通道定义
#define LEFT_MOTOR_PWM_CHANNEL  3              // 左电机PWM通道(TIM2_CH3)
#define RIGHT_MOTOR_PWM_CHANNEL 4              // 右电机PWM通道(TIM2_CH4)

/** 
 * @brief 获取电机编码器计数值
 * @param Motor 电机结构体指针
 * @return 编码器当前计数值
 */
uint16_t Motor_Get_Frequency(MotorTypeDef *Motor);

/** 
 * @brief 设置电机目标速度
 * @param Motor 电机结构体指针
 * @param Target 目标速度值
 */
void Motor_Set_Target_Speed(MotorTypeDef *Motor, float Target);

/**
 * @brief 计算电机当前速度
 * @param Motor 电机结构体指针
 * @note 基于编码器脉冲差值计算实际转速
 */
void Cal_Current_Speed(MotorTypeDef *Motor);

/**
 * @brief PID控制算法
 * @param Motor 电机结构体指针
 * @param PID PID控制器结构体指针
 * @return PID计算出的控制输出值
 */
float PID_Control(MotorTypeDef *Motor, PIDTypeDef *PID);

/**
 * @brief 设置电机速度(统一函数)
 * @param isLeftMotor 电机选择: 1-左电机, 0-右电机
 * @param Speed 速度值(正负表示方向)
 */
void Set_Motor_Speed(uint8_t isLeftMotor, float Speed);

#endif
