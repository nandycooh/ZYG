#include "sleep.h"

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
	//SystemInit();
	delay_init();
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
//	LED_OFF;
	//定时器任务要暂停
//	TIME_ControlStruct.stop(1);
//	TIME_ControlStruct.stop(2);
	/*伪待机，睡眠模式*/
	//4438待机或关机模式
	
//	RF_433_470.ShutDown();
	ADC_Cmd(ADC1, DISABLE);

	Sys_Enter_Stop();
	/*重启*/
	__disable_irq();
	NVIC_SystemReset();
}

void ALL_GPIO_AN(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|
																GPIO_Pin_1|
																GPIO_Pin_2|
																GPIO_Pin_3|
																GPIO_Pin_4|
																GPIO_Pin_5|
																GPIO_Pin_6|
																GPIO_Pin_7|
//																GPIO_Pin_8|
																GPIO_Pin_9|
																GPIO_Pin_10|
																GPIO_Pin_11|
																GPIO_Pin_12|
																GPIO_Pin_13|
																GPIO_Pin_14|
																GPIO_Pin_15;
	//选择GPIO响应速度
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    
	//设置为输出           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	//初始化    			
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|
																GPIO_Pin_1|
																GPIO_Pin_2|
//																GPIO_Pin_3|
																GPIO_Pin_4|
																GPIO_Pin_5|
																GPIO_Pin_6|
																GPIO_Pin_7|
																GPIO_Pin_8|
																GPIO_Pin_9|
																GPIO_Pin_10|
																GPIO_Pin_11;
//																GPIO_Pin_12|
//																GPIO_Pin_13|
//																GPIO_Pin_14|
//																GPIO_Pin_15;
	//选择GPIO响应速度
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    
	//设置为输出           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	//初始化    			 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

void Un_Used_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5  | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

static void Restore_Default_Settings(void)
{
	ALL_GPIO_AN();
	TIM_DeInit(TIM4);
	TIM_DeInit(TIM2);
	ADC_DeInit(ADC1);
	USART_DeInit(USART1);
	USART_DeInit(USART2);
	USART_DeInit(USART3);
}

static void Enter_Stop(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	SysTick->CTRL = 0x00;//关闭定时器
	SysTick->VAL = 0x00;//清空val,清空定时器;
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//重点来了这个就是进入STOP
	SystemInit();   //初始化系统时钟
}

static  void Enter_LowPowerStop(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	SysTick->CTRL = 0x00;//关闭定时器
	SysTick->VAL = 0x00;//清空val,清空定时器;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//重点来了这个就是进入STOP
	SystemInit();   //初始化系统时钟
}

static void Enter_Standby(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;  		  
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA和复用功能时钟

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    //使用外部中断方式
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	//中断线0连接GPIOA.0

  EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	// 初始化外部中断

	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//复位所有IO口
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
	PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式
}

void EXTI0_IRQHandler(void)
{    
	EXTI_ClearITPendingBit(EXTI_Line0);    
} 

static void Restart(void)
{
	__disable_irq();
	NVIC_SystemReset();
}
/**
  * @brief 硬件Sleep实例且初始化
  * @param
  * @retval: None
*/
HalAdapter_Management_OperatingMode_t halAdapter_OptMode = 
{
.isInit = true,
.HAL_EnterStop = Enter_Stop,
.HAL_EnterLowPowerStop = Enter_LowPowerStop,
.HAL_Restart = Restart,
.HAL_EnterStandby = Enter_Standby,
.HAL_RestoreDefaultSettings = Restore_Default_Settings,
};

static HalAdapter_Management_OperatingMode_t* HAL_adapterInit_OptMode(void)
{
	if(halAdapter_OptMode.isInit == false){
		//实例化
		//ALL_GPIO_AN();
		halAdapter_OptMode.isInit = true;
	}
	return &halAdapter_OptMode;
}
/**
  * @brief 获取硬件Sleep实例
  * @param
  * @retval: None
*/
HalAdapter_Management_OperatingMode_t* GetHalAdapter_OptMode(void)
{
	return HAL_adapterInit_OptMode();
}
