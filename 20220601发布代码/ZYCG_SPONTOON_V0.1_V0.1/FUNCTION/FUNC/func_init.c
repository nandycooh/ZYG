#include "func_init.h"
#include "delay.h"

//初始化
void Touch_Gpio_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//触摸电源管脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//触摸状态及备用IO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Vibrate_Gpio_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//触摸电源管脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


//初始化
void PCI_Gpio_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//保险 电击 伸缩状态IO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void delay_ms(u16 ms){
	halAdapter_Clock.HAL_DelayMS(ms);
}

void delay_us(u32 us){
	halAdapter_Clock.HAL_DelayUS(us);
}

void get_timestamp(unsigned long* tick){
	halAdapter_Clock.HAL_Get_TickMS(tick);
}
/*适配器句柄*/
FUNC_adapter_t func_adapter_implements =
{
.isInit = false,
.delay_ms = delay_ms,
.delay_us = delay_us,
.get_timestamp = get_timestamp,
};

static FUNC_adapter_t* Func_adapterInit(void)
{
	func_adapter_implements.MPU =  GetFuncAdapter_MPU();

	return &func_adapter_implements;
}
/**
  * @brief 获取硬件整体实例
  * @param
  * @retval: None
*/
FUNC_adapter_t* GetFuncAdapter(void)
{
	return Func_adapterInit();
}


