#include "protocol.h"
#include "crc.h"
#include "func_init.h"
#include "drv_SI4438.h"
#include "walk.h"
#include "hal_timer.h"
#include "hal_flash.h"
/************************************************************  
FileName: protocol.c   
Author: hf             
Date: 2021.03.23
Description: 数据协议处理及发送        
Version: V 1.0  
Other:
***********************************************************/ 
static uint8_t g_SI4463ItStatus[9] = { 0 };	//中断状态
static uint32_t SendProtocolLength = 0;
uint8_t g_SI4463RxBuffer[64] = { 0 };	//接收数据缓存buffer
u8 crcdata;
int connetflg;

Msg_t p_Msg;
Protocol433_t *Protocol4438;//433协议：接收总层板协议
MsgProtocol_t transferProtocol;

extern VoidGear_Walk_t* pW_Instance;
extern Event_Flag_t p_Flag;//标志位
extern u8 Si4438ChannelSet;
/*数据发送*/
void RF433_SendData(Msg_t *d)
{
	u8 len;
	len=sizeof(p_Msg);
	SI446x_Send_Packet(Si4438ChannelSet, 0x80,(uint8_t *)d, len);
}

void Si4438_Rcecv_Buff1(void)
{
		uint8_t i;
	
		SI446x_Interrupt_Status( g_SI4463ItStatus );		//读中断状态
		
		if(g_SI4463ItStatus[3] & (0x01<<4))//中断状态
		{
			i = SI446x_Read_Packet( g_SI4463RxBuffer );		//读FIFO数据
			if( i != 0 )
			{
				Protocol4438 = (Protocol433_t*)g_SI4463RxBuffer;
				i=0;
				
			}
		
			SI446x_Change_Status( RXTUNE_STATE );
			while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
			SI446x_Start_Rx(Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3 );
		}		
}

extern u8 SI433_ID;

void SI443x_dataHandle(void)
{
	u8 shakeDataLen;
	u8 shakeData[54];
	
    shakeDataLen = Protocol4438->data[8];//数据长度
    memcpy(shakeData, &Protocol4438->data[10], shakeDataLen);
}

void Si4438_Rcecv_Buff(void)
{
		uint8_t i;
		u8 location;
	
		SI446x_Interrupt_Status( g_SI4463ItStatus );		//读中断状态 
		
		if(g_SI4463ItStatus[3] & (0x01<<4))//中断状态 PACKET_RX状态位
		{
			i = SI446x_Read_Packet( g_SI4463RxBuffer );		//读FIFO数据 返回数据个数
			if( i != 0 )
			{
				Protocol4438 = (Protocol433_t*)g_SI4463RxBuffer;
				i=0;
				
				if(Protocol4438->data[0] == 0x66 && Protocol4438->data[1] == 0x65 && Protocol4438->data[2] == SI433_ID &&\
					Protocol4438->data[3] ==  Si4438ChannelSet && Protocol4438->data[15]==0x64)
				{
					crcdata = crc8_maxim(Protocol4438->data,14);

					if(Protocol4438->data[14] != crcdata)
					{
						memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
						return;
					}
					
					if(Protocol4438->data[6]!=0)//信道每次必须设置，优先级高于脚步ID
					{
						if(Protocol4438->data[4]==0x55 || Protocol4438->data[4]==0x56)//是否设置脚步ID
						{
							SI433_ID=Protocol4438->data[4];
							flash_config.pair_device.device_num=SI433_ID;
						}
						Si4438ChannelSet=Protocol4438->data[6];
						
						flash_config.pair_device.pair_ch=Si4438ChannelSet;//保存信道到flash
						GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));
						printf("%d->%x %x\r\n",__LINE__,Si4438ChannelSet,SI433_ID);
					}
					//自动开关机控制
					if(Protocol4438->data[8]==0x01)
						p_Flag.is_TurnOffFlag=true;
					else if(Protocol4438->data[8]==0x00)
						p_Flag.is_TurnOffFlag=false;
//					printf("%x\r\n",p_Flag.is_TurnOffFlag);
					RF433_SendData(&p_Msg);		//发送数据	
					p_Flag.is_ConnectTerminal = true;//收到应答命令标志	
					connetflg=0;//离线计数清零
					memset(&p_Msg,0,sizeof(p_Msg));		//清空发送数据
					memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
//					memset(&Protocol4438,0,sizeof(Protocol4438));
					return;
				}
			}

			/*读到接受状态或异常状态都自动切换为接收状态才能自动重连*/
			SI446x_Change_Status( RXTUNE_STATE );
			while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
			SI446x_Start_Rx(Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3 );
		}
		else if(Protocol4438->data[0] == 0x99 && Protocol4438->data[1] == 0x33)
		{
			location = Protocol4438->data[10] & 0xF8;
			location = location >> 3;
			
			if(location==13)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				delay_ms(100);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			}
//			SI443x_dataHandle();
//			memset(Protocol4438,0,sizeof(Protocol433_t));
//			shakeData_Analysis(); //根据震动数据配置pwm
		}
		else
		{
			connetflg++;
			if(connetflg>=2000)
			{
				connetflg=0;//清零后防止脚步不能秒连
//				memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
				p_Flag.is_ConnectTerminal = false;
				SI446x_Change_Status( RXTUNE_STATE );
				while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
				SI446x_Start_Rx(Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3 );
			}
		}
}
static u8 Version[12];
u8* GetVerison(void)
{
	int i = 0;
	int temp = HARDWARE_VERSION;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = BYTE0(temp);
	temp = SOFEWARE_VERSION;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = BYTE0(temp);
	temp = PROTOCOL_VERSION;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i] = 	 BYTE0(temp);
//	memcpy(&Version[0],&res[0],12);
	return Version;
}

////////////////////协议处理结构体申明///////////////////////
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
