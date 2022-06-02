/******************************************************************************

                  版权所有 (C), 重庆子元科技有限公司

 ******************************************************************************
  文 件 名   : nrf24l01p.c
  版 本 号   : v1.0
  作    者   : ljk
  生成日期   : 2017年3月2日
  最近修改   :
  功能描述   : nrf24l01p
  函数列表   :
              u8   RF_checkF(void);  
              void RF_confF(u8 ch); 
              void RF_initF(u8 ch);  

              u8   RF_readRegF(u8 readAddr); 
              void RF_writeRegF(u8 writeAddr, u8 writeData); 
              void RF_readRegsF(u8 readAddr, u8 *readData, u8 len);   
              void RF_writeRegsF(u8 writeAddr, u8 *writeData, u8 len);

              u8   RF_irqRxTxAckF(u8 ch);
  修改历史   :
  日    期   : 2017年3月2日
  作    者   : ljk
  修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "nrf24l01p.h"
#include "spi.h"
#include "string.h"
#include "./systick/bsp_SysTick.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
u8 rfRxChG;
u8 rfTxChG;

u8 rfIrqRxFlagG;
u8 rfIrqTxFlagG;
 
u8 rfRxBufCh0[32];  u8 rfRxBufLenCh0 = 32;
u8 rfRxBufCh1[32];  u8 rfRxBufLenCh1 = 32;
u8 rfRxBufCh2[32];  u8 rfRxBufLenCh2 = 32;
u8 rfRxBufCh3[32];  u8 rfRxBufLenCh3 = 32;
u8 rfRxBufCh4[32];  u8 rfRxBufLenCh4 = 32;
u8 rfRxBufCh5[32];  u8 rfRxBufLenCh5 = 32;

u8 rfTxBufCh0[32];  u8 rfTxBufLenCh0 = 32;
u8 rfTxBufCh1[32];  u8 rfTxBufLenCh1 = 32;
u8 rfTxBufCh2[32];  u8 rfTxBufLenCh2 = 32;
u8 rfTxBufCh3[32];  u8 rfTxBufLenCh3 = 32;
u8 rfTxBufCh4[32];  u8 rfTxBufLenCh4 = 32;
u8 rfTxBufCh5[32];  u8 rfTxBufLenCh5 = 32;



/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
u8   RF_checkF(void);  
void RF_confF(u8 ch); 
void RF_initF(u8 ch);  

u8   RF_readRegF(u8 readAddr); 
void RF_writeRegF(u8 writeAddr, u8 writeData); 
void RF_readRegsF(u8 readAddr, u8 *readData, u8 len);   
void RF_writeRegsF(u8 writeAddr, u8 *writeData, u8 len);

u8   RF_irqRxTxAckF(u8 ch);
u8   rfRxFlat;
/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
u8 rfRxAddrCh0[rfRxAddrWidM] = {0x00, 0x44, 0x33, 0x22, 0x11};  
u8 rfTxAddrCh0[rfTxAddrWidM] = {0x00, 0x44, 0x33, 0x22, 0x11};  
u8 rfRxAddrCh1[rfRxAddrWidM] = {0x02, 0x44, 0x33, 0x22, 0x11}; 
u8 rfTxAddrCh1[rfTxAddrWidM] = {0x02, 0x44, 0x33, 0x22, 0x11};  
u8 rfRxAddrCh2[rfRxAddrWidM] = {0x05, 0x44, 0x33, 0x22, 0x11};
u8 rfTxAddrCh2[rfTxAddrWidM] = {0x05, 0x44, 0x33, 0x22, 0x11};  
u8 rfRxAddrCh3[rfRxAddrWidM] = {0x07, 0x44, 0x33, 0x22, 0x11}; 
u8 rfTxAddrCh3[rfTxAddrWidM] = {0x07, 0x44, 0x33, 0x22, 0x11}; 
u8 rfRxAddrCh4[rfRxAddrWidM] = {0x09, 0x44, 0x33, 0x22, 0x11}; 
u8 rfTxAddrCh4[rfTxAddrWidM] = {0x09, 0x44, 0x33, 0x22, 0x11}; 
u8 rfRxAddrCh5[rfRxAddrWidM] = {0x0b, 0x44, 0x33, 0x22, 0x11};
u8 rfTxAddrCh5[rfTxAddrWidM] = {0x0b, 0x44, 0x33, 0x22, 0x11};

u8 rfRxAddrPn[6]      = {RF_RX_ADDR_P0, RF_RX_ADDR_P1, RF_RX_ADDR_P2, RF_RX_ADDR_P3, RF_RX_ADDR_P4, RF_RX_ADDR_P5};
u8 rfEnPn[6]          = {0x01, 0x03, 0x05, 0x09, 0x11, 0x21};
u8 rfWriteAckPayPn[6] = {RF_W_ACK_PAYLOAD_P0, RF_W_ACK_PAYLOAD_P1, RF_W_ACK_PAYLOAD_P2, RF_W_ACK_PAYLOAD_P3, RF_W_ACK_PAYLOAD_P4, RF_W_ACK_PAYLOAD_P5};

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*****************************************************************************
 函 数 名  : u8 RF_checkF(void) 
 功能描述  : 通讯检查
 输入参数  : 
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
u8 RF_checkF(void)  
{
    u8 i;
    u8 buf[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    RF_writeRegsF(RF_W_REGISTER+RF_TX_ADDR, buf, 5);
    for(i = 0; i < 5; i++)
        buf[i] = 0;
    RF_readRegsF(RF_TX_ADDR, buf, 5); 
    for(i = 0; i < 5; i++)
        if(buf[i] != 0xA5)
            break;                                 
    if(i != 5)
        return ERROR;
    return SUCCESS; 
}   
/*****************************************************************************
 函 数 名  : void RF_confF(u8 ch) 
 功能描述  : 芯片配置
 输入参数  : u8 ch  
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
void RF_confF(u8 ch) 
{
    u8 sta;                                     
    sta = RF_readRegF(RF_STATUS);     
    RF_writeRegF(RF_W_REGISTER + RF_STATUS, sta);  
    RF_writeRegF(RF_FLUSH_TX, 0xFF); 
    RF_writeRegF(RF_FLUSH_RX, 0xFF); 
    
    RF_ceEn();  
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[0], rfRxAddrCh0, rfRxAddrWidM);
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[1], rfRxAddrCh1, rfRxAddrWidM);
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[2], rfRxAddrCh2, 1);
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[3], rfRxAddrCh3, 1);
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[4], rfRxAddrCh4, 1);
    RF_writeRegsF(RF_W_REGISTER + rfRxAddrPn[5], rfRxAddrCh5, 1);
    RF_writeRegF(RF_W_REGISTER + RF_EN_AA, rfEnPn[ch]);     
    RF_writeRegF(RF_W_REGISTER + RF_EN_RXADDR, rfEnPn[ch]);  
    RF_writeRegF(RF_W_REGISTER + RF_SETUP_RETR, 0x20); //待修改
    RF_writeRegF(RF_W_REGISTER + RF_RF_CH, 40);       //待修改
    RF_writeRegF(RF_W_REGISTER + RF_RF_SETUP, 0x0E);  
    RF_writeRegF(RF_W_REGISTER + RF_DYNPD, rfEnPn[ch]);      
    RF_writeRegF(RF_W_REGISTER + RF_FEATURE, 0x06);
    RF_writeRegF(RF_W_REGISTER + RF_CONFIG, 0x3B);  
    RF_ceDis();
}
/*****************************************************************************
 函 数 名  : void RF_initF(u8 ch)  
 功能描述  : 射频初始化
 输入参数  : u8 ch  
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
void RF_initF(u8 ch)  
{   
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_2); 

    mdelay(500);
    while(!RF_checkF())
    {
        mdelay(1000);
        mdelay(1000);
    }
    RF_confF(ch);
}

/*****************************************************************************
 函 数 名  : u8 RF_readRegF(u8 readAddr) 
 功能描述  : 读片寄存器
 输入参数  : u8 readAddr  
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
u8 RF_readRegF(u8 readAddr)   
{
    u8 readData;    
    RF_csnEn();
    SPI1_ReadWriteByte(readAddr);
    readData = SPI1_ReadWriteByte(0xFF);
    RF_csnDis();
    return readData;      
}   
/*****************************************************************************
 函 数 名  : void RF_writeRegF(u8 writeAddr, u8 writeData)
 功能描述  : 写芯片寄存器
 输入参数  : u8 writeAddr  
             u8 writeData  
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
void RF_writeRegF(u8 writeAddr, u8 writeData)  
{
    RF_csnEn();
    SPI1_ReadWriteByte(writeAddr);
    SPI1_ReadWriteByte(writeData);
    RF_csnDis();                
}
/*****************************************************************************
 函 数 名  : void RF_readRegsF(u8 readAddr, u8 *readData, u8 len)  
 功能描述  : 读芯片寄存器
 输入参数  : u8 readAddr   
             u8 *readData  
             u8 len        
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
void RF_readRegsF(u8 readAddr, u8 *readData, u8 len)  
{
    u8 byte_ctr;    
    RF_csnEn();
    SPI1_ReadWriteByte(readAddr);
    for(byte_ctr = 0; byte_ctr < len; byte_ctr++)
        readData[byte_ctr] = SPI1_ReadWriteByte(0xFF);
    RF_csnDis();  
}
/*****************************************************************************
 函 数 名  : void RF_writeRegsF(u8 writeAddr, u8 *writeData, u8 len) 
 功能描述  : 写芯片寄存器
 输入参数  : u8 writeAddr   
             u8 *writeData  
             u8 len         
 输出参数  : 
 返 回 值  : 
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
void RF_writeRegsF(u8 writeAddr, u8 *writeData, u8 len) 
{
    u8 byte_ctr;    
    RF_csnEn();
    SPI1_ReadWriteByte(writeAddr);
    for(byte_ctr = 0; byte_ctr < len; byte_ctr++)
        SPI1_ReadWriteByte(writeData[byte_ctr]); 
    RF_csnDis();  
}

/*****************************************************************************
 函 数 名  : u8 RF_irqRxTxAckF(u8 ch)
 功能描述  : 中断接收数据，并发送ack
 输入参数  : u8 ch  
 输出参数  : 
 返 回 值  : RF_TX_OK
 日    期  : 2017年3月2日
 作    者  : ljk

*****************************************************************************/
u8 RF_irqRxTxAckF(u8 ch)
{
    u8 sta; 
    sta = RF_readRegF(RF_STATUS);  
    RF_writeRegF(RF_W_REGISTER + RF_STATUS, sta);
    if(sta & RF_RX_OK)
    {
        switch (ch)
        {
            case 0:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[0], rfTxBufCh0, rfTxBufLenCh0); 
                    RF_ceDis();
                    rfRxBufLenCh0 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh0, rfRxBufLenCh0); 
                    break;
            case 1:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[1], rfTxBufCh1, rfTxBufLenCh1); 
                    RF_ceDis();
                    rfRxBufLenCh1 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh1, rfRxBufLenCh1); 
                    break;
            case 2:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[2], rfTxBufCh2, rfTxBufLenCh2); 
                    RF_ceDis();
                    rfRxBufLenCh2 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh2, rfRxBufLenCh2); 
                    break;
            case 3:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[3], rfTxBufCh3, rfTxBufLenCh3); 
                    RF_ceDis();
                    rfRxBufLenCh3 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh3, rfRxBufLenCh3);
            
