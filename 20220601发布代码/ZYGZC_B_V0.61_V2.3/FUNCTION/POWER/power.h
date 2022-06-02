#ifndef __POWER_H
#define __POWER_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include "delay.h"
#include "led.h"

/*********************************宏定义功能区**************************/
#define POWER_ON_OFF_GPIO_PORT			GPIOC								
#define POWER_ON_OFF_GPIO_CLK				RCC_APB2Periph_GPIOC
#define POWER_ON_OFF_GPIO_PIN				GPIO_Pin_13

#define POWER_STATU_GPIO_PORT			GPIOA								
#define POWER_STATU_GPIO_CLK				RCC_APB2Periph_GPIOA
#define POWER_STATU_GPIO_PIN				GPIO_Pin_0

#define RS485_POWER_GPIO_PORT		GPIOA								
#define RS485_POWER_GPIO_CLK		RCC_APB2Periph_GPIOA
#define RS485_POWER_GPIO_PIN		GPIO_Pin_5

#define SI4438_POWER_GPIO_PORT	GPIOA								
#define SI4438_POWER_GPIO_CLK		RCC_APB2Periph_GPIOA
#define SI4438_POWER_GPIO_PIN		GPIO_Pin_11



#define POWER_ONOFF_STATU_PIN_STATE   		PCin(13)//电源状态
#define POWER_PIN_STATE   			PAout(0)//电源开关控制
#define SI485_POWER_PIN_STATE  	PAout(5)
#define SI4438_POWER_PIN_STATE  PAout(11)


/*********************************函数申明区****************************/
void Power_Init(void);
void Power_OUT_ON(void);
u8 Power_Shut_Down(u8 shutime);
/*********************************API接口层****************************/
//typedef struct
//{
//	void (*Driver_LedInit)(void);
//	void (*Driver_LedShutDown)(void);
//	void (*App_LedTurnState)(u8 colour);
//	
//	void (*App_LedWhiteState)(bool status);
//	void (*App_LedBlueState)(bool status);
//	void (*App_LedGreenState)(bool status);
//	void (*App_LedredState)(bool status);

//	void (*App_LedManagement)(void);
//}LED_DRIVER_APP_API;

//extern LED_DRIVER_APP_API Led_Driver_App_Port;//API接口供外部使用

#endif
