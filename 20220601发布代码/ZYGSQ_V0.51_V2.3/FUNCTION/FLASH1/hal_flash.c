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

#include "hal_flash.h"

/*参考《STM32F10xxx闪存编程》
*64k-128k属于中容量产品，扇区每页为1k，STM32F103CBT6总共127k
*注意写入的起始地址不能与程序运行地址冲突
*/
#define  STARTADDR  0x0801FC00      //STM32F103RB 其他型号基本适用，未测试 

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;   //Flash操作状态变量

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
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, uint16_t ReadNum)
{
	int DataNum = 0;
	ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
	while(DataNum < ReadNum) 
	{
		 *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;
		 DataNum++;
	}
	return DataNum;
}

static u32 byteToUint(uint8_t* pBuf)
{
	return (pBuf[3]<<24 | pBuf[2]<<16 |pBuf[1]<<8 | pBuf[0]);
}

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
void WriteFlashOneWord(uint32_t WriteAddress,uint8_t* Buf,uint16_t size)
{
	int i = 0;
	int j = 0;

	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
  FLASHStatus = FLASH_ErasePage(STARTADDR);
	
	if(FLASHStatus == FLASH_COMPLETE)
	{
		for(i = 0;i<size/4;i++)
		{
			FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, byteToUint(&Buf[j]));    //flash.c 中API函数
			WriteAddress += 4;
			j+=4;
		}
	}
	FLASH_LockBank1();
}

/*离线参数*/
flash_config_t flash_config;

HalAdapter_Management_FLASH_t GFLASH={
	.Read=ReadFlashNBtye,
	.Write=WriteFlashOneWord
};