//                    if(rfTxBufCh3[0] == 0x82)
//                        usartSendBytes(rfTxBufCh3, 10);
//                    rfTxBufCh3[0] = 0;
                    break;
            case 4:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[4], rfTxBufCh4, rfTxBufLenCh4); 
                    RF_ceDis();
                    rfRxBufLenCh4 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh4, rfRxBufLenCh4); 
                    break;
            case 5:
                    RF_ceEn();
                    RF_writeRegsF(rfWriteAckPayPn[5], rfTxBufCh5, rfTxBufLenCh5); 
                    RF_ceDis();
                    rfRxBufLenCh5 = RF_readRegF(RF_R_RX_PL_WID);
                    RF_readRegsF(RF_R_RX_PAYLOAD, rfRxBufCh5, rfRxBufLenCh5); 
                    break;      
        }
        sta = RF_readRegF(RF_STATUS);     
        RF_writeRegF(RF_W_REGISTER + RF_STATUS, sta);
        sta = sta & 0x0F;
        if(sta == 0x0E)  
        { 
            return RF_TX_OK;
        }       
        if(sta == 0x0F)  
        { 
            RF_writeRegF(RF_FLUSH_TX, 0xFF); 
            return RF_MAX_TX;
        }   
        return RF_RX_OK; 
    }
    return ERROR;
}
