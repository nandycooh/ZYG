/******************************************************************************

                  版权所有 (C), 重庆子元科技有限公司

 ******************************************************************************
  文 件 名   : USART3.c
  版 本 号   : 初稿
  作    者   : song_mk
  生成日期   : 2017年12月28日
  最近修改   :
  功能描述   : 串口2驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2017年12月28日
    作    者   : song_mk
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "USART3.h"
#include "fifo.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define USART3_FIFO_LEN 32
#define complement(a) (FRAME_LEN-a)
/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
u8 TxBuffer3[258];
u8 TxCounter3 = 0;
u8 count3 = 0; 
u8 USART3_RxBUF[54];
u8 USART3_Rxcomplete;

struct fifo fifo1;
static unsigned char copy_len = FRAME_LEN;
u8 buf123[USART3_FIFO_LEN];
u8 USART3_fifo[FRAME_LEN];
/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

 void NVIC_UART3_Config(void)
 {
     NVIC_InitTypeDef NVIC_InitStructure; 
 
       /* Enable the USART3 Interrupt */
     NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
 }



 void Initial_UART3(u32 baudrate)
 {
     USART_InitTypeDef USART_InitStructure;
     GPIO_InitTypeDef GPIO_InitStructure;
     
     // 打开串口GPIO的时钟
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); 
          
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOC, &GPIO_InitStructure);    
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
     GPIO_Init(GPIOC, &GPIO_InitStructure);
     GPIO_SetBits(GPIOC, GPIO_Pin_11);
     
     USART_DeInit(USART3);
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
     GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
     
     USART_InitStructure.USART_BaudRate = baudrate;
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;
     USART_InitStructure.USART_StopBits = USART_StopBits_1;
     USART_InitStructure.USART_Parity = USART_Parity_No ;
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
     USART_Init(USART3, &USART_InitStructure); 
     USART_ITConfig(USART3, USART_IT_TXE, DISABLE);  
     USART_ClearFlag(USART3, USART_FLAG_TC);
     USART_ITConfig(USART3, USART_IT_RXNE , ENABLE); 
     USART_ClearFlag(USART3, USART_FLAG_RXNE);
     
     USART_Cmd(USART3, ENABLE);   
     NVIC_UART3_Config();
 }

 void USART3_IRQHandler(void)
 {
    unsigned char buf;
    static unsigned char NUM = 0;
    static unsigned char RxCnt = 0;
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {   
        /* Write one byte to the transmit data register */
        USART_SendData(USART3, TxBuffer3[TxCounter3++]);                    

        /* Clear the USART3 transmit interrupt */
        USART_ClearITPendingBit(USART3, USART_IT_TXE);  

        if(TxCounter3 == count3)
        {
            /* Disable the USART3 Transmit interrupt */
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }    
    }
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        buf = USART_ReceiveData(USART3);

        if(NUM == 0)
        {
            if(buf == 0x99)
            {
                NUM = 1;
                USART3_RxBUF[RxCnt++] = buf;
            }
            else
            {
                NUM = 0;
                RxCnt = 0;
            }
        }
        else if(NUM == 1)
        {
            if(buf == 0x33)
            {
                NUM = 2;
                USART3_RxBUF[RxCnt++] = buf;
            }
            else 
            {
                NUM = 0;
                RxCnt = 0;
            }
        }
        else if(NUM == 2)
        {
            USART3_RxBUF[RxCnt++] = buf;
            if(RxCnt == USART3_RxBUF[8] + 12)
            {
                NUM = 0;
                RxCnt = 0;
                USART3_Rxcomplete = 1;
            }
        }

        else 
            NUM = 0;

        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

        /* Clear the USART3 transmit interrupt */
    }

 }

 void UART3_Put_Char(unsigned char DataToSend)
 {
     TxBuffer3[count3++] = DataToSend;  
     USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  
 }
 void usart3SendBytes(u8 *buf, u8 len)
 {
     u8 *ptr;
     
     ptr = buf;
     while(len)
     {
         UART3_Put_Char(*ptr);
         ptr++;
         len--;
     }
 }


 void init_usart3_fifo(void)
{   
//  signed int ret;
    
//  fifo_alloc(&fifo1, USART1_FIFO_LEN);
    
    fifo_init(&fifo1, buf123, USART3_FIFO_LEN);
    
//  if(ret<0){
////        printf("fifo1 alloc fail!\n");
//  }
}
 u8 sum_check(u8 *buf)
 {
     u8 i,sum=0;
     
     for(i=0; i < 8; i++)
     {
         sum += buf[i];
     }
     return sum;
 }
 
 void usart3_data_handler(void)
 {
     unsigned int len;
     unsigned char buf[FRAME_LEN];
 
    while((fifo_used(&fifo1))>=((copy_len)))
    {
        len = fifo_out(&fifo1, buf+complement(copy_len), (copy_len));
        if(len <1)
        {
        
    //      printf("usart1 fifo out err!\n");
        }
        if(buf[0] == FRAME_HEAD1 && buf[sizeof(buf)-1] == FRAME_TAIL)
//        if(buf[0] == FRAME_HEAD1 && buf[sizeof(buf)-1] == FRAME_TAIL && (buf[sizeof(buf)-2] == sum_check(buf)))
        {  /* cqx:find a frame */
            copy_len=FRAME_LEN;
            memcpy(USART3_fifo,buf,sizeof(buf));
            memset(buf,0,sizeof(buf));
            continue;
        }
        else
        {   /* cqx:not a frame */
            for(copy_len=1;copy_len<FRAME_LEN;copy_len++)
            {
                if(buf[copy_len] == FRAME_HEAD1)
                {   /* find head,then cycle */
                    memcpy(buf,buf+copy_len,(complement(copy_len)));
                    break;
                }
            }
        }
    }
 }

