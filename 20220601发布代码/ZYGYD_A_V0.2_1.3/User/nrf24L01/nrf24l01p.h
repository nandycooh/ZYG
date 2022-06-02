
#ifndef __NRF24L01P_H
#define __NRF24L01P_H

#include "stm32f10x.h"


/***********NRF24L01P指令***********/
#define RF_R_REGISTER          0x00  //读命令/状态寄存器    
#define RF_W_REGISTER          0x20  //写命令/状态寄存器    
#define RF_R_RX_PAYLOAD        0x61  //读RX载荷             
#define RF_W_TX_PAYLOAD        0xA0  //写TX载荷             
#define RF_FLUSH_TX            0xE1  //清除TX FIFO         
#define RF_FLUSH_RX            0xE2  //清除RX FIFO          
#define RF_REUSE_TX_PL         0xE3  //重新使用上一个TX载荷 
#define RF_R_RX_PL_WID         0x60  //读RX载荷宽度

#define RF_W_ACK_PAYLOAD_P0    0xA8  //数据通道0 ACK
#define RF_W_ACK_PAYLOAD_P1    0xA9  //数据通道1 ACK
#define RF_W_ACK_PAYLOAD_P2    0xAA  //数据通道2 ACK
#define RF_W_ACK_PAYLOAD_P3    0xAB  //数据通道3 ACK
#define RF_W_ACK_PAYLOAD_P4    0xAC  //数据通道4 ACK
#define RF_W_ACK_PAYLOAD_P5    0xAD  //数据通道5 ACK

#define RF_W_TX_PAYLOAD_NOACK  0xB0  //禁用ACK
#define RF_NOP                 0xFF  //空操作               

/**********NRF24L01P寄存器**********/
#define RF_CONFIG       0x00  //配置寄存器                
#define RF_EN_AA        0x01  //使能自动应答
#define RF_EN_RXADDR    0x02  //使能数据通道         
#define RF_SETUP_AW     0x03  //设置地址宽度
#define RF_SETUP_RETR   0x04  //配置自动重发             
#define RF_RF_CH        0x05  //RF通道                        
#define RF_RF_SETUP     0x06  //RF配置寄存器                    
#define RF_STATUS       0x07  //状态寄存器                   

#define RF_RX_OK        0x40  //接收数据中断
#define RF_TX_OK        0x20  //数据发送完成中断
#define RF_MAX_TX       0x10  //达到最多次重发中断

#define RF_OBSERVE_TX   0x08  //发送检测寄存器       
#define RF_RPD          0x09  //载波检测寄存器         
#define RF_RX_ADDR_P0   0x0A  //数据通道0接收地址 
#define RF_RX_ADDR_P1   0x0B  //数据通道1接收地址 
#define RF_RX_ADDR_P2   0x0C  //数据通道2接收地址 
#define RF_RX_ADDR_P3   0x0D  //数据通道3接收地址 
#define RF_RX_ADDR_P4   0x0E  //数据通道4接收地址  
#define RF_RX_ADDR_P5   0x0F  //数据通道5接收地址 
#define RF_TX_ADDR      0x10  //发送地址           
#define RF_RX_PW_P0     0x11  //接收数据通道0有效数据宽度
#define RF_RX_PW_P1     0x12  //接收数据通道1有效数据宽度
#define RF_RX_PW_P2     0x13  //接收数据通道2有效数据宽度
#define RF_RX_PW_P3     0x14  //接收数据通道3有效数据宽度
#define RF_RX_PW_P4     0x15  //接收数据通道4有效数据宽度
#define RF_RX_PW_P5     0x16  //接收数据通道5有效数据宽度
#define RF_FIFO_STATUS  0x17  //FIFO状态寄存器              
#define RF_DYNPD        0x1C  //使能/禁止动态载荷                       
#define RF_FEATURE      0x1D  //功能寄存器

#define rfRxAddrWidM 5 
#define rfTxAddrWidM 5 

#define RF_csnEn()  GPIO_ResetBits(GPIOB, GPIO_Pin_0) 
#define RF_csnDis() GPIO_SetBits(GPIOB, GPIO_Pin_0)          //CSN
#define RF_ceEn()   GPIO_ResetBits(GPIOB, GPIO_Pin_1) 
#define RF_ceDis()  GPIO_SetBits(GPIOB, GPIO_Pin_1)          //CE
#define RF_getIrq() GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) //IRQ   

extern u8 rfRxChG;
extern u8 rfTxChG;

extern u8 rfIrqRxFlagG;
extern u8 rfIrqTxFlagG;

extern u8 rfRxBufCh0[32];  extern u8 rfRxBufLenCh0;
extern u8 rfRxBufCh1[32];  extern u8 rfRxBufLenCh1;
extern u8 rfRxBufCh2[32];  extern u8 rfRxBufLenCh2;
extern u8 rfRxBufCh3[32];  extern u8 rfRxBufLenCh3;
extern u8 rfRxBufCh4[32];  extern u8 rfRxBufLenCh4;
extern u8 rfRxBufCh5[32];  extern u8 rfRxBufLenCh5;

extern u8 rfTxBufCh0[32];  extern u8 rfTxBufLenCh0;
extern u8 rfTxBufCh1[32];  extern u8 rfTxBufLenCh1;
extern u8 rfTxBufCh2[32];  extern u8 rfTxBufLenCh2;
extern u8 rfTxBufCh3[32];  extern u8 rfTxBufLenCh3;
extern u8 rfTxBufCh4[32];  extern u8 rfTxBufLenCh4;
extern u8 rfTxBufCh5[32];  extern u8 rfTxBufLenCh5;
                                                 
extern u8   RF_checkF(void);  
extern void RF_confF(u8 ch); 
extern void RF_initF(u8 ch);  

extern u8   RF_readRegF(u8 readAddr); 
extern void RF_writeRegF(u8 writeAddr, u8 writeData); 
extern void RF_readRegsF(u8 readAddr, u8 *readData, u8 len);   
extern void RF_writeRegsF(u8 writeAddr, u8 *writeData, u8 len);

extern u8   RF_irqRxTxAckF(u8 ch);

extern u8   rfRxFlat;


#endif
