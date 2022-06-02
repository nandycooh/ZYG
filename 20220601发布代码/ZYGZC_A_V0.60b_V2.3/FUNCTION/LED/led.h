#ifndef __LED_H
#define __LED_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>

#define LED_GPIO_PORT			GPIOA								
#define LED_GPIO_CLK			RCC_APB2Periph_GPIOA

//基础颜色类型
#define LED_RED_PIN 		GPIO_Pin_7
#define LED_GREEN_PIN 	GPIO_Pin_6
#define LED_BLUE_PIN 		GPIO_Pin_5

#define LED_OFF   0
#define LED_ON    1

#define BLUE_LED   PAout(5)
#define GREEN_LED  PAout(6)
#define RED_LED    PAout(7)

#define RED     1
#define GREEN   2
#define BLUE    3
#define YELLOW  4
#define PURPLE  5
#define NAVY    6
#define WHITE   7

/*********************************函数申明区****************************/
void Led_Init(void);
void Led_ShutDown(void);
void led_green_turn(void);
void led_red_turn(void);
void led_blue_turn(void);
void led_white_turn(void);
void led_ctl(u8 colour);
void led_green_off(void);
void led_red_off(void);
void led_blue_off(void);
void led_blue_on(void);
void led_white_off(void);
void led_white_on(void);

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

///*********************************API接口层****************************/
//LED硬件管理器
typedef struct
{
	bool isInit;

	void (*HAL_LedInit)(void);
	void (*HAL_LedShutDown)(void);
	void (*HAL_LedGreenTurn)(void);
	void (*HAL_LedRedTurn)(void);
	void (*HAL_LedBlueTurn)(void);
	void (*HAL_LedWhiteTurn)(void);
	void(*HAL_LedPurpleTurn)(void);
	void(*HAL_LedYellowTurn)(void);
	
	void (*HAL_LedWhiteOn)(void);
	void (*HAL_LedBlueOn)(void);
	
	void (*HAL_LedGreenOff)(void);
	void (*HAL_LedredOff)(void);
	void (*HAL_LedblueOff)(void);
	void (*HAL_LedwhiteOff)(void);
	
	void (*HAL_LedpurpleOn)(void);
}HalAdapter_Management_Led_t;

#endif
