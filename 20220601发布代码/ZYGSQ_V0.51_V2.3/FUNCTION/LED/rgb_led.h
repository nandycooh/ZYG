#ifndef __RGB_LED_H
#define __RGB_LED_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include "delay.h"

//基础颜色类型
#define LED_GPIO_PORT			GPIOB								
#define LED_GPIO_CLK			RCC_APB2Periph_GPIOB
#define LED_GPIO_PIN			GPIO_Pin_2

#define LED_PIN_PORT			PBout(2)
#define LED_OFF   				PBout(2)=0
#define LED_ON    				PBout(2)=1

/*********************************函数申明区****************************/
void Led_Init(void);
void Led_ShutDown(void);
void Led_Management(void);
/*********************************宏定义功能区**************************/


/*********************************API接口层****************************/
//LED硬件管理器
typedef struct
{	
	void (*Driver_LedInit)(void);
	void (*App_LedManagement)(void);
}LED_ControlDef;


#endif
