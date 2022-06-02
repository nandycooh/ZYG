#include "USART2.h"
#include "string.h"
#include "stdio.h"
#include "../function/crc/crc.h"

#define FRAME_LEN 13
#define USART2_FIFO_LEN 64
#define complement(a) (FRAME_LEN-a)




R485_t Recv_485_Msg;

u8 TxBuffer2[258];
u8 TxCounter2 = 0;
u8 count2 = 0; 
//unsigned char ctrl_buf[FRAME_LEN];


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
u8 Rxd_buf[20];
u8 Rxd_len = 0;

void USART2_IRQHandler(void) 
{
	u16 crc16 =0;
//	u8 restrt;
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

		Rxd_buf[Rxd_len++]=USART_ReceiveData(USART2);

		crc16 = Message_count_CRC_2(15, Rxd_buf);
		if(Rxd_buf[15] != (crc16&0xFF) && Rxd_buf[16] != ((crc16&0xFF00)>>8))
			return;
		
		if(Rxd_buf[Rxd_len-1] == 0x22 && Rxd_buf[Rxd_len-2]==0x11)
		{
			Rxd_len = 0;
			if(Rxd_buf[0]==0x66 && Rxd_buf[1]==SLAVER_NUM)
			{
				Recv_485_Msg.belt[0]=Rxd_buf[3];
				Recv_485_Msg.belt[1]=Rxd_buf[4];
				Recv_485_Msg.belt[2]=Rxd_buf[5];
				Recv_485_Msg.belt[3]=Rxd_buf[6];
				
				Recv_485_Msg.data[0]=Rxd_buf[8];
				Recv_485_Msg.data[1]=Rxd_buf[9];
				Recv_485_Msg.data[2]=Rxd_buf[10];
				Recv_485_Msg.data[3]=Rxd_buf[11];
				Recv_485_Msg.data[4]=Rxd_buf[12];
				Recv_485_Msg.data[5]=Rxd_buf[13];
				
				Recv_485_Msg.back=Rxd_buf[14];
			}
		}
	}
}

//static u8 rcv_byte = 0;
//static u8 rcv_byte_last = 0;
//static int rx_num = 0;
//u8 rx_buf[20];//串口2接受buff
//void USART2_IRQHandler(void)//210417
//{
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//		
//		rcv_byte_last = rcv_byte;//上一次接收字节
//		rcv_byte =USART_ReceiveData(USART2);	//读取接收到的数据
//		
//		rx_buf[rx_num++] = rcv_byte;
//		USART_SendData(USART3,rcv_byte);
//		if(rcv_byte_last==0x66 && rcv_byte==SLAVER_NUM)//接收上位机下发协议
//    {
//      rx_buf[0] = rcv_byte_last;
//      rx_buf[1] = rcv_byte;
//      rx_num    = 2;
//    }

//		if(rx_num >= 20 || (rx_buf[rx_num-2] == 0x11 && rx_buf[rx_num-1] == 0x22)) //上位机下发帧长为12字节
//    {       
//			rcv_byte=0;
//			rx_num = 0;
//    }			
//	}
//}

//static u8 sum_check(u8 *buf, unsigned char len)
//{
//	u8 i,sum=0;

//	for(i=0; i < len; i++) {
//		sum += buf[i];
//	}
//	return sum;
//}

