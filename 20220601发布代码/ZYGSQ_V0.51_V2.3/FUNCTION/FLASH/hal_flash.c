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

#define  STARTADDR  0x08010000      //STM32F103RB 其他型号基本适用，未测试 
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

/*
#define FLASH_SIZE 128          //所选MCU的FLASH容量大小(单位为K)

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024    //字节
#else 
  #define SECTOR_SIZE           2048    //字节
#endif

//读取指定地址的半字(16位数据)
uint16_t FLASH_Read_HalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}
//读取指定地址的全字(32位数据)
uint32_t FLASH_ReadWord(uint32_t address)
{
  uint32_t temp1,temp2;
  temp1=*(__IO uint16_t*)address; 
  temp2=*(__IO uint16_t*)(address+2); 
  return (temp2<<16)+temp1;
}
//从指定地址开始读取多个数据
void FLASH_Read_MoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToRead;dataIndex++)
  {
    readData[dataIndex]=FLASH_ReadWord(startAddress+dataIndex);
  }
}



//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
  if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE)))
  {
    return;//非法地址
  }
  FLASH_Unlock();         //解锁写保护
  uint32_t offsetAddress=startAddress-FLASH_BASE;               //计算去掉0X08000000后的实际偏移地址
  uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;            //计算扇区地址，对于STM32F103VET6为0~255
  
  uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;    //对应扇区的首地址

  FLASH_ErasePage(sectorStartAddress);//擦除这个扇区
  
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
  {
    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
  }
  
  FLASH_Lock();//上锁写保护
}*/

























