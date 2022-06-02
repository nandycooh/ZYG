#ifndef __USR_C322_H
#define __USR_C322_H	

#include "stm32f10x.h"
#include "power.h"
#include "usr_c322.h"
#include "led.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include "protocol.h"
#include "md5.h" 
//#include "usart.h"
#include "motor.h"
#include "hal_flash.h"
#include "drv_SI4438.h"
#include "usart.h"	

/*********************************宏定义功能区**************************/
/*--------------------------------WIFI模块相关设置---------------------------------------------*/
#define CLIENT_PORT               8087//客服端的port
#define SERVER_PORT               "8087"//服务器的port
#define BOARD_CAST_CYCLE        1000//广播的间隔时间
#define RX_NUM_SIZE 44

//STM32 单片机
#define 	RestartSystem()								{	flash_config.reBoot.re_boot_flag = 1;\
																					GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));\
																					delay_ms(100);\
																					__disable_fault_irq(); NVIC_SystemReset();\
																				}  /*软件重启系统*/

//广播数据
__packed typedef struct
{
	u8 	MAC[6];
	u8 	IP[4];
	u16 port;
}BroadCastData_t;


//心跳数据
__packed typedef struct
{
	u8 hash[12];//设备编号
	u8 pwd[16];//密锁-MD5
	u8 IP[4];//IP
}LoginOrBoardCastAckData_t;

//功能标志位
__packed typedef struct
{
	u8 IsWorkMode;//工作模式
	u8 IsLoginOrBoardCastAck;//登录应答标志
	u8 IsResume;
	u8 reConnectFlag;//连接标志
	u8 IsconnectStatus;//广播命令标志
	u8 IsUploadVersion;//版本
	u8 IsUpBatteryFlag;//电亮上传标志位
	bool IsReoilFlag;//后坐力标志位
	u8 IsMotorFlag;
	bool IsSetChannel;
}funtionFlag_t;

/*********************************函数申明区****************************/
void RS232_Init(u32 bound);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void DataCallBack_WIFI(uint8_t* Data,uint16_t Len);
void Get_Mcu_Mac(void);
u8 Login_Or_Boardcast(uint8_t BoardcastMode);
void HeartCheckCallBack(void);
u8* GetVerison(void);
/*********************************API接口层****************************/


#endif
