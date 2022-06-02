#ifndef __HAL_IIC_H
#define __HAL_IIC_H

#include "sys.h"
#include "stdio.h"
#include "stdbool.h"

/***************************IIC********************************/
//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}

//IO操作函数	 
#define IIC_SCL    PBout(6) 		//SCL
#define IIC_SDA    PBout(7) 		//SDA	 
#define READ_SDA   PBin(7) 		//输入SDA 

//IIC管理器
typedef struct
{
	bool isInit;
	/**
		* @brief  IIC初始化
		* @param  None
		* @retval : None
	**/
	void (*HAL_IIC_Init)(void);
	/**
		* @brief  IIC起始信号
		* @param  None
		* @retval : None
	**/
	void (*HAL_IIC_Start)(void);
	/**
		* @brief  IIC停止信号
		* @param  None
		* @retval : None
	**/
	void (*HAL_IIC_Stop)(void);
	/**
		* @brief  IIC等待应答
		* @param  None
		* @retval : None
	**/
	u8 (*HAL_IIC_Wait_Ack)(void);
	/**
		* @brief  IIC应答
		* @param  None
		* @retval : None
	**/
	void (*HAL_IIC_Ack)(void);
	/**
		* @brief  IIC不应答
		* @param  None
		* @retval : None
	**/
	void (*HAL_IIC_NAck)(void);
	/**
		* @brief  IIC写数据
		* @param  单个字节
		* @retval : None
	**/
	void (*HAL_IIC_Write_Byte)(u8);
	
	/**
		* @brief 	IIC读数据
		* @param  是否应答
		* @retval : 单个字节
	**/
	u8 (*HAL_IIC_Read_Byte)(unsigned char);

	
}HalAdapter_Management_IIC_t;

HalAdapter_Management_IIC_t* GetHalAdapter_IIC(void);
	  
#endif
















