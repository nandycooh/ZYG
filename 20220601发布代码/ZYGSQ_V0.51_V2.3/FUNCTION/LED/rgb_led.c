#include "../function/led/rgb_led.h"
#include "../function/power/powerManage.h"

/************************************************************  
FileName: rgb_led.c   
Author: hf             
Date: 2021.03.23
Description: LED初始化及底层驱动        
Version: V 1.0  
Other:
***********************************************************/ 

/*********************************硬件驱动层****************************/
/**************************************************************   	
函数名称：	Led_Init
函数功能：	LED初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}

/*********************************功能实现层****************************/
/**************************************************************   	
函数名称：	led_ctrl
函数功能：	LED闪烁
入口参数：	times：1秒钟闪烁次数
返回参数：	无
说明：	    无
**************************************************************/ 
void led_ctrl(u8 times)
{
	LED_PIN_PORT = !LED_PIN_PORT;
	delay_ms(1000/times);
}

LED_ControlDef LED_ControlStructf = 
{
	.Driver_LedInit = Led_Init,
};
