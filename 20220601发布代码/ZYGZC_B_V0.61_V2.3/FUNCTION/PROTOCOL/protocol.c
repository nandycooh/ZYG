#include "../function/protocol/protocol.h"
#include "../FUNCTION/SI4438/drv_SI4438.h"
#include "../function/crc/crc.h"
#include "stdlib.h" 
#include "string.h" 

/************************************************************  
FileName: protocol.c   
Author: hf             
Date: 2021.03.23
Description: 数据协议处理及发送        
Version: V 1.0  
Other:
***********************************************************/

static uint32_t SendProtocolLength = 0;
uint8_t g_SI4463ItStatus[9] = { 0 };	//中断状态
uint8_t g_SI4463RxBuffer[64] = { 0 };	//接收数据缓存buffer
extern u8 Si4438ChannelSet;
////////////////////协议处理结构体申明///////////////////////
MsgProtocol_t transferProtocol;
Msg_t p_Msg;
Protocol433_t *Protocol4438;

/*数据发送*/
void RF433_SendData(Msg_t *d)
{
	u8 len;
	len=sizeof(p_Msg);
	SI446x_Send_Packet(Si4438ChannelSet, 0x80,(uint8_t *)d, len);
}

void Si4438_Rcecv_Buff(void)
{
		uint8_t i;
	
		SI446x_Interrupt_Status( g_SI4463ItStatus );		//读中断状态
		
		if(g_SI4463ItStatus[3] & (0x01<<4))//中断状态
    {
			i = SI446x_Read_Packet( g_SI4463RxBuffer );		//读FIFO数据
			if( i != 0 )
			{
				Protocol4438 = (Protocol433_t*)g_SI4463RxBuffer;
			}
		
			SI446x_Change_Status( RXTUNE_STATE );
			while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
			SI446x_Start_Rx(Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3 );
		}
}

/******************************协议处理部分*************************/

//8
static void ThisFmtU8(uint8_t d)
{
	transferProtocol.MsgPayLoad[SendProtocolLength++] = d;
}
//16
static void ThisFmtU16(uint16_t d)
{
	memcpy(&transferProtocol.MsgPayLoad[SendProtocolLength], &d, 2);
	SendProtocolLength += 2;
}
//32
static void ThisFmtU32(uint32_t d)
{
	memcpy(&transferProtocol.MsgPayLoad[SendProtocolLength], &d, 4);
	SendProtocolLength += 4;
}
//str
static void ThisFmtStr(char* str)
{
	memcpy(&transferProtocol.MsgPayLoad[SendProtocolLength], str, strlen(str));
	SendProtocolLength += strlen(str);
}
//head
static void ThisFmtHead(void)
{
	SendProtocolLength = 0;
	//头
	transferProtocol.MsgHead[0] = FORMAT_HEAD1;
	transferProtocol.MsgHead[1] = FORMAT_HEAD2;
}
//end
static void ThisFmtend(void)
{
	transferProtocol.MsgPayLoad[SendProtocolLength++] = FORMAT_TAIL;
}
//get buffer
static  uint8_t* ThisGetBuffer(void)
{
	return (uint8_t*)&transferProtocol;
}
//get length
static uint16_t ThisGetLength()
{
	return SendProtocolLength + sizeof(MsgProtocol_t) - MSG_SEND_BUFFER;
}
//crc16
static void ThisFmtCrc16(void)
{
	uint16_t Dt = Message_count_CRC_2(SendProtocolLength + 2, (u8*)&transferProtocol);
	memcpy(&transferProtocol.MsgPayLoad[SendProtocolLength], (uint8_t*)&Dt, 2);
	SendProtocolLength += 2;
}

//crc8
static void ThisFmtCrc8(void)
{
	transferProtocol.MsgPayLoad[SendProtocolLength++] = crc8_maxim((u8*)&transferProtocol, SendProtocolLength + 2);
}

static void ThisFmtHex(uint8_t* Dt, uint16_t Len)
{
	memcpy(&transferProtocol.MsgPayLoad[SendProtocolLength], Dt, Len);
	SendProtocolLength += Len;
}


//cmd
//static void ThisFmtCmd(uint16_t cmd)
//{
//	uint16_t tmp_cmd = cmd ^ R_CODE;
//	transferProtocol.MsgClass = tmp_cmd;
//}

ProtocolFormat_t SendProtocol =
{
	ThisFmtU8,
	ThisFmtU16,
	ThisFmtU32,
	ThisFmtStr,
	ThisFmtHex,
	ThisFmtHead,
	ThisFmtend,
//	ThisFmtCmd,
	ThisFmtCrc16,
	ThisFmtCrc8,
	ThisGetBuffer,
	ThisGetLength,
};

//get object
ProtocolFormat_t* SendProtocolGetObj(void)
{
	return &SendProtocol;
}

//数据解析
MsgProtocol_t* Protocol_Analysis(void* d)
{
	MsgProtocol_t *p = (MsgProtocol_t*)d;
	return p;
}

