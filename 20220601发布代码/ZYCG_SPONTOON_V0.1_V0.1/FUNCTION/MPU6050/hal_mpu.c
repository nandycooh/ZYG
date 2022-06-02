#include "hal_mpu.h"

bool is_Interrupt = false;

//初始化
static void MPU_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

bool Get_MPU_Interrupt(void)
{
	bool res = is_Interrupt;
	is_Interrupt = false;
	return res;
}

void Disable_MPU_Interrupt(void){
	EXTI->IMR &= ~(EXTI_Line8); //屏蔽按键中断
}

void Enable_MPU_Interrupt(void){
	EXTI->IMR |= EXTI_Line8;
}

void EXTI9_5_IRQHandler(void)
{   
	if(EXTI_GetITStatus(EXTI_Line8) != RESET){
		EXTI_ClearITPendingBit(EXTI_Line8);
		is_Interrupt = true;
	}    
}

HalAdapter_Management_MPU_t halAdapter_MPU = 
{
.isInit = false,
.HAL_MPU_Init = MPU_Init,
.HAL_MPU_EnableInterrupt = Enable_MPU_Interrupt,
.HAL_MPU_DisableInterrupt = Disable_MPU_Interrupt,
.HAL_MPU_GetInterrupStatus = Get_MPU_Interrupt,
};
/**
  * @brief 硬件MPU实例且初始化
  * @param
  * @retval: None
*/
static HalAdapter_Management_MPU_t* HAL_adapterInit_MPU(void)
{
	if(halAdapter_MPU.isInit == false){
		//实例化
		halAdapter_MPU.HAL_MPU_Init();
		halAdapter_MPU.isInit = true;
	}
	return &halAdapter_MPU;
}
/**
  * @brief 获取硬件Key实例
  * @param
  * @retval: None
*/
HalAdapter_Management_MPU_t* GetHalAdapter_MPU(void)
{
	return HAL_adapterInit_MPU();
}


