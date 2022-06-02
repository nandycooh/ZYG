/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2018 </center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#ifndef __HAL_FLASH_H_
#define __HAL_FLASH_H_

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "delay.h"

/*------------------------------------------------------------------------------------*/
//配对离线数据
__packed typedef struct
{
	uint8_t 	device_num;
	uint8_t   pair_ch;//配对通道
	uint8_t  _XX2;
	uint8_t  _XX3;
}rf_pair_device_t;

//服务器参数离线设置
__packed typedef struct
{
	u8  ip[4];
	char  port[7];//保存的是字符
	u8 xx;
}serverConfig_t;

//服务器参数离线设置
__packed typedef struct
{
	u32 re_boot_flag;
	
}reBoot_t;

//wifi 离线数据保存
//服务器参数离线设置
__packed typedef struct
{
	u32 bund;
}offonline_wifi_t;

//离线保存
typedef struct
{
	serverConfig_t servercofig;
	
	rf_pair_device_t pair_device;
	
	reBoot_t reBoot;
	
	offonline_wifi_t offonline_wifi;
	
}flash_config_t;

//flash管理器
typedef struct
{
	/**
		* @brief  写入数据
		* @param  addr 写入的地址 ,d 写入的数据 len写入的数据的长度
		* @retval : int 返回读到数据的大小
	**/
	int (*Read)(uint32_t addr,u8 *d,uint16_t len);
	/**
		* @brief  配置为输入输出
		* @param  channel io脚 ,out_input 输入输出配置
		* @retval : None
	**/
	void (*Write)(uint32_t addr,u8 *d,uint16_t len);
	
}HalAdapter_Management_FLASH_t;

/*离线参数*/

/* Private Values -------------------------------------------------------------*/
/****************************************************************
*Name:		ReadFlashNBtye
*Function:	从内部Flash读取N字节数据
*Input:		ReadAddress：数据地址（偏移地址）ReadBuf：数据指针	ReadNum：读取字节数
*Output:  读取的字节数  
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, uint16_t ReadNum) ;

/****************************************************************
*Name:		WriteFlashOneWord
*Function:	向内部Flash写入32位数据
*Input:		WriteAddress：数据地址（偏移地址）WriteData：写入数据
*Output:	NULL 
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
void WriteFlashOneWord(uint32_t WriteAddress,uint8_t* Buf,uint16_t size);


void FLASH_Read_MoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
#endif
