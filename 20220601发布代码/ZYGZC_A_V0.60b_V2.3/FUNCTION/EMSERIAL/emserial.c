#include "emserial.h"


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{  
    void SendOneByte(u8 Byte);
	SendOneByte((uint8_t) ch);    
	return ch;
}
#endif 

/**
  * @brief TIM3配置
  * @param  None
  * @retval : None
**/
static void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3,ENABLE);    

}

/*********************************硬件驱动层****************************/
void Emserial_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(EMULATE_SERIAL_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(EMULATE_SERIAL_PORT, EMULATE_SERIAL_PIN);           //设置默认位为高电平
	TIM3_Configuration();
}

void SerialDelay_Us(__IO uint32_t nTime)
{ 

	uint16_t tmp;

	tmp = TIM_GetCounter(TIM3);
	
	if(tmp + nTime <= 65535)
		while( (TIM_GetCounter(TIM3) - tmp) < nTime );
	else
	{
		TIM_SetCounter(TIM3, 0);
		while( TIM_GetCounter(TIM3) < nTime );
	}	
}

//模拟引脚发送单个字节
void SendOneByte(u8 Byte)
{
  u8 i = 0;
  EMULATE_SERIAL_IO=0;             //引脚拉低，即将发送数据
  SerialDelay_Us(BuadRate115200);  //延时104us
  for(i = 0; i < 8; i++) //8位数据位
  {
      if(Byte & 0x01)    //如果bit位为1
          EMULATE_SERIAL_IO=1;  //引脚拉高
      else    //否则拉低
          EMULATE_SERIAL_IO=0;
      SerialDelay_Us(BuadRate115200);
      Byte >>= 1;
  }
  EMULATE_SERIAL_IO=1;   //停止位
  SerialDelay_Us(BuadRate115200);
	delay_ms(1);//防止乱码210723
}

void SendBytes(u8 *str,u8 len)	//发送数组最好用这个，也可发送字符串
{
  u16 i;
  for(i=0;i<len;i++)
  {
  	SendOneByte(*str);
  }
}

void SendStr(u8 *str) //发送字符串，发送数组如果带0x00就中断发送了
{
 while(*str)
 SendOneByte(*str++);

}
