#include "../function/recoil/recoil.h"
#include "../FUNCTION/TIME/hal_timer.h"
#include "../function/peripheral/peripheral.h"

/************************************************************  
FileName: recoil.c   
Author: hf             
Date: 2021.03.23
Description: 手枪上膛初始化及驱动        
Version: V 1.0  
Other:
***********************************************************/ 

u32 time_count = 0;//换相时间计数
u8 banji_flag = 0;
static u8 flg=0;//扳机单次松开与按下标志位

extern u8 bullet_count;
extern u8 recoi_mode;

/**************************************************************   	
函数名称：	GPIO_Configuration
函数功能：	后坐力部分初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void GPIO_Configuration(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA| \
		RCC_APB2Periph_GPIOC, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
 
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//LIN1~LIN3
   GPIO_Init(GPIOB, &GPIO_InitStructure);
 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;//HIN1~HIN3
   GPIO_Init(GPIOA, &GPIO_InitStructure); 

   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//HADD1/HADD2
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//HADD3
   GPIO_Init(GPIOB, &GPIO_InitStructure);
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器					 
}

/**************************************************************   	
函数名称：	TIME1_PWM_Configuration
函数功能：	定时器1及多通道初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void TIME1_PWM_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;//输出比较
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
 
  TIM_TimeBaseStructure.TIM_Period = 2000;//自动重新加载值
  TIM_TimeBaseStructure.TIM_Prescaler =2;//预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; ////设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2; //计数模式
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;//指定重复计数器的值
  TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure); 
 
  TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;//输出比较状态
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable;//互补输出比较状态
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//指定输出极性
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//指定互补输出极性  仅对“TIM1”和“TIM8”有效
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Reset;//指定空闲状态下的TIM输出比较引脚状态。 仅对“TIM1”和“TIM8”有效
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//指定空闲状态下的TIM输出比较引脚状态  仅对“TIM1”和“TIM8”有效
 
  TIM_OCInitStructure.TIM_Pulse = 5;//指定要加载到捕获比较寄存器的脉冲值 
  TIM_OC1Init(ADVANCE_TIM,&TIM_OCInitStructure);
  TIM_OC1PreloadConfig(ADVANCE_TIM,TIM_OCPreload_Enable);
 
  TIM_OCInitStructure.TIM_Pulse = 5; 
  TIM_OC2Init(ADVANCE_TIM,&TIM_OCInitStructure);
  TIM_OC2PreloadConfig(ADVANCE_TIM,TIM_OCPreload_Enable);
	
  TIM_OCInitStructure.TIM_Pulse = 5; 
  TIM_OC3Init(ADVANCE_TIM,&TIM_OCInitStructure);
  TIM_OC3PreloadConfig(ADVANCE_TIM,TIM_OCPreload_Enable);	
 
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;//在运行模式下使用的off状态选择
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;//空闲状态下使用的off状态
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;//锁定级别参数
  TIM_BDTRInitStructure.TIM_DeadTime = 0x80;//从切换到关闭之间的延迟时间
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;//是否启用TIM Break输入
  //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;//TIM Break输入引脚极性
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//是否启用TIM自动输出特性
  TIM_BDTRConfig(ADVANCE_TIM,&TIM_BDTRInitStructure);
 
  TIM_Cmd(ADVANCE_TIM, ENABLE);  
  TIM_CtrlPWMOutputs(ADVANCE_TIM,ENABLE);
}

/**************************************************************   	
函数名称：	Commutation
函数功能：	换相
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Commutation(void)
{
	 u8 h_io = 0;//高测输入
   h_io = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);		
	 h_io = h_io + (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)<<1);//1<<1 等价于 1*2^1=2
   h_io = h_io + (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)<<2);//1<<2 等价于 1*2^2=4
	 
	switch(h_io)//判断霍尔位置
	{
		case 0x02:   //CA
		{TIM1->CCER = 0x0108;};break;
		case 0x06:   //CB  
		{TIM1->CCER = 0x0180;};break;
		case 0x04:   //AB  
		{TIM1->CCER = 0x0081;};break;
		
		case 0x05:   //AC
		{TIM1->CCER = 0x0801;};break;
		case 0x01:   //BC    
		{TIM1->CCER = 0x0810;};break;
		case 0x03:   //BA 
		{TIM1->CCER = 0x0018;};break;
			default:break;
	}
	
//	switch(h_io)//原版手枪 210426
//	{
//		case 0x04:   //BA  1.4
//		{TIM1->CCER = 0x0018;};break;
//		case 0x06:   //BC  4.5
//		{TIM1->CCER = 0x0810;};break;
//		case 0x02:   //AC  2.5
//		{TIM1->CCER = 0x0801;};break;
//		case 0x03:   //AB  2.3
//		{TIM1->CCER = 0x0081;};break;
//		case 0x01:   //CB  3.6    
//		{TIM1->CCER = 0x0180;};break;
//		case 0x05:   //CA  6.1
//		{TIM1->CCER = 0x0108;};break;
//			default:break;
//	}
}

/**************************************************************   	
函数名称：	Recoil_Init
函数功能：	后坐力部分初始化封装
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Recoil_Init(void)
{
	//计算：72M/7200=10k--->10k=1s--->10=1ms以此类推 注意：arr为u16，MAX65535
	//T=（TIM_Period+1）*（TIM_Prescaler+1）/TIMxCLK
	TIM3_Int_Init(1,35999);
	GPIO_Configuration(); //IO
  TIME1_PWM_Configuration(); //
}

/**************************************************************   	
函数名称：	TIM3_IRQHandler
函数功能：	定时器中断，判断扳机的换相操作
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
		
		if(!recoi_mode)//应答模式
		{
			if(bullet_count == 1)//累计时间 扳机按下就换向
			{
				TIM_Cmd(TIM1,ENABLE);
			  time_count++;
				Commutation();//换相
			}
			
			if(time_count >=120)//计数70次换相一次 相当于换相时长
			{
				TIM1->CCER = 0;//清空
				bullet_count=0;
				TIM_Cmd(TIM1,DISABLE);
			}
		}
		else//本地控制模式
		{
			if(TRIGGER_BUTTON && flg==0)//扳机按下
			{
				TIM_Cmd(TIM1,ENABLE);
				banji_flag = 1;//扳机按下标志
				time_count = 0;
				flg=1;
			}			
			
			if(banji_flag == 1)//累计时间 扳机按下就换向
			{
			  time_count++;
				Commutation();//换相
			}
			
			if(time_count >=120)//计数70次换相一次 相当于换相时长
			{
				TIM1->CCER = 0;//清空
				TIM_Cmd(TIM1,DISABLE);
				
				banji_flag=0;/*防止扳机按下一直通电 不能置于扳机松开判断内，否则无效*/
				
				if(!TRIGGER_BUTTON && flg==1)//扳机松开 单次触发
				{
//					bullet_count++;//消抖计数
//					
//					if(bullet_count>=100)
//					{
//						
//						bullet_count=0;
						flg =0;
						time_count = 0;//重新计时
//					}
				}
			}
		}
	}
}

