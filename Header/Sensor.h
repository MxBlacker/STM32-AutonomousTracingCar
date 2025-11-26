#ifndef __INFERIOR_RED_RAY_SENSOR
#define __INFERIOR_RED_RAY_SENSOR

#define OUT_1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)
#define OUT_2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9)
#define OUT_3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10)
#define OUT_4 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
#define OUT_5 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)

/**
 *	@brief 红外对射模块的头文件
 */
void TRACK(void);

#endif
