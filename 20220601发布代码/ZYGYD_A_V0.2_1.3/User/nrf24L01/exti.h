#ifndef __EXTI_H
#define __EXTI_H	

#include "stm32f10x_it.h"
#include "nrf24L01p.h"

extern uint8_t IrqDynRxFlag;
extern uint8_t IrqDynTxFlag;


extern void EXTI2_Init(void);

#endif
