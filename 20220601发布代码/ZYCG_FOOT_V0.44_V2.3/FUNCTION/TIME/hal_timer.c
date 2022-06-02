#include "hal_timer.h"
#include "rgb_led.h"
#include "powerManage.h"

/************************************************************  
FileName: hal_timer.c   
Author: hf             
Date: 2021.03.23
Description: tim初始化及底层驱动        
Version: V 1.0  
Other:
***********************************************************/ 




u8 Low_Battery_Flg=0;
u8 dgbugcount=0;
/**************************************************************   	
函数名称：	TIM2_Int_Init
函数功能：	tim2初始化
入口参数：	arr：自动重装值。
            psc：时钟预分频数
返回参数：	无
说明：	    无
**************************************************************/ 
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx
}
/**************************************************************   	
函数名称：	TIM3_Int_Init
函数功能：	通用定时器3中断初始化
入口参数：	arr：自动重装值。
            psc：时钟预分频数
返回参数：	无
说明：	    无
**************************************************************/ 
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}
/**************************************************************   	
函数名称：	TIM4_Int_Init
函数功能：	tim4初始化
入口参数：	arr：自动重装值。
            psc：时钟预分频数
返回参数：	无
说明：	    无
**************************************************************/ 
void TIM4_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx

//	Tim4_Tim_Flg=0;
}

/**************************************************************   	
函数名称：	timer_start
函数功能：	定时器启动
入口参数：	timerEx_id：指定定时器 本次设定2个
            timer_period：定时器时间 单位ms
返回参数：	无
说明：	    无
**************************************************************/ 
void timer_start(u8 timerEx_id,uint32_t timer_period)
{
	if(timerEx_id==1)
	{
		//计算：72M/7200=10k--->10k=1s--->10=1ms以此类推 注意：arr为u16，MAX65535
		TIM2_Int_Init(timer_period*10-1,7199);//10Khz的计数频率，计数到5000为500ms
	}
	else if(timerEx_id==2)
	{
		TIM3_Int_Init(timer_period*10-1,7199);
	}
	else if(timerEx_id==3)
		TIM4_Int_Init((timer_period*10)-1,7199);
}


/**************************************************************   	
函数名称：	timer_stop
函数功能：	timer暂停
入口参数：	timerEx_id：指定暂停的定时器 本次设定2个
返回参数：	无
说明：	    无
**************************************************************/ 
static void timer_stop(u8 timerEx_id)
{
	if(timerEx_id==1)
		TIM_Cmd(TIM2, DISABLE);
	else if(timerEx_id==2)
		TIM_Cmd(TIM3, DISABLE);
	else if(timerEx_id==3)
		TIM_Cmd(TIM4, DISABLE);
}


void ApplicationTimer_Stop(void)
{
		timer_stop(1);
		timer_stop(2);
}

extern void UpdateBatteryQuantity(void);
extern void UpdateChargingInterfaceStatus(void);
extern void DormancyState_Judge(void);
//定时器2中断服务程序
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志
		UpdateChargingInterfaceStatus();//充电接口 210609
		UpdateBatteryQuantity();//低电量判断 210609
		DormancyState_Judge();//210609 TESTOK
	}
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志
		Low_Battery_Flg++;
		dgbugcount++;
	}
}

//定时器4中断服务程序
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
	}
}

/*外部调用api变量定义*/
TIME_ControlDef TIME_ControlStruct = 
{
	.start = timer_start,
	.stop  = timer_stop,
};







