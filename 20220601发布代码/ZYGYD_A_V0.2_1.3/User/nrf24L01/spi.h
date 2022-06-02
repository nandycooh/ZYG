
#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

extern void SPI1_Init(void);		
extern void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
extern uint8_t SPI1_ReadWriteByte(uint8_t TxData);
		     
#endif
