#include "attitude.h"


/*数据上传*/
DataPayload_t dataPayload;

/*全身姿态数据*/
attitude_data_t attitude_data;

static u16 gunstat,lwalkstat,rwalkstat,beltstat,spontoonstat,shieldstat,papperspraystat;//从机在线状态
static u8 handgun_online_flg,walkl_online_flg,walkr_online_flg,belt_online_flag,spontoon_online_flg,shield_online_flg,papperspray_online_flg;//从机在线标志
static u8 handgun_offline_flg,walkl_offline_flg,walkr_offline_flg,belt_offline_flag,spontoon_offline_flg,shield_offline_flg,papperspray_offline_flg;//从机离线标志

/*电量 左 有 左脚 右脚 主板*/
u8 gamePadBatteryData[5];
extern SI4438DataDef SI4438DataStruct;
/**
  * @brief 读取姿态数据 异步获取
  * @param  
  * @retval
**/
attitude_data_t* APL_attitudeGet(void)
{
	//////////////////////////////////////////////// 左脚 ////////////////////////////////////////////////////
	/*左脚存在状态*/
	if(SI4438DataStruct.leftwalkflg==true)
	{
		walkl_online_flg++;
		walkl_offline_flg=0;
		
		if(walkl_online_flg>=3)
		{
			SI4438DataStruct.leftwalkflg=false;
			lwalkstat |= 0x0008;
		}	
	}
	else/*左脚离线状态*/
	{
		walkl_offline_flg++;
		walkl_online_flg=0;
		
		if(walkl_offline_flg>=33)//粗略2S左右
		{
			lwalkstat &= 0x0000;
			memset(attitude_data.LeftWalk,0,sizeof(attitude_data.LeftWalk));
		}	
	}
	
	//////////////////////////////////////////////// 右脚 ////////////////////////////////////////////////////
	/*右脚在线状态*/	
	if(SI4438DataStruct.righttwalkflg==true)
	{
		walkr_online_flg++;
		walkr_offline_flg=0;
		
		if(walkr_online_flg>=3)
		{
				SI4438DataStruct.righttwalkflg=false;
				rwalkstat |= 0x0004;
		}	
	}
	else/*右脚离线状态*/
	{
		walkr_offline_flg++;
		walkr_online_flg=0;
		
		if(walkr_offline_flg>=33)//粗略2S左右
		{
			rwalkstat &= 0x0000;
			memset(attitude_data.RightWalk,0,sizeof(attitude_data.RightWalk));
		}	
	}
	
	///////////////////////////////////////////////// 手枪 ////////////////////////////////////////////////////
	/*手枪在线状态*/
	if(SI4438DataStruct.gunflg==true)
	{
		handgun_offline_flg=0;
		handgun_online_flg++;
		
		if(handgun_online_flg>=3)
		{
			SI4438DataStruct.gunflg=false;
			gunstat |= 0x0010;
		}
	}
	else/*手枪离线状态*/
	{
		handgun_offline_flg++;
		handgun_online_flg=0;
		if(handgun_offline_flg>=33)//粗略2S左右
		{					
			gunstat &= 0x0000;
//			memset(dataPayload.key_data,0,sizeof(dataPayload.key_data));
		}	
	}
	
	////////////////////////////////////////////////// 背心 ////////////////////////////////////////////////////
	/*背心在线状态*/
	if(SI4438DataStruct.beltflg==true)
	{
		belt_online_flag++;
		belt_offline_flag=0;
		
		if(belt_online_flag>=3)
		{
			SI4438DataStruct.beltflg=false;
			beltstat |= 0x0400;
		}
	}
	else/*背心离线状态*/
	{
		belt_offline_flag++;
		belt_online_flag=0;
		
		if(belt_offline_flag>=33)//粗略2S左右
		{
			beltstat &= 0x0000;//断开状态位清除在线状态及数据
			memset(attitude_data.QuaternionsData,0,sizeof(attitude_data.QuaternionsData));
		}	
	}
	
	///////////////////////////////////////////////// 警棍 ////////////////////////////////////////////////////
	/*警棍在线状态*/
	if(SI4438DataStruct.spontoonflg==true)
	{
		spontoon_offline_flg=0;
		spontoon_online_flg++;
		
		if(spontoon_online_flg>=3)
		{
			SI4438DataStruct.spontoonflg=false;
			spontoonstat |= 0x0002;
		}
	}
	else/*离线状态*/
	{
		spontoon_offline_flg++;
		spontoon_online_flg=0;
		if(spontoon_offline_flg>=33)//粗略2S左右
		{					
			spontoonstat &= 0x0000;
//			memset(dataPayload.key_data,0,sizeof(dataPayload.key_data));
		}	
	}
	
	///////////////////////////////////////////////// 盾牌 ////////////////////////////////////////////////////
	/*盾牌在线状态*/
	if(SI4438DataStruct.shieldflg==true)
	{
		shield_offline_flg=0;
		shield_online_flg++;
		
		if(shield_online_flg>=3)
		{
			SI4438DataStruct.shieldflg=false;
			shieldstat |= 0x0040;
		}
	}
	else/*离线状态*/
	{
		shield_offline_flg++;
		shield_online_flg=0;
		if(shield_offline_flg>=33)//粗略2S左右
		{					
			shieldstat &= 0x0000;
//			memset(dataPayload.key_data,0,sizeof(dataPayload.key_data));
		}	
	}
	
	///////////////////////////////////////////////// 辣椒水 ////////////////////////////////////////////////////
	/*辣椒水在线状态*/
	if(SI4438DataStruct.pappersprayflg==true)
	{
		papperspray_offline_flg=0;
		papperspray_online_flg++;
		
		if(papperspray_online_flg>=3)
		{
			SI4438DataStruct.pappersprayflg=false;
			papperspraystat |= 0x0001;
		}
	}
	else/*离线状态*/
	{
		papperspray_offline_flg++;
		papperspray_online_flg=0;
		if(papperspray_offline_flg>=33)//粗略2S左右
		{					
			papperspraystat &= 0x0000;
//			memset(dataPayload.key_data,0,sizeof(dataPayload.key_data));
		}	
	}
	
	attitude_data.OnLineState=lwalkstat | rwalkstat | gunstat | beltstat | spontoonstat | shieldstat | papperspraystat;//从机在线状态
	
	return &attitude_data;
}

/**
  * @brief  姿态数据处理上传每一段时间上传一次
  * @param  
  * @retval
**/
void attitudeUploadHandler(void)
{		
	u16 attitudeDataLen = sizeof(attitude_data_t);
//	dataPayload.Auto_Shutdown_Flg=9;
	/*姿态数据 210114 全为0*/
	memset(dataPayload.attitude_data,0,attitudeDataLen);
	
	memcpy(dataPayload.attitude_data,APL_attitudeGet(),attitudeDataLen);
	
	/*发送姿态数据到PC 姿态数据加6个字节手柄数据 + 1个字节的无线模块通道数据*/
	protocol_sendData((uint8_t*)&dataPayload,attitudeDataLen+GAME_PAD_KEY_LEN+2,ATTITUDE_DATA_CMD);
}

/**
	* @brief 电池的电量上传
  * @param  
  * @retval
**/
u8* GetbatteryPresent(void)
{
	return gamePadBatteryData;
}

