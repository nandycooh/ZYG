#ifndef __PROTOL_H
#define __PROTOL_H

#include "stm32f10x.h"
#include "drv_SI4438.h"
#include "stdio.h"
#include "string.h"
#include "usr_c322.h"
#include "md5.h"  


#define MCU_TO_PC_BUF_SIZE        128//下位机上传数据BUF的大小
#define VERSION                   0x09//协议版本号 高2位大版本号,低6位小版本号:如(0.1.0)->(00 001 000)->hex(0x08)
#define ATTITUDE_DATA_CMD         0x03//姿态数据命令
#define BAT_DATA_CMD        			0x0A//电量
#define VERSION_DATA_CMD        	0x0C//版本上传
#define KEY_DATA_CMD        		  0x04//按键命令
#define BROADCAST_DATA_CMD        0x02//广播数据命令
#define LOGIN_DATA_CMD        		0x06//登录数据命令
#define BOOT_OFF_DELAY         		20//长按关机延时时间
#define POWER_KEY                 MACHINE_KEY2//电源按键
#define BAT_UPDATA_TIME           3*20*3*20//电量上传的速度1min间隔


#define USB_MODE_EN               0//usb vcp模式
#define CABLE_DEBUG_LOSS_EN       0//是否打印有线传感器的丢包率
#define IS_4_POINT_MODE           1//是否配置成4点四元数模式
#define IS_3_POINT_MODE           0//是否配置成3点四元数模式
#define NEW_PROTOCOL_EN           1//新的协议使用MAC地址固定上传
#define UCOSII_ENABLE        			1//是否使用ucos ii 操作系统
#define IS_HEART_CHECK            1//是否开启心跳检测 检测是否断开
#define LED_STATE_DEBUG_MODE      0//LED调试模式时的状态和正式模式的状态切换
#define IS_SUPPORT_GUN            0//是否支持枪的数据
#define IS_LED_LOW_ENABLE         0//led是否是低电平开
#define IS_TEST_ADDR              0//是否RF是用测试的地址不需要配对

#define MCU_TO_PC_BUF_SIZE        128//下位机上传数据BUF的大小
#define VERSION                   0x09//协议版本号 高2位大版本号,低6位小版本号:如(0.1.0)->(00 001 000)->hex(0x08)
#define ATTITUDE_DATA_CMD         0x03//姿态数据命令
#define BAT_DATA_CMD        			0x0A//电量
#define VERSION_DATA_CMD        	0x0C//版本上传
#define KEY_DATA_CMD        		  0x04//按键命令
#define BROADCAST_DATA_CMD        0x02//广播数据命令
#define LOGIN_DATA_CMD        		0x06//登录数据命令
#define BOOT_OFF_DELAY         		20//长按关机延时时间
#define POWER_KEY                 MACHINE_KEY2//电源按键
#define BAT_UPDATA_TIME           3*20*3*20//电量上传的速度20ms间隔

/*----------------------版本号---------------------------------------------------------*/
/*每一字节算一个版本号位 高：大版本号,中：中版本号，小：小版本号，
最后1个字节用于前4位标识设备端识别码 ABCDEFG 对应1234567 例如：2x标识为B端，7x标识为G端 
后4位标识是否为测试版本 x0测试版本， x1正常版本， xF发布版本*/
#define HARDWARE_VERSION					0x0006007F//硬件版本号 5.2.0
#define SOFEWARE_VERSION					0x0002027F//软件版本号 5.2.1
#define PROTOCOL_VERSION          			0x0001037F//协议版本号 0.1.2 

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define LEFTWALKID		0x55
#define RIGHTWALKID		0x56
#define HANDGUNID		0x57
#define BELTID 			0x58
#define SPONTOON		0x59
#define SHIELD			0x60
#define PAPPER_SPRAY	0x61

///////////////////////////////////////////////////////////////////
//210611
//////////////////////////////////////////////////////////////////
#define MAX_R_BUF 28

/*数据缓存区*/
typedef struct
{
	int Gun_Hardware_Version;
	int Gun_Soft_Version;
	int Gun_Protocol_Version;
	
	int Foot_Hardware_Version;
	int Foot_Soft_Version;
	int Foot_Protocol_Version;
	
	int Back_Hardware_Version;
	int Back_Soft_Version;
	int Back_Protocol_Version;
	
}SLAVE_VERSION_MSG;

/*数据缓存区*/
typedef struct{
	uint8_t ID;
	uint8_t data[MAX_R_BUF]; //_attribute_((aligned(4)));
	uint16_t length;
}Msg_t;
/*433协议*/
typedef struct
{
	uint8_t data[16*3];
}Protocol433_t;



//433接收数据标志位
typedef struct
{
	bool gunflg;
	bool leftwalkflg;
	bool righttwalkflg;
	bool beltflg;
	bool spontoonflg;
	bool shieldflg;
	bool pappersprayflg;
}SI4438DataDef;



__packed typedef struct
{
	u8 	MAC[6];
}MacDataDef ;

void Si4438_Rcecv_Buff(void);
	

void SendHeartRateData(void);
///////////////////////////////////////
/**
  * @brief  发送数据到PC端 小端模式
  * @param 
  * @retval
**/
u32 protocol_sendData(void * pData,u32 DataLen,u8 cmd);

/**
  * @brief  重发数据
  * @param 
  * @retval
**/
void protocol_ResendData(u32 PackageCnt,void * pData,u32 DataLen,u8 cmd);

/**
  * @brief 累加和校验方式
  * @param 
  * @retval
**/
u8 sumCheck(u8 *buf, u8 len);


/**
  * @brief crc8校验
  * @param 
  * @retval
**/
u8 crc8_maxim(u8 *data, u16 length);

u16 Message_count_CRC_2(u8 CrcLenth,u8 *CRC_Data);
void Ack_data_protocol(u8 slaveid,void * pData,u32 DataLen);
void SendMasterCmdData(void);
void Si433_Rcecv_Buff(void);
void Send_Master_Cmd_Data(u8 sendnum);
#endif

