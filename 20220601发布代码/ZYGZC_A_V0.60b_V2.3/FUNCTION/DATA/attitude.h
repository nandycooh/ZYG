#ifndef __ATTITUDE_H
#define __ATTITUDE_H	

#include "sys.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include "usr_c322.h"
#include "drv_SI4438.h"

/* Macro -------------------关于全身传感器-----------------------------------------------------*/

#if IS_3_POINT_MODE > 0
	#define   CABLE_SLAVE_DEVICE_8_EN         0
#else
	#define   CABLE_SLAVE_DEVICE_8_EN         1
#endif

#if CABLE_SLAVE_DEVICE_8_EN > 0
	#define CABLE_SLAVE_DEVICE_CNT    				10//有线姿态传感器个数 6个 或者8个 不能小于6个
#else
	#define CABLE_SLAVE_DEVICE_CNT    				6//有线姿态传感器个数 6个 或者8个 不能小于6个
#endif

#define GAMEPAD_SENSOR_PROTOCOL_HEAD_LEN    3//手柄传感器协议头数据长度
#define WIRED_SENSOR_PROTOCOL_HEAD_LEN      3//有线传感器协议数据长度
#define WIRED_EULER_PROTOCOL_OFFEST					8//协议头+四元数
#define WIRELESS_KEY_DATA_OFFSET \
	GAMEPAD_SENSOR_PROTOCOL_HEAD_LEN + \
	QUATERNIONS_DATA_LEN+1                    //按键数据偏移
#define ROCKER_DATA_OFFSET \
	WIRELESS_KEY_DATA_OFFSET+1                //摇杆数据偏移

#define CABLE_SENSOR_DATA_LEN     					11//有线传感器有效数据长度
#define CABLE_WAIST_SENSOR_DATA_LEN     		17//有线传感器有效数据长度
#define WIRELESS_SLAVE_DEVICE_CNT 					4//无线传感器的个数 包括两个手柄  两个脚步传感器
#define SENSOR_DATA_LEN                     (QUATERNIONS_DATA_LEN+1+1)//1个字节精度1个字节在线状态
#define ALL_SENSOR_QUATERNIONS_DATA_LEN   	(QUATERNIONS_DATA_LEN*SENSOR_NUM)//总的四元数据长度
#define SENSOR_NUM													14//总的传感器数的个数没有的传感器占位
#define GAME_PAD_KEY_LEN                  	10//手柄数据按键数据的长度
#define GUN_485_ADDR                        0x11//枪的地址
#define WAIST_485_ADDR											0x05//腰的地址
#define QUATERNIONS_DATA_LEN      					(4)//四元数的数据长度 用16位
#define EULER_DATA_LEN											(6)//欧拉角数据长度
#define GAMEPAD_DATA_LEN          					(12)//手柄数据长度
#define FOOTSTEP_DATA_LEN         					(14)//脚步传感器数据长度
/****脚部步行定义****/
#define WALK_PROTOCOL_OFFEST								7//协议头+四元数
#define WALK_DATA_LEN												3
/*-----------------*/

__packed typedef struct
{
	/*全身姿态数据：由人体上半身传感器四元数数
	据和人体运动状态数据组成，四元数为归一化处
	理后数据，均为小数，扩大100倍后上传，上传
	顺序为W,X,Y,Z,采用右手坐标系，共32字节有符
	号数据*/
	u8 QuaternionsData[ALL_SENSOR_QUATERNIONS_DATA_LEN];
	
	/*腰部欧拉角数据X，Y，Z 每个轴占2位，范围
	0-360°*/
	u8 EulerData[EULER_DATA_LEN];
	
	/*脚部步行数据*/
	u8 LeftWalk[WALK_DATA_LEN];
	u8 RightWalk[WALK_DATA_LEN];
	
	/*人体运动趋势状态：为无符号数据，共1字节，
	有效为1，复位为0。靠左为人的身体有向左倾，
	靠右，向后同理。向前标志位暂时保留*/
	u8 MotionState;
	
	/*全身姿态感应器状态：无符号数据，共2字节，
	每一位表示一个部位的传感器精度，1表示传感
	器的精度达到了3（即最高精度），0表示传感器
	的精度为非3. bit5~bit0 保留位*/
	u16 SensorAccuracy;
	
	/*全身姿态在线状态:无符号数据共2字节
	每一位表示一个部位的传感器状态,1表示传感
	器的状态是在线,0表示传感器的状态为离线*/
	u16 OnLineState;
	
}attitude_data_t;

//数据上传
__packed typedef struct
{
	/*姿态数据和按键数据上传固定长度字节*/
	u8 attitude_data[sizeof(attitude_data_t)];
	
	/*GAME_PAD_KEY_LEN个字节的按键数据*/
	u8 key_data[GAME_PAD_KEY_LEN];
	
	/*增加一个字节的433信道显示*/
	u8 nrf_channel;
	
	u8 Auto_Shutdown_Flg;
	/*广播数据上传*/
//	BroadCastData_t BroadCastData;//主协议不发广播数据
	
}DataPayload_t;

u8* GetbatteryPresent(void);	
void attitudeUploadHandler(void);
	
#endif
