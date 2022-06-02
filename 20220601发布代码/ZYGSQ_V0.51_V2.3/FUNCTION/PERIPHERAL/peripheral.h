#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H	

#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "sys.h"
#include "stm32f10x.h"

//保险
#define INSURANCE_GPIO_PORT			GPIOA								
#define INSURANCE_GPIO_CLK			RCC_APB2Periph_GPIOA
#define INSURANCE_GPIO_PIN			GPIO_Pin_11
//扳机
#define TRIGGER_GPIO_PORT				GPIOA								
#define TRIGGER_GPIO_CLK				RCC_APB2Periph_GPIOA
#define TRIGGER_GPIO_PIN				GPIO_Pin_4
//复位
#define GUN_REST_GPIO_PORT			GPIOB								
#define GUN_REST_GPIO_CLK				RCC_APB2Periph_GPIOB
#define GUN_REST_GPIO_PIN				GPIO_Pin_11
//上膛
#define CHAMBERING_GPIO_PORT		GPIOC								
#define CHAMBERING_GPIO_CLK			RCC_APB2Periph_GPIOC
#define CHAMBERING_GPIO_PIN			GPIO_Pin_13
//上弹夹
#define UP_CLIP_GPIO_PORT				GPIOC								
#define UP_CLIP_GPIO_CLK				RCC_APB2Periph_GPIOC
#define UP_CLIP_GPIO_PIN				GPIO_Pin_14
//退弹夹
#define DOWN_CLIP_GPIO_PORT			GPIOC								
#define DOWN_CLIP_GPIO_CLK			RCC_APB2Periph_GPIOC
#define DOWN_CLIP_GPIO_PIN			GPIO_Pin_15

//触摸检测
#define TOUCH_GPIO_PORT				GPIOB								
#define TOUCH_GPIO_CLK				RCC_APB2Periph_GPIOB
#define TOUCH_GPIO_PIN				GPIO_Pin_8
//触摸电源控制
#define TOUCH_POWER_GPIO_PORT			GPIOB								
#define TOUCH_POWER_GPIO_CLK			RCC_APB2Periph_GPIOB
#define TOUCH_POWER_GPIO_PIN			GPIO_Pin_9

#define TRIGGER_BUTTON 			PAin(4)	//扳机按钮
#define INSURANCE_BUTTON 		PAin(11)//保险按钮
#define REST_BUTTON 				PBin(11)	//复位按钮
#define CHAMBERING_BUTTON 	PCin(13)	//上膛按钮
#define UP_CLIP_BUTTON 			PCin(14)	//上弹夹按钮
#define DOWN_CLIP_BUTTON 		PCin(15)	//退弹夹按钮
#define TOUCH_BUTTON 				PBin(8)	//触摸按钮

#define TOUCH_POWER					PBout(9)//触摸电源控制

#define setbit(x,y) x|=(1<<y) //将X的第Y位置1
#define clrbit(x,y) x&=!(1<<y) //将X的第Y位清0

#define BULLET_NUMBER 12

void Ziyuan_GunState_Init(void);
void Ziyuan_Touch_Init(void);

void Gun_All_Status(void);
#endif
