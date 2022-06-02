#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"

#define FRAME_HEAD1 0x67
#define FRAME_HEAD2 0x66
#define FRAME_TAIL  0x65
#define SLAVER_NUM   5

#define RE_485_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_3)
#define RE_485_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define DE_485_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define DE_485_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_4)
 
#define SLAVERTX    {RE_485_HIGH;DE_485_HIGH;}
                    
#define SLAVERRX    {RE_485_LOW;DE_485_LOW;}

extern void init_usart2_fifo(void); 
extern void usart2_data_handler(void);
extern void Initial_UART2(u32 baudrate);
extern void usart2_send_data(u8 *sdata, u8 len);
extern void usart2SendBytes(u8 *buf, u8 len);

#endif
