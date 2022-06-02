#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  
 
 
/***************************CLOCK*****************************/
//是否打开时钟滴答器Clock ticker
#define CLOCKTICKER														0

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

void TimeStamp_Increment(void);
void get_tick_count(unsigned long *count);


//时钟管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  时钟初始化函数
		* @param  u32 延时时间单位ms
		* @retval : None
	**/
	void (*HAL_ClockInit)(void);
	/**
		* @brief  延时函数
		* @param  u32 延时时间单位ms
		* @retval : None
	**/
	void (*HAL_DelayMS)(u16);
	
	/**
		* @brief  延时函数
		* @param  u32 延时时间单位us
		* @retval : None
	**/
	void (*HAL_DelayUS)(u32);
	/**
		* @brief  延时函数
		* @param  u32 延时时间单位us
		* @retval : None
	**/
	void (*HAL_Get_TickMS)(unsigned long*);
}HalAdapter_Management_Clock_t;

extern HalAdapter_Management_Clock_t halAdapter_Clock;
#endif



