#include "USART2.h"
#include "fifo.h"
#include "string.h"
#include "stdio.h"


#define FRAME_LEN 13
#define USART2_FIFO_LEN 64
#define complement(a) (FRAME_LEN-a)
u8 TxBuffer2[258];
u8 TxCounter2 = 0;
u8 count2 = 0; 
//unsigned char ctrl_buf[FRAME_LEN];
struct fifo fifo2;

void init_usart2_fifo(void)
{	
	signed int ret;

	fifo_alloc(&fifo2, USART2_FIFO_LEN);

	if(ret<0){
		printf("fifo2 alloc fail!\n");
	}
}

void Initial_UART2(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    

	USART_DeInit(USART2);   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART2, &USART_InitStructure); 
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);  
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE , ENABLE); 
	USART_ClearFlag(USART2, USART_FLAG_RXNE);
    
  USART_Cmd(USART2, ENABLE);
}

void usart2_send_data(u8 *sdata, u8 len) 
{	
	USART_ClearFlag(USART2,USART_FLAG_TC);
	for(u8 i=0;i<len;i++) {
		USART_SendData(USART2, sdata[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)!=SET);
	}
}
 void UART2_Put_Char(unsigned char DataToSend)
 {
     TxBuffer2[count2++] = DataToSend;  
     USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
 }
 void usart2SendBytes(u8 *buf, u8 len)
 {
     u8 *ptr;
     
     ptr = buf;
     while(len)
     {
         UART2_Put_Char(*ptr);
         ptr++;
         len--;
     }
 }
u8 send485_flag = 0;
u8 Rxd_buf[20];
u8 Rxd_len = 0;
 extern u8 send458_buf[9];
 extern u8 send458_ybuf[9];
void USART2_IRQHandler(void) 
{
//	unsigned int ret;
//	static u8 flag = 0;
//	static u8 i = 0;
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
			/* Write one byte to the transmit data register */
			USART_SendData(USART2, TxBuffer2[TxCounter2++]);                    

			/* Clear the USART2 transmit interrupt */
			USART_ClearITPendingBit(USART2, USART_IT_TXE);  

			if(TxCounter2 == count2)
			{
					/* Disable the USART2 Transmit interrupt */
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			}    
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE)!= RESET)
	  {	
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
			Rxd_buf[Rxd_len++] = USART_ReceiveData(USART2);
			
			if(Rxd_len <= 1)
				return;
			
			if(Rxd_buf[Rxd_len-1] == 0x22 && Rxd_buf[Rxd_len-2]==0x11)//ÅÐ¶ÏÖ¡Î²
			{
				Rxd_len = 0;
				if(Rxd_buf[0]==0x77 && Rxd_buf[1] ==0x55 && Rxd_buf[2] ==0x33 && Rxd_buf[3]==SLAVER_NUM)//ÅÐ¶ÏÊý¾Ý
				{
					send485_flag = 1;
					if(SLAVER_NUM == 5){
					//usart2SendBytes(send458_buf, 13);
					usart2SendBytes(send458_ybuf, 19);
					}
					else{
						usart2SendBytes(send458_buf, 13);
					}
				}
			}
		}
}

static u8 sum_check(u8 *buf, unsigned char len)
{
	u8 i,sum=0;

	for(i=0; i < len; i++) {
		sum += buf[i];
	}
	return sum;
}

void usart2_data_handler(void)
{
	unsigned int len;
	unsigned char buf[FRAME_LEN];
	static unsigned char copy_len = FRAME_LEN;

	while((fifo_used(&fifo2)) > ((copy_len))) {
		len = fifo_out(&fifo2, buf+complement(copy_len), (copy_len));
		if(len <1) {
			//      printf("usart1 fifo out err!\n");
		}
		/* cqx:find a frame */
		if(buf[0] == FRAME_HEAD1 && buf[1]==FRAME_HEAD2  && buf[sizeof(buf)-1] == FRAME_TAIL 
				&& (buf[sizeof(buf)-2] == sum_check(buf, FRAME_LEN-2))) {  
			copy_len=FRAME_LEN;
//			memcpy(ctrl_buf, buf, sizeof(buf));
			memset(buf, 0, sizeof(buf));
			continue;
		}
		/* cqx:not a frame */
		else {   
			for(copy_len=1;copy_len<FRAME_LEN;copy_len++) {
				if(buf[copy_len] == FRAME_HEAD1) {   /* find head,then cycle */
					memcpy(buf,buf+copy_len,(complement(copy_len)));
					break;
				}
			}
		}
	}
}

