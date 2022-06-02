#include "gun.h"

static u8 gamePadKeyData[GAME_PAD_KEY_LEN];


///**
//  * @brief 读取手柄的按键数据
//  * @param  
//  * @retval
//**/
//u8* APL_gamePadKeyGet(void)
//{
//	/*左右手柄 4个按键*/
//		if(SI4438DataStruct.gunflg==1)
//		{
//			SI4438DataStruct.gunflg=0;//接收到手枪数据标志位
//			//gamePadKeyData[5] =	p_Msg->data[7];
//			
//			return gamePadKeyData;
//		}
//		else
//			gamePadKeyData[7]=0x00;
//			
//	return 0;
//}
