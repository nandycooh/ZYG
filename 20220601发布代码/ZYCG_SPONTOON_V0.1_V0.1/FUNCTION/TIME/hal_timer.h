#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include "sys.h"


/*控制标志位*/
typedef struct
{
	bool is_UpDateLED;//数据跟新标志位
	bool is_ConnectTerminal;//连接标志位
	bool is_ConnectChargingInterface;//连接充电器标志位
	bool is_LowBattery;//低电量标志位
//	bool is_WaitingStartUp;//睡眠标志位
	bool is_RfClearReceiveBuffer;//清空BUFFER
	bool is_TurnOffFlag;
}Event_Flag_t;

//extern Event_Flag_t p_Flag;

/*定时器操作API
*/
typedef struct
{
	void (*start)(u8 timerEx_id,uint32_t timer_period);
	void (*stop)(u8 timerEx_id);
}TIME_ControlDef;

void timer_start(u8 timerEx_id,uint32_t timer_period);
void TIM3_Int_Init(u16 arr,u16 psc);
void ApplicationTimer_Stop(void);

#endif
