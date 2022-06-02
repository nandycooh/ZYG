#include "func_init.h"
#include "delay.h"

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


