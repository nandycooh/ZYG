#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include "sys.h"

/*¶¨Ê±Æ÷²Ù×÷API
*/
typedef struct
{
	void (*start)(u8 timerEx_id,uint32_t timer_period);
	void (*stop)(u8 timerEx_id);
}TIME_ControlDef;

void timer_start(u8 timerEx_id,uint32_t timer_period);
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
