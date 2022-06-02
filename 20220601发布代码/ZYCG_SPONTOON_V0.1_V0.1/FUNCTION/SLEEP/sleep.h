#ifndef __SLEEP_H
#define __SLEEP_H

#include "sys.h"
#include "delay.h"

//工作模式管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  进入Stop模式函数
		* @param  None
		* @retval : None
	**/
	void (*HAL_EnterStop)(void);
	/**
		* @brief  进入LowPowerStop模式函数
		* @param  None
		* @retval : None
	**/
	void (*HAL_EnterLowPowerStop)(void);
	/**
		* @brief  进入Standby模式函数
		* @param  None
		* @retval : None
	**/
	void (*HAL_EnterStandby)(void);
	/**
		* @brief  恢复默认设置
		* @param  None
		* @retval : None
	**/
	void (*HAL_RestoreDefaultSettings)(void);
	/**
		* @brief  重启函数
		* @param  None
		* @retval : None
	**/
	void (*HAL_Restart)(void);
}HalAdapter_Management_OperatingMode_t;

HalAdapter_Management_OperatingMode_t* GetHalAdapter_OptMode(void);

void Un_Used_Gpio_Init(void);
#endif
