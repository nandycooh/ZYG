#ifndef __FUNC_CONFIG_H
#define __FUNC_CONFIG_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

/***************************TIMER******************************/
/*虚拟定时器的个数*/
#define TIME2_EVENT_NUM 							7
#define TIME4_EVENT_NUM 							9

/*定时器ID管理，不能超过对应定时器个数*/
#define RF_RECEIVE										1
#define UPDATE_BATTERY								2 //充电芯片状态更新定时器
#define LED_FLICKER										3 //LED闪烁定时器

/*定时器时间长短管理*/
#define UPDATE_BATTERY_TIME						200 //200ms更新一次电池电量

/***************************POWER******************************/
//#define BAT_CAPACITY_LV1   4.0      //对应显示电量80%
//#define BAT_CAPACITY_LV2   3.85     //对应显示电量60%
//#define BAT_CAPACITY_LV3   3.77     //对应显示电量40%
//#define BAT_CAPACITY_LV4   3.72     //对应显示电量20%
//#define BAT_CAPACITY_LV5   3.6      //对应显示电量空
//#define BAT_CAPACITY_LV6   3.3      //对应强制关机电量

#define BATQUEUE_LENGTH						10//电池电压队列长度，用于计算平均电压值

/***************************MODULE*****************************/
//部件ID掩码
#define POWER_MASK								0x00
#define RF_MASK 									0x01
#define LED_MASK									0x02
#define MPU_MASK									0x03
#define BATADC_MASK								0x04

/***************************LED********************************/
#define IS_LED_LOW_ENABLE         1//led是否是低电平开， 使能低电平开

#define PB13 							BIT_ADDR(GPIOB_ODR_Addr, 13)
#define PB14 							BIT_ADDR(GPIOB_ODR_Addr, 14)
#define PB15 							BIT_ADDR(GPIOB_ODR_Addr, 15)

#endif
