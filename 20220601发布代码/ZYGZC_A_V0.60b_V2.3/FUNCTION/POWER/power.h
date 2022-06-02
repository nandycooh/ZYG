#ifndef __POWER_H
#define __POWER_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include "delay.h"
#include "hal_flash.h"
#include "led.h"

/*********************************宏定义功能区**************************/
#define RS232_RESET_GPIO_PORT			GPIOA								
#define RS232_RESET_GPIO_CLK				RCC_APB2Periph_GPIOA
#define RS232_RESET_GPIO_PIN				GPIO_Pin_12

#define RS232_POWER_STATU_GPIO_PORT			GPIOA								
#define RS232_POWER_STATU_GPIO_CLK				RCC_APB2Periph_GPIOA
#define RS232_POWER_STATU_GPIO_PIN				GPIO_Pin_11

#define SI4438_POWER_GPIO_PORT	GPIOB								
#define SI4438_POWER_GPIO_CLK		RCC_APB2Periph_GPIOB
#define SI4438_POWER_GPIO_PIN		GPIO_Pin_0

#define RS232_RESET_STATU   		PAout(12)//复位
#define RS232_POWER_STATE   		PAout(11)//电源控制 低电平通
#define SI4438_POWER_PIN_STATE  PBout(0)//433电源控制

/*********************************函数申明区****************************/
void Power_Init(void);

#endif
