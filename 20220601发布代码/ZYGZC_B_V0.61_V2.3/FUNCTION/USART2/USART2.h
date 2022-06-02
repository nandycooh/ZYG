#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"

#define SLAVER_NUM   5

#define RE_485_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_3)
#define RE_485_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define DE_485_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define DE_485_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_4)
 
#define SLAVERTX    {RE_485_HIGH;DE_485_HIGH;}
                    
#define SLAVERRX    {RE_485_LOW;DE_485_LOW;}



typedef struct{
		uint8_t belt[4];
		uint8_t data[6];
		uint8_t back;
}R485_t;



void Initial_UART2(u32 baudrate);


#endif
