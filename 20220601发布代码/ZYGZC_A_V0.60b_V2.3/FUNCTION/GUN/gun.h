#ifndef __GUN_H
#define __GUN_H	

#include "sys.h"
#include "stm32f10x.h"
#include "attitude.h"
	
extern  u8 gamePadKeyData[];
//extern HandgunDataDef *hanfbuf;
u8* APL_gamePadKeyGet(void);

#endif
