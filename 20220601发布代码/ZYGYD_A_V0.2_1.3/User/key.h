#ifndef _KEY_H
#define _KEY_H

#include "sys.h"
#include "stm32f10x.h"
	 
#define KEY0   PBin(8)
#define KEY1   PBin(9)
#define KEY2   PBin(10)
#define KEY3   PBin(11)
#define KEY4   PBin(12)  
#define KEY5   PBin(13)  
#define KEY6   PBin(14)  
	 
extern void init_key(void);
extern void init_backKey(void);
extern u8 key_scan(void);

#endif
