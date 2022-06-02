#include "power.h"

/*********************************硬件驱动层****************************/
extern HalAdapter_Management_Led_t halAdapter_Led;//API接口供外部使用

static u8 shutdown_flg=0;

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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SI4438_POWER_GPIO_PIN|RS485_POWER_GPIO_PIN | POWER_STATU_GPIO_PIN; //485和12V电源初始化  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = POWER_ON_OFF_GPIO_PIN; //485和12V电源初始化  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Power_OUT_ON(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = POWER_ON_OFF_GPIO_PIN; //485和12V电源初始化  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,  POWER_ON_OFF_GPIO_PIN);
}


u8 Power_Shut_Down(u8 shutime)
{
	if(!POWER_ONOFF_STATU_PIN_STATE)//电源开关按下
	{
		shutdown_flg++;
		halAdapter_Led.HAL_LedBlueTurn();

		if(shutdown_flg>=shutime)
		{	
			halAdapter_Led.HAL_LedShutDown();
			POWER_PIN_STATE=0;
			while(1);
		}
		return 1;
	}
	else
		shutdown_flg=0;
}


