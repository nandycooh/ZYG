#ifndef __POWERMANAGE_H
#define __POWERMANAGE_H

#include "stm32f10x.h"
#include "stdio.h"
#include "sys.h"
#include "delay.h"

#define BAT_CHARGE_PIN 	GPIO_Pin_11    //在插上USB后判断，充电中：低   充满：高
#define USB_CHECK_PIN 	GPIO_Pin_12		 //USB电源管脚
#define BAT_CHRG_IN     PBin(12)
#define USB_CHECK_IN    PBin(10)
#define KEY_02			PBin(3)
#define	POWER_ON_OFF	PBout(4)

#define READBAT  GPIO_ReadInputDataBit(GPIOB, BAT_CHARGE_PIN)//充电时的状态
#define READUSB  GPIO_ReadInputDataBit(GPIOB, USB_CHECK_PIN)//是否插上USB


/***************************POWER******************************/
#define BAT_CAPACITY_LV1   4.000000      //对应显示电量80%
#define BAT_CAPACITY_LV2   3.850000     //对应显示电量60%
#define BAT_CAPACITY_LV3   3.770000     //对应显示电量40%
#define BAT_CAPACITY_LV4   3.720000     //对应显示电量20%
#define BAT_CAPACITY_LV5   3.600000      //对应显示电量空
#define BAT_CAPACITY_LV6   3.300000      //对应强制关机电量
//#define BAT_CAPACITY_LV1   1.35     //对应显示电量80%
//#define BAT_CAPACITY_LV2   1.30     //对应显示电量60%
//#define BAT_CAPACITY_LV3   1.22    //对应显示电量40%
//#define BAT_CAPACITY_LV4   1.15     //对应显示电量20%
//#define BAT_CAPACITY_LV5   1.10     //对应显示电量空
//#define BAT_CAPACITY_LV6   1.06     //对应强制关机电量


u8 Power_Shut_Down(u8 shutime);

//IIC管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  IIC初始化
		* @param  None
		* @retval : None
	**/
	void (*HAL_Power_Init)(void);
	/**
		* @brief  开启充电口插拔中断
		* @param  None
		* @retval : None
	**/
	void (*HAL_PowerCharge_EnableInterrupt)(void);
	/**
		* @brief  开启充电口插拔中断
		* @param  None）
		* @retval : None
	**/
	void (*HAL_PowerCharge_DisableInterrupt)(void);
	/**
		* @brief  获取充电口状态
		* @param  None
		* @retval : 0：充电口未插入 1：充电口插入
	**/
	uint8_t (*HAL_Get_ChargePinStatus)(void);
	/**
		* @brief  获取是否充满满电(条件是处于充电状态下)
		* @param  None
		* @retval :插上充电口时（0：未充满电 1：充满电） 未插上充电口（只返回1）
	**/
	uint8_t (*HAL_Get_FullBatteryPin_InCharge)(void);
	
	uint8_t (*HAL_Get_battery_power)(void);
	
}HalAdapter_Management_Power_t;


#endif
