#include "hal_adc.h"


/************************************************************  
FileName: hal_adc.c   
Author: hf             
Date: 2021.03.23
Description: adc初始化及底层驱动        
Version: V 1.0  
Other:
***********************************************************/ 


/**************************************************************   	
函数名称：	adc_init
函数功能：	adc初始化
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
void Adc_init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable ADC1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12MHZ 

	/* Configure PAx as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	//单次模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	//是否工作在连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//ADC数据对齐方式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//规定了顺序进行转换的ADC通道数目
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
    ADC_TempSensorVrefintCmd(ENABLE);  //使能内部温度传感器和内部参考电压

	//设定单个通道转换时间
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_13Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	/* 启用ADC1复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1复位校准寄存器的末端 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 开始ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查ADC1校准结束 */
	while(ADC_GetCalibrationStatus(ADC1));
}

void Adc_ShutDown(void)
{
	ADC_Cmd(ADC1, DISABLE);
}

//static void Adc_Start(void)
//{
//	ADC_Cmd(ADC1, ENABLE);
//}

/**************************************************************   	
函数名称：	Get_Adc
函数功能：	获得ADC值
入口参数：	ch:通道值 0~3
返回参数：	无
说明：	    无
**************************************************************/ 
static u16 Get_Adc(u8 ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//static float Calculate_AdcToVoltage(void)
//{
//	u16 adc;
//	adc=Get_Adc(ADC_Channel_1);
//	return (float)adc* 2 * (1.2/Get_Adc(ADC_Channel_17));  //(6 = 基准电压 * 2)
//}

/**************************************************************   	
函数名称：	Get_Adc_Average
函数功能：	求获得ADC值的平均值
入口参数：	ch		:	通道值 0~3
						times	：转换次数
返回参数：	无
说明：	    无
**************************************************************/ 
u16 Get_Adc_Average(u8 ch, u8 times)
{
	u32 temp_val=0;
	
	Get_Adc(ch);
	for(u8 i=0; i<times; i++) {
		temp_val += Get_Adc(ch);
//		delay_ms(5);
	}
	return temp_val/times;
} 

//float calculate_bat(void)
//{
//	u16 adc;
//	adc=Get_Adc_Average(ADC_Channel_1,SAMPE_TIMES);
//	return (float)adc*6/4096;  //(6 = 基准电压 * 2)
//}

/**************************************************************   	
函数名称：	calculate_bat
函数功能：	电压计算
入口参数：	无
返回参数：	无
说明：	    无
**************************************************************/ 
float calculate_bat(void)
{
	float adcnum;
	u16 adc;
	
	adc=Get_Adc_Average(ADC_Channel_1,SAMPE_TIMES);
	adcnum = (float)adc*(3.3/4096);
	return (float)adc*(1.2/Get_Adc_Average(ADC_Channel_17,SAMPE_TIMES));  //(6 = 基准电压 * 2)
}
