#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"

#define FRAME_LEN    10
#define FRAME_HEAD1  0x77
#define FRAME_TAIL   0x55

void Initial_UART3(u32 baudrate);
void UART3_Put_Char(unsigned char DataToSend);
void usart3SendBytes(u8 *buf, u8 len);
void init_usart3_fifo(void);
void usart3_data_handler(void);

extern u8 USART3_RxBUF[54];
extern u8 USART3_Rxcomplete;

#endif
