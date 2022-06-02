/******************************************************************************
 * @File 	  	: crc.c
 * @Author		: 
 * @Version		: V0.0.0
 * @Date 	  	: 02-February-2016
 * @Brief	  	: This file provides all the CRC firmware functions.
 ******************************************************************************
 * @Attention:
 * This file include crc8 check.
 *
 ******************************************************************************/
/* Includes -----------------------------------------------------------------*/
#include "crc.h"

/****************************************************************************** 
 * Name:    CRC-8/MAXIM         x8+x5+x4+1 
 * Poly:    0x31 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Alias:   DOW-CRC,CRC-8/IBUTTON 
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20 
 *****************************************************************************/  
u8 crc8_maxim(u8 *data, u16 length)  
{  
	uint8_t i;  
	uint8_t crc = 0;   

	while(length--){  
		crc ^= *data++;  
		for (i = 0; i < 8; i++){  
			if (crc & 1)  
				crc = (crc >> 1) ^ 0x8C;  
			else  
				crc >>= 1;  
		}  
	}  
	return crc;  
} 
