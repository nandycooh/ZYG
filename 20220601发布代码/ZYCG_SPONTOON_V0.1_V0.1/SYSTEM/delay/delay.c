#include "delay.h"


static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
volatile uint32_t g_ul_ms_ticks=0;	

			   
////初始化延迟函数
////当使用OS的时候,此函数会初始化OS的时钟节拍
////SYSTICK的时钟固定为HCLK时钟的1/8
////SYSCLK:系统时钟
//void delay_init()
//{
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
//	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  

//	fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数   
//}								    

//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()	 
{
#if CLOCKTICKER
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	if (SysTick_Config(SystemCoreClock/100000))
	{ 
		/* Capture error */ 
		while (1);
	}
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#else 
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数
#endif	
}


#if CLOCKTICKER
	/**
  * @brief  软件延迟MS
  * @param  None
  * @retval : None
**/
static void delay_ms(u16 time)
{
  u16 i;
	if(time == 0x00)
		return;
	while(time--)
	{
		i=12000;
		while(i--);
	}
}

/**
  * @brief  软件延迟US
  * @param  None
  * @retval : None
**/
static void delay_us(u32 time)
{
  u16 i;
	if(time == 0x00)
		return;
	while(time--)
	{
		i=12;
		while(i--);
	}
}

static void TimeStamp_Increment(void)
{
	g_ul_ms_ticks++;
}

static void get_tick_count(unsigned long *count)
{
  count[0] = g_ul_ms_ticks;
}

#else 

//延时nus
//nus为要延时的us数.		    								   
static void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
static void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
}
#endif

//SysTick_Handler调用
void TimeStamp_Increment(void)
{
	g_ul_ms_ticks++;
}

void get_tick_count(unsigned long *count)
{
  count[0] = g_ul_ms_ticks;
}


HalAdapter_Management_Clock_t halAdapter_Clock = 
{
//.isInit = false,
.HAL_ClockInit = delay_init,
.HAL_DelayUS = delay_us,
.HAL_DelayMS = delay_ms,
.HAL_Get_TickMS = get_tick_count,
};


/**
  * @brief 硬件Clock实例且初始化
  * @param
  * @retval: None
*/
static HalAdapter_Management_Clock_t* HAL_adapterInit_Clock(void)
{
//	if(halAdapter_Clock.isInit == false){
		//实例化
		halAdapter_Clock.HAL_ClockInit();
		halAdapter_Clock.isInit = true;
//	}
	return &halAdapter_Clock;
}

/**
  * @brief 获取硬件时钟实例
  * @param
  * @retval: None
*/
HalAdapter_Management_Clock_t* GetHalAdapter_Clock(void)
{
	return HAL_adapterInit_Clock();
}































