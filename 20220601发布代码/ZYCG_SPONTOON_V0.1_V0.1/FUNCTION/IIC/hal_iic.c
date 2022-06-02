#include "hal_iic.h"
#include "delay.h"
 
  //MPU IIC 延时函数
static void IIC_Delay(void)
{
	halAdapter_Clock.HAL_DelayUS(2);
}

//初始化IIC
static void IIC_Init(void)
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//先使能外设IO PORTC时钟 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
  GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);						 //PB10,PB11 输出高	
 
}
//产生IIC起始信号
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	IIC_Delay();
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	IIC_Delay();
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	IIC_Delay();
	IIC_SCL=1;  
	IIC_SDA=1;//发送I2C总线结束信号
	IIC_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;
	IIC_Delay();	   
	IIC_SCL=1;
	IIC_Delay();	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	IIC_SCL=0;
}
//不产生ACK应答		    
static void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
static void IIC_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA_OUT(); 	    
  IIC_SCL=0;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	  
		IIC_SCL=1;
		IIC_Delay(); 
		IIC_SCL=0;	
		IIC_Delay();
  }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    IIC_SCL=0; 
    IIC_Delay();
		IIC_SCL=1;
    receive<<=1;
    if(READ_SDA)receive++;   
		IIC_Delay(); 
  }					 
  if (!ack)
    IIC_NAck();//发送nACK
  else
    IIC_Ack(); //发送ACK   
  return receive;
}



HalAdapter_Management_IIC_t halAdapter_IIC = 
{
.isInit = false,
.HAL_IIC_Init = IIC_Init,
.HAL_IIC_Start = IIC_Start,
.HAL_IIC_Stop = IIC_Stop,
.HAL_IIC_Wait_Ack = IIC_Wait_Ack,
.HAL_IIC_Ack = IIC_Ack,
.HAL_IIC_NAck = IIC_NAck,
.HAL_IIC_Write_Byte = IIC_Send_Byte,
.HAL_IIC_Read_Byte = IIC_Read_Byte,
};
/**
  * @brief 硬件IIC实例且初始化
  * @param
  * @retval: None
*/
static HalAdapter_Management_IIC_t* HAL_adapterInit_IIC(void)
{
	if(halAdapter_IIC.isInit == false){
		//实例化
		halAdapter_IIC.HAL_IIC_Init();
		halAdapter_IIC.isInit = true;
	}
	return &halAdapter_IIC;
}
/**
  * @brief 获取硬件ADC实例
  * @param
  * @retval: None
*/
HalAdapter_Management_IIC_t* GetHalAdapter_IIC(void)
{
	return HAL_adapterInit_IIC();
}














