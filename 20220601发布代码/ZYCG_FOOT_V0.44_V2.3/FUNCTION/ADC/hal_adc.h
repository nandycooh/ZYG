#ifndef __HAL_ADC_H
#define __HAL_ADC_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stdbool.h"

#define SAMPE_TIMES 3

void Adc_init(void);
uint16_t ADC_GetValueF(ADC_TypeDef * chn);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
float calculate_bat(void);
void Adc_ShutDown(void);

//ADC管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  ADC初始化（默认开启ADC）
		* @param  None
		* @retval : None
	**/
	void (*HAL_Adc_Init)(void);
	/**
		* @brief  返回ADC值
		* @param  通道选择（0~3）
		* @retval : ADC值
	**/
	float (*HAL_Adc_CalculateValue)(void);
	/**
		* @brief  ADC关闭
		* @param  None
		* @retval : None
	**/
	void (*HAL_Adc_ShutDown)(void);
	/**
		* @brief  ADC开启
		* @param  None
		* @retval : None
	**/
	void (*HAL_Adc_Start)(void);
}HalAdapter_Management_Adc_t;

HalAdapter_Management_Adc_t* GetHalAdapter_Adc(void);


#endif
