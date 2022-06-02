#include "../FUNCTION/PERIPHERAL/peripheral.h"
#include "../FUNCTION/LED/rgb_led.h"
#include "../function/protocol/protocol.h"
#include "../function/time/hal_timer.h"

/************************************************************  
FileName: peripheral.c   
Author: hf             
Date: 2021.03.23
Description: 手枪其他外设初始化及驱动        
Version: V 1.0  
Other:
***********************************************************/ 

uint8_t gun_statusflg[2]={0};//手枪数据及上传状态
u8 bullet_count=0;//扳机计数
u8 recoil_stop;//扳机与套筒联动限制位 211111

extern u32 sleep_count;
extern u16 Tim4_Tim_Flg;

/**************************************************************   	
函数名称：	Ziyuan_GunState_Init
函数功能：	外设初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Ziyuan_GunState_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	RCC_APB2PeriphClockCmd(TRIGGER_GPIO_CLK | GUN_REST_GPIO_CLK | CHAMBERING_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= TRIGGER_GPIO_PIN;//扳机初始化
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(TRIGGER_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= INSURANCE_GPIO_PIN;//保险初始化
	GPIO_Init(INSURANCE_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= GUN_REST_GPIO_PIN;//复位初始化
	GPIO_Init(GUN_REST_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= CHAMBERING_GPIO_PIN;//上膛初始化
	GPIO_Init(CHAMBERING_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= UP_CLIP_GPIO_PIN;//上弹夹初始化
	GPIO_Init(UP_CLIP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= DOWN_CLIP_GPIO_PIN;//退弹夹初始化
	GPIO_Init(DOWN_CLIP_GPIO_PORT, &GPIO_InitStructure);
}

/**************************************************************   	
函数名称：	Ziyuan_Touch_Init
函数功能：	触摸及其电源控制初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/
void Ziyuan_Touch_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(TOUCH_GPIO_CLK, ENABLE);
	//GPIOE.2 中断线以及中断初始化配置   下降沿触发
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	
	GPIO_InitStructure.GPIO_Pin 	= TOUCH_POWER_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(TOUCH_POWER_GPIO_PORT, &GPIO_InitStructure);
	
	//电源初始化
	GPIO_InitStructure.GPIO_Pin 	= TOUCH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(TOUCH_GPIO_PORT, &GPIO_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键WK_UP所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2， 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); 
		
	TOUCH_POWER=1;//Touch供电
}

//外部中断8服务程序 
void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line8); //清除LINE8上的中断标志位   
}

/**************************************************************   	
函数名称：	Gun_All_Status
函数功能：	手枪状态处理
入口参数：	无
返回参数：	无
说明：	    0：未按下或未触发 1：按下或触发
**************************************************************/

void Gun_All_Status(void)
{
	/*********退弹************/
	if(DOWN_CLIP_BUTTON)								//退弹未触发
		gun_statusflg[0] &= (~0x40);
	else
	{
		gun_statusflg[0] |= 0x40;						//退弹触发
		gun_statusflg[1]=0x01;
	}
		
	/*********触摸************/
	if(!TOUCH_BUTTON)										//无触摸
	{
		gun_statusflg[0]&=(~0x80);
	}
	else
	{
		gun_statusflg[0]|=0x80;							//触摸
		sleep_count=0;										//清除睡眠计数
	}
	
	/*********保险************/	
	if(INSURANCE_BUTTON)								//保险未按下 置1
	{
		gun_statusflg[0]|=0x20;						
	}		
	else//保险拉下
	{	
		gun_statusflg[0]&=(~0x20);						//保险按下 置0
	}
	
	/*********扳机************/
	if(!TRIGGER_BUTTON)//扳机松开
	{	
		gun_statusflg[0] &= (~0x10);
		
		if(Tim4_Tim_Flg>500)
		{
			if(!CHAMBERING_BUTTON)					//未上膛 
				gun_statusflg[0] &= (~0x02);
			else
			{
				gun_statusflg[0] |= 0x02;				//上膛
			}
		}
	}			
	else
	{
			Tim4_Tim_Flg=0;//计数清零 与套筒限位
			
			recoil_stop=1;
			gun_statusflg[0] |= 0x10;						//扳机按下
			
			gun_statusflg[1]=0x01;
	}
	
	/*********上弹************/
	if(UP_CLIP_BUTTON)									//上弹未触发
		gun_statusflg[0]&=(~0x08);
	else
	{
		gun_statusflg[0]|=0x08;							//上弹触发
		gun_statusflg[1]=0x01;
	}
	
	/*********复位************/
	if(REST_BUTTON)											//未复位
		gun_statusflg[0]&=(~0x04);
	else
	{
		gun_statusflg[0]|=0x04;
		gun_statusflg[1]=0x01;
	}
	
	if(!DOWN_CLIP_BUTTON || TOUCH_BUTTON || !INSURANCE_BUTTON || TRIGGER_BUTTON || CHAMBERING_BUTTON || !UP_CLIP_BUTTON || !REST_BUTTON)
		LED_ON;
	else
		LED_OFF;
}
