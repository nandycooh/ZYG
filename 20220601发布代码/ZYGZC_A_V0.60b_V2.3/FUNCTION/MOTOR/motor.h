#ifndef __MOTOR_H
#define __MOTOR_H
#include "stdio.h"	
#include "sys.h" 

//串口硬件定义 210202
#define UART3_PORT					USART3
#define UART3_PORT_CLK				RCC_APB2Periph_USART3
#define UART3_PORT_AF				GPIO_Remap_USART3
	
//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(u32 bound);

void uart3_tx_bytes( uint8_t* TxBuffer, uint8_t Length );
uint8_t uart3_rx_bytes( uint8_t* RxBuffer,USART_TypeDef *USARTX);

#endif


