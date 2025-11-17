#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#include "Initialization.h"

uint8_t Serial_RxData[6]; //虽然我给了六位的空间，但是由于速度在-1000到1000间，所以实际上需要用到的应该就5位
char Serial_TxData[10]; //自定义信息idk，感觉p用没有，但是

void USART1_Serial_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	AutoInitGPIO(GPIOA, GPIO_Mode_AF_PP, GPIO_Pin_9, GPIO_Speed_50MHz); //Transmit
	AutoInitGPIO(GPIOA, GPIO_Mode_IPU, GPIO_Pin_10, GPIO_Speed_50MHz); //Receive

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600; //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //模式
	USART_InitStructure.USART_Parity = USART_Parity_No; //校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //数据位
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);
	
	AutoInitNVIC(NVIC_PriorityGroup_2,USART1_IRQn,1,0);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte){ //发送字符
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t * Array, uint16_t Length){ //发送数组
	for(int index = 0 ; index < Length ; index++)
		Serial_SendByte(Array[index]);
}

void Serial_SendString(char * String){ //发送字符串
	for(int index = 0 ; String[index] != '\0' ; index++)
		Serial_SendByte(String[index]);
}

void Serial_Printf(char * format, ...){ //格式化文本封装
	
	char Temp_String[100];
	
	va_list arg;
	va_start(arg , format);
	vsprintf(Temp_String , format , arg);
	va_end(arg);
	
	Serial_SendString(Temp_String);
	
}

void Serial_SendPack(void){
	
	Serial_SendByte('@');
	Serial_SendString(Serial_TxData);
	Serial_SendByte('%');

}

/*
	Serial_GetRxFlag();
	如果调用函数时读到Flag为1，那就直接去拿RxData里的值就行了
*/

int Serial_RxFlag = 0;
uint8_t Serial_GetRxFlag(void){
	if(Serial_RxFlag == 1){
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

/*
	Transfer_RxData(void);
	经典的字符串转数字函数
*/

int16_t Transfer_RxData(void){	
	
	int index = 0 , sign = 1;
	if(Serial_RxData[index] == '-'){
		sign = -1; index++;
	} 
	
	int16_t num_data = 0;
	for(;Serial_RxData[index] != '\0';index++){
		//Serial_Printf("%c ",Serial_RxData[index]);
		num_data = num_data * 10 + (Serial_RxData[index] - '0');
	}
	
	return num_data * sign;
}

/*
	状态机，由于长度不确定，所以就俩状态
*/

void USART1_IRQHandler(void) {
    static uint8_t Rx_State = 0;
    static uint8_t index = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        uint8_t RxData = USART_ReceiveData(USART1);
        
        switch(Rx_State) {
			case 0: // 等待包头
				if(RxData == '@') {
					Rx_State = 1;
					index = 0;
				}
				break;
                
            case 1: // 接收数据
                if(RxData == '%') { // 收到包尾
                    Serial_RxData[index] = '\0'; // 添加字符串结束符
                    Rx_State = 0;
                    Serial_RxFlag = 1;
                } 
                else if(index < sizeof(Serial_RxData) - 1) { // 防止数组越界
                    Serial_RxData[index++] = RxData;
                } 
                else { // 数据过长，重置
                    Rx_State = 0;
                }
                break;
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

//Serial_GetRxFlag == 1时读取数据




