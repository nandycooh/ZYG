#include "timer2.h"
#include "./nrf24L01/exti.h"
#include "./usart/USART2.h"

void TIM2_IntInit(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period=arr; 
	TIM_TimeBaseStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 

	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	TIM_Cmd(TIM2,ENABLE); 		
}

uint8_t time;

extern uint8_t IrqDynRxFlag;
extern uint8_t IrqDynTxFlag;

extern float roll_result, yaw_result, pitch_result;
extern float q_result[4];
extern int8_t accuracy_result;

u32 time_flag = 0;
void TIM2_IRQHandler(void)   
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET) {
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
		time_flag ++;
	}
}
