#ifndef MOTORBIKE_H
#define MOTORBIKE_H

#include "stm32f10x.h"                  // Device header
#include <stdint.h>        

typedef struct {
	int Counter;
    int Prev_Count; 
    int Cur_Count;
    float Cur_Speed;
    int Target_Speed;
} MotorTypeDef;

typedef struct {
    float KP, KI, KD, I_Lim, I, Output_Lim;
    float error, prev_error;
} PIDTypeDef;

uint16_t Motor_Get_Left_Frequency(void);
uint16_t Motor_Get_Right_Frequency(void);
void Motor_Set_Target_Speed(MotorTypeDef *Motor, float Target);
void Cal_Current_Speed(MotorTypeDef *Motor, TIM_TypeDef *TIMx);
float PID_Control(MotorTypeDef *Motor, PIDTypeDef *PID);

void Set_Left_Motor_Speed(float Speed);
void Set_Right_Motor_Speed(float Speed);

#endif
