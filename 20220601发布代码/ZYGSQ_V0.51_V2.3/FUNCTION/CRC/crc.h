#ifndef _CRC_H
#define _CRC_H

#include "stm32f10x.h"

#define 	CRC_16		1  //采用查表法还是计算法

#if	(CRC_16 == 1)//查表法
u16 Message_count_CRC_2(u8 Crc_Lenth, u8 *CRC_Data);
#endif
#if	(CRC_16 == 0)//计算法
u16 Message_count_CRC_2(u8 CRC_Lenth, u8 CRC_Data[]);
#endif

u8 crc8_maxim(u8 *data, u16 length);
#endif
