#ifndef __TIMER2_H
#define __TIMER2_H

#include "stm32f10x_it.h"
#include "./nrf24L01/spi.h"
#include "./nrf24L01/nrf24l01p.h"

extern u32 time_flag;

extern void TIM2_IntInit(uint16_t arr,uint16_t psc);

#endif
