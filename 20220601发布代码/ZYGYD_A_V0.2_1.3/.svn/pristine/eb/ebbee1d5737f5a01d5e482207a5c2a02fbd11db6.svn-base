/*******************************************************************************
* @File 	  : usart.h
* @Author   : cqx
* @Version  : V0.0.0
* @Date 	  : 01-March-2016
* @Brief	  : This file provides all the flash functions.
********************************************************************************
* @Attention:
* Non
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _FLASH_H
#define _FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_flash.h"
#define FlashDataAddr 0x08018400

	 
#define STM32_FLASH_SIZE 128//flash capacity 
#define STM32_FLASH_BASE 0x08000000//flash start address
#define BOOTLOADER_ADDR  0x08000000  //bootloader start address

u16 flash_read_halfword(u32 faddr);
void flash_read(u32 readaddr,u16 *pbuffer,u16 length);
void flash_write_nocheck(u32 writeaddr,u16 *pbuffer,u16 length);
void flash_write(u32 writeaddr, u16 *pbuffer, u16 length);
void flash_write_data(u32 writeaddr, u16 *pbuf, u16 length);
void init_flash_open_data(void);

#ifdef __cplusplus
}
#endif

#endif
