#ifndef __HAL_MPU_H
#define __HAL_MPU_H

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdbool.h"

//管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  初始化
		* @param  None
		* @retval : None
	**/
	void (*HAL_MPU_Init)(void);
	/**
		* @brief  开启中断
		* @param  None
		* @retval : None
	**/
	void (*HAL_MPU_EnableInterrupt)(void);
	/**
		* @brief  关闭中断
		* @param  None）
		* @retval : None
	**/
	void (*HAL_MPU_DisableInterrupt)(void);
	
	/**
		* @brief  是否发生中断
		* @param  None
		* @retval : false：没有发生中断, true：发生中断
	**/
	bool (*HAL_MPU_GetInterrupStatus)(void);
	
}HalAdapter_Management_MPU_t;

HalAdapter_Management_MPU_t* GetHalAdapter_MPU(void);

#endif
