/*******************************************************************************
* @File       : flash.h
* @Author   : cqx
* @Version  : V0.0.0
* @Date       : 20-February-2016
* @Brief      : This file provides all the flash functions.
********************************************************************************
* @Attention:
* Non
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash.h"

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE 2048
#endif

u16 Flash_buf[STM_SECTOR_SIZE/2];//max 2k


/*******************************************************************************
* Function Name : u16 flash_read_halfword(u32 faddr)
* Description   : Non
* Input         : Non
* Output        : Non
* Return        : Non
* atention      : Non
*****************/
u16 flash_read_halfword(u32 faddr)
{
    return *(vu16*)faddr; 
}

/*******************************************************************************
* Function Name : flash_read(u32 readaddr,u16 *pbuf,u16 length)
* Description   : Non
* Input         : Non
* Output        : Non
* Return        : Non
* atention      : Non
*****************/
void flash_read(u32 readaddr, u16 *pbuf, u16 length)    
{
    u16 i;
    for(i=0;i<length;i++)
    {
        pbuf[i]=flash_read_halfword(readaddr);//read two bytes
        readaddr+=2;//address offset two bytes
    }
}

/*******************************************************************************
* Function Name : flash_write_nocheck(u32 writeaddr,u16 *pbuf,u16 length)
* Description   : Non
* Input         : Non
* Output        : Non
* Return        : Non
* atention      : Non
*****************/
void flash_write_nocheck(u32 writeaddr, u16 *pbuf, u16 length) 
{                    
    u16 i;
    for(i=0;i<length;i++)
    {
        FLASH_ProgramHalfWord(writeaddr,pbuf[i]);
        writeaddr+=2;//address add 2
    }  
}

/*******************************************************************************
* Function Name : flash_write(u32 writeaddr, u16 *pbuf, u16 length) 
* Description   : Non
* Input         : Non
* Output        : Non
* Return        : Non
* atention      : Non
*****************/
void flash_write(u32 writeaddr, u16 *pbuf, u16 length)  
{
    u32 secpos;    //sector position
    u16 secoff;    //sector offset address
    u16 secremain; //sector remained address
    u16 i;    
    u32 offaddr;   //去掉0X08000000后的地址
    if(writeaddr < STM32_FLASH_BASE || (writeaddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))//check the address
    return;//illegal address
    
    FLASH_Unlock();
    offaddr = writeaddr - STM32_FLASH_BASE;//actually offset偏移地址
    secpos = offaddr / STM_SECTOR_SIZE;//sector address 0~127 for STM32F103CBT6占用几个扇区
    secoff = (offaddr % STM_SECTOR_SIZE) / 2;       //在扇区内的偏移(2个字节为基本单位.)
    secremain = STM_SECTOR_SIZE / 2 - secoff;       //sector remained capacity扇区剩余容量
    if(length <= secremain)
        secremain = length;//不大于该扇区范围
    while(1) 
    {   
        flash_read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, Flash_buf, STM_SECTOR_SIZE / 2);//read the whole sector
        /******判断剩余扇区是否需要擦除*******/
        for(i = 0; i < secremain; i++)//check data 
        {
            if(Flash_buf[secoff + i] != 0XFFFF)
                break;//need to erase      
        }
        if(i < secremain)//need to erase  
        {
            FLASH_ErasePage(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE);//erase the sector
            for(i = 0; i < secremain; i++)//copy
            {
                Flash_buf[i + secoff] = pbuf[i];      
            }
            flash_write_nocheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, Flash_buf, STM_SECTOR_SIZE / 2);//写入整个扇区  
        }
        else 
            flash_write_nocheck(writeaddr, pbuf, secremain);//写已经擦除了的,直接写入扇区剩余区间. 
        /*************************************/
        if(length == secremain)
            break;//write ended
        else//write don't ended
        {
            secpos++;               //sector add
            secoff = 0;                    
            pbuf += secremain;      //pointer offset
            writeaddr += secremain;   //write address offset   
            length -= secremain;    //byte sub(16bits)
            if(length > (STM_SECTOR_SIZE / 2))
            {
                secremain = STM_SECTOR_SIZE / 2;//next sector still don't write over
            }
            else 
                secremain = length;//next sector still write over
        }    
    };  
    FLASH_Lock();
}




