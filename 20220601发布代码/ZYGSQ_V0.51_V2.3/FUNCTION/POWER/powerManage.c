#include "../function/power/powerManage.h"
#include "../function/led/rgb_led.h"
#include "../function/time/hal_timer.h"
#include "../function/si4438/drv_SI4438.h"
#include "../function/recoil/recoil.h"

/************************************************************  
FileName: powerManage.c   
Author: hf             
Date: 2021.03.23
Description: 电源部分初始化及控制        
Version: V 1.0  
Other:
***********************************************************/ 

extern TIME_ControlDef TIME_ControlStruct;

/**************************************************************   	
函数名称：	Sys_Standby
函数功能：	待机模式，只能时钟和PA0唤醒
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
static void Sys_Standby(void)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
	PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式 		 
}

/**************************************************************   	
函数名称：	Sys_Enter_Standby
函数功能：	系统进入待机模式
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Sys_Enter_Standby(void)
{	
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//复位所有IO口
	Sys_Standby();
}

/**************************************************************   	
函数名称：	Sys_Enter_Stop
函数功能：	停止模式，可以通过任意外部中断唤醒
入口参数：	无
返回参数：	无
说明：	    因为使用了定时，中断唤醒优先级一定要最高，否则无法唤醒
**************************************************************/ 
void Sys_Enter_Stop(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//唤醒后从此处开始继续运行
	SystemInit();
	//恢复关闭的功能
	SI44xx_Power_Init();//先供电才能初始化
	delay_ms(1000);//等待充电完成
	
	SI446x_Init();
	Recoil_Init();
	TIME_ControlStruct.start(1,1000);								//初始化定时器2 1S 睡眠计数
	TIME_ControlStruct.start(2,1);
}


/**************************************************************   	
函数名称：	Enter_Sleep
函数功能：	系统进入低功耗模式
入口参数：	无
返回参数：	无
说明：	    这里选择进入停止模式，可以通过任意外部中断唤醒
**************************************************************/ 
void Enter_Sleep(void)
{
	//定时器任务要暂停
	TIME_ControlStruct.stop(1);
	TIME_ControlStruct.stop(2);
	TIME_ControlStruct.stop(3);
	//433休眠
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//4438电源断
	//灯灭
	LED_OFF;
	
	Sys_Enter_Stop();
	
//	/*重启*/
//	__disable_irq();
//	NVIC_SystemReset();
}
