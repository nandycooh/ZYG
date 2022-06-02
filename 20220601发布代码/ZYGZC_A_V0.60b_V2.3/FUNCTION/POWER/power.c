#include "power.h"

/*********************************硬件驱动层****************************/

/************************************************************  	
函数名称：	Power_Init
函数功能：	电源相关初始化
入口参数：	void
返回参数：	无
说明：	    无
***********************************************************/ 
void Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SI4438_POWER_GPIO_PIN;   //4438电源初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//232电源和复位控制初始化
	GPIO_InitStructure.GPIO_Pin = RS232_POWER_STATU_GPIO_PIN |	RS232_RESET_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

