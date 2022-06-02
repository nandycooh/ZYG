#ifndef __PROTOL_H
#define __PROTOL_H

#include "stm32f10x.h"
#include <stdint.h>
#include <string.h>

//数据缓存buf大小
#define MAX_R_BUF 28
//数据封装buf大小
#define MSG_SEND_BUFFER          26//有效数据的最大长度

#define FORMAT_HEAD1 0x77
#define FORMAT_HEAD2 0x76
#define FORMAT_TAIL  0x75

#pragma pack(push)//保存对齐状态 
#pragma pack(1)//设定为1字节对齐 


/*----------------------版本号---------------------------------------------------------*/
/*每一字节算一个版本号位 高：大版本号,中：中版本号，小：小版本号，
最后1个字节用于前4位标识设备端识别码 ABCDEFG 对应1234567 例如：2x标识为B端，7x标识为G端 
后4位标识是否为测试版本 x0测试版本， x1正常版本， xF发布版本*/
#define HARDWARE_VERSION					0x0004047F//硬件版本号 5.2.0
#define SOFEWARE_VERSION					0x0002037F//软件版本号 5.2.1
#define PROTOCOL_VERSION          			0x0001027F//协议版本号 0.1.2 

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

//协议1字节对齐
typedef struct {
	uint8_t 	MsgHead[2];
		uint8_t MsgPayLoad[MSG_SEND_BUFFER];//由于协议定长，其实这样定义不能让其变得简洁易懂,主要缺少对数据类型的管理，导致数据单一。
}MsgProtocol_t;

#pragma pack(pop)
//格式话
typedef struct
{
	void(*FmtU8)(uint8_t d);
	void(*FmtU16)(uint16_t d);
	void(*FmtU32)(uint32_t d);
	void(*FmtStr)(char* str);
	void(*FmtHex)(uint8_t* Dt, uint16_t len);
	void(*FmtHead)(void);
	void(*Fmtend)(void);
//	void(*FmtCmd)(uint16_t cmd);
	void(*FmtCrc16)(void);
	void(*FmtCrc8)(void);
	uint8_t*  (*GetBuffer)(void);
	uint16_t(*GetLength)(void);
}ProtocolFormat_t;

ProtocolFormat_t* SendProtocolGetObj(void);
MsgProtocol_t*  Protocol_Analysis(void* d);


///////////////////////////////////////////////////////////
/*数据缓存区 发射数据*/
typedef struct{
	uint8_t ID;
	uint8_t data[MAX_R_BUF];
	uint16_t length;
}Msg_t;

/*433协议 应答数据*/
typedef struct
{
	uint8_t data[16];
}Protocol433_t;

extern u8 gun_statusflg;
void SendKeyDown(void);
void SendKeyUp(void);
void Handgun_Send433Data(void);
void Si4438_Rcecv_Buff(void);
void RF433_SendData(Msg_t *d);
void Handgun_SendData(void);
u8* GetVerison(void);
#endif

