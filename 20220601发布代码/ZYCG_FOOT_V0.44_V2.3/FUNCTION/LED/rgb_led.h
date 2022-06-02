#ifndef __RGB_LED_H
#define __RGB_LED_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>

//基础颜色类型
typedef enum
{
	WHITE = 0,
	YELLOW,
	GREEN,
	BLUE,
	PURPLE,
	RED,
	BLACK,
	NONE
}COLOR_U;

/*********************************宏定义功能区**************************/

//基础颜色类型
#define LED_GPIO_PORT				GPIOB								
#define LED_GPIO_CLK				RCC_APB2Periph_GPIOB
#define LED_RED_GPIO_PIN 		GPIO_Pin_15
#define LED_GREEN_GPIO_PIN 	GPIO_Pin_14
#define LED_BLUE_GPIO_PIN 		GPIO_Pin_13

#define BLUE_LED   PBout(13)
#define GREEN_LED  PBout(14)
#define RED_LED    PBout(15)

#define RED     1
#define GREEN   2
#define BLUE    3
#define YELLOW  4
#define PURPLE  5//紫色
#define NAVY    6
#define WHITE   7


#define LED_OFF   0
#define LED_ON    1
/*********************************函数申明区****************************/
void Led_Init(void);
void Led_ShutDown(void);
void led_ctrl(u8 colour, bool state);
void led_red_state(bool status);
void led_turn_state(u8 colour);
void Led_Management(void);
void led_red_turn(void);
/*********************************宏定义功能区**************************/
#define BLUE_LED_ON    BLUE_LED = 0;\
                       GREEN_LED = 1;\
                       RED_LED = 1
#define BLUE_LED_OFF   BLUE_LED = 1;\
                       GREEN_LED = 1;\
                       RED_LED = 1
#define GREEN_LED_ON   GREEN_LED = 0;\
                       BLUE_LED = 1;\
                       RED_LED = 1
#define GREEN_LED_OFF  BLUE_LED = 1;\
                       GREEN_LED = 1;\
                       RED_LED = 1
#define RED_LED_ON     RED_LED = 0;\
                       BLUE_LED = 1;\
                       GREEN_LED = 1
#define RED_LED_OFF    BLUE_LED = 1;\
                       GREEN_LED = 1;\
                       RED_LED = 1
#define WHITE_LED_ON   BLUE_LED = 0;\
                       GREEN_LED = 0;\
                       RED_LED = 0
#define WHITE_LED_OFF  BLUE_LED_OFF;\
                       GREEN_LED_OFF;\
                       RED_LED_OFF

#define YELLOW_LED_ON  BLUE_LED = 1;\
                       GREEN_LED = 0;\
                       RED_LED = 0
#define YELLOW_LED_OFF  BLUE_LED = 1;\
                       GREEN_LED = 1;\
                       RED_LED = 1
					   
/*********************************API接口层****************************/
//LED硬件管理器
typedef struct
{
	void (*Driver_LedInit)(void);
	void (*Driver_LedShutDown)(void);
	void (*App_LedTurnState)(u8 colour);
	
	void (*App_LedWhiteState)(bool status);
	void (*App_LedBlueState)(bool status);
	void (*App_LedGreenState)(bool status);
	void (*App_LedredState)(bool status);

	void (*App_LedManagement)(void);
}LED_DRIVER_APP_API;


void led_blue_turn(void);
void Led_Management(void);
#endif
