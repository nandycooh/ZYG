#include "protocol.h"
#include "attitude.h"


SI4438DataDef SI4438DataStruct;
MacDataDef Macstruct;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*协议数据缓存*/
u8 protocolDataBuf[MCU_TO_PC_BUF_SIZE];

static u8 ReoilNum;

extern attitude_data_t attitude_data;
extern funtionFlag_t  funtionFlag;/*功能标志位*/

/*电量 左 有 左脚 右脚 主板*/
extern u8 gamePadBatteryData[5];

/*--------------------------------*/
/*        CRC 校验字节值表        */
/*--------------------------------*/
//CRC 高位字节值表
const u8 CRC_Array_H[] = 
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
};
//CRC低位字节值表 
const u8 CRC_Array_L[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/**
  * @brief    tranmission the hex data
  * @param  	data hex data ,send data length
  * @retval :   None
**/
static void hex_data_tranmission(uint8_t *data,uint16_t length)
{
	while(length--)
	{
		Usart_SendByte(USART1,*data++);
	}
}

u16 Message_count_CRC_2(u8 CrcLenth,u8 *CRC_Data)
{
	u8  Crc_H = 0xFF;                                   //高CRC字节初始化
	u8  Crc_L = 0xFF;                                   //低CRC字节初始化  
	u8  Index;                                          //CRC循环中的索引
	while (CrcLenth--)
	{                                                 	//传输消息缓冲区 
	   	Index = Crc_H ^ *CRC_Data++;                    //计算CRC
		Crc_H = Crc_L ^  CRC_Array_H[Index]; 
		Crc_L = CRC_Array_L[Index]; 
	}
	return((Crc_H << 8) | Crc_L); 
}
/**
  * @brief 累加和校验方式
  * @param 
  * @retval
**/
u8 sumCheck(u8 *buf, u8 len)
{
	u8 i, sum = 0;

	for(i = 0; i < len; i++) {
		sum += buf[i];
	}
	return sum;
}
/**
  * @brief crc8校验
  * @param 
  * @retval
**/
u8 crc8_maxim(u8 *data, u16 length)  
{  
	uint8_t i;  
	uint8_t crc = 0;   

	while(length--){  
		crc ^= *data++;  
		for (i = 0; i < 8; i++){  
			if (crc & 1)  
				crc = (crc >> 1) ^ 0x8C;  
			else  
				crc >>= 1;  
		}  
	}  
	return crc;  
}

static void data_protocol(u32 PackageCnt,void * pData,u32 DataLen,u8 cmd)
{
	static u32 i = 0;
	i = 0;
	
	/*帧头*/
	protocolDataBuf[i++] = 0x66;
	protocolDataBuf[i++] = 0xFF;
	
	/*版本号*/
	protocolDataBuf[i++] = VERSION;
#if NEW_PROTOCOL_EN >0	
	/*设备的MAC地址6个字节*/
	memcpy(protocolDataBuf+i,Macstruct.MAC,6);
//	memcpy(protocolDataBuf+i,BroadCastData.MAC,6);//210701 	
	i += 6;
#endif	
	/*包序号*/
	memcpy(protocolDataBuf+i,&PackageCnt,4);
	i += 4;
	
	/*ACK 默认为0*/
	protocolDataBuf[i++] = 0x00;
	
	/*数据长度 有效数据载荷长度*/
	protocolDataBuf[i++] = DataLen;
	
	/*命令*/
	protocolDataBuf[i++] = cmd;
	
	/*有效数据*/
	memcpy(protocolDataBuf+i,pData,DataLen);
	i +=DataLen;
	
	/*CRC校验码 2字节校验码*/
	u16 crc = Message_count_CRC_2(i,protocolDataBuf);
	memcpy(protocolDataBuf+i,&crc,2);//将数据追加复制到后面
	i +=2;
	
	/*通过WIFI发送数据*/
	hex_data_tranmission(protocolDataBuf,i);
}

/**
  * @brief  发送数据到PC端 小端模式
  * @param 
  * @retval
**/
u32 protocol_sendData(void * pData,u32 DataLen,u8 cmd)
{
	static u32 PackageCnt = 0;//包序号累加
	
	/*包序号 4字节*/
	PackageCnt++;
	
	data_protocol(PackageCnt,pData,DataLen,cmd);
	
	/*返回包的序号*/
	return PackageCnt;
}

/**
  * @brief  重发数据
  * @param 
  * @retval
**/
void protocol_ResendData(u32 PackageCnt,void * pData,u32 DataLen,u8 cmd)
{
	data_protocol(PackageCnt,pData,DataLen,cmd);
}

///////////////////////////////////////////////////////////////////
//轮循协议部分210611
//////////////////////////////////////////////////////////////////

Protocol433_t Protocol433;
Msg_t 	*p_Msg;
extern DataPayload_t dataPayload;
static uint8_t g_SI4463ItStatus[9] = { 0 };	//中断状态
static uint8_t g_SI4463RxBuffer[64] = { 0 };	//接收数据缓存buffer

static u8 nu;
/*数据发送*/
void RF433_SendData(Protocol433_t *d)
{
	u8 len;
	len=sizeof(Protocol433);
	
	SI446x_Send_Packet(dataPayload.nrf_channel, 0x80,(uint8_t *)d, len);
	
	if(d->data[13]==0x03)
		printf("z s 03 %d\r\n",nu++);
	if(nu>=15)
		nu=0;
	
//	memset(&d,0,sizeof(d));
}
u8 GunCannelFlag=0;
u8 FootLCannelFlag=0;
u8 FootRCannelFlag=0;
u8 WaistCannelFlag=0;
extern u8 NewChannel;
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;


void Send_Master_Cmd_Data(u8 sendnum)
{
	Protocol433.data[0]=0x66;
	Protocol433.data[1]=0x65;
	Protocol433.data[3]=dataPayload.nrf_channel;
	
	switch(sendnum)
	{				
		case 1:
				Protocol433.data[2]=HANDGUNID;
				
				//SDK控制后座力指令
//				if(funtionFlag.IsReoilFlag==true)
//				{
//					Protocol433.data[13]=0x03;
//					printf("recv reoil cmd\r\n");
//					ReoilNum++;
//					if(ReoilNum>=2)
//					{
//						ReoilNum=0;funtionFlag.IsReoilFlag=false;
//					}
//				}
//				else
//				{
//					Protocol433.data[13]=0x00;
//				}
		
				if(Protocol433.data[13]==0x03)
				{
					printf("gun recv reoil cmd\r\n");
					ReoilNum++;
					if(ReoilNum>=2)
					{
						ReoilNum=0;
						Protocol433.data[13]=0x00;
					}
				}
				
				//上位机设置信道
				if(Protocol433.data[5]!=0)
				{
					GunCannelFlag++;
					if(GunCannelFlag>=5)
					{
						GunCannelFlag=0;
						Protocol433.data[5]=0;//信道
					}
				}
				break;
		case 2:Protocol433.data[2]=BELTID;
				//上位机设置信道 发送几次后清零，不然会一直对从机进行设置
				if(Protocol433.data[7]!=0)
				{
					WaistCannelFlag++;
					if(WaistCannelFlag>=5)
					{
						WaistCannelFlag=0;
						Protocol433.data[7]=0;//信道
					}
				}
				break;	
		case 3:Protocol433.data[2]=LEFTWALKID;
				//上位机设置ID和信道:同时设置ID和信道或者单独设置信道				
				if(Protocol433.data[4]==LEFTWALKID || Protocol433.data[6]!=0)
				{
					FootLCannelFlag++;
					if(FootLCannelFlag>=5)
					{
						FootLCannelFlag=0;
						Protocol433.data[6]=0;//信道
						Protocol433.data[4]=0;//ID
					}
				}
				break;
		case 4:Protocol433.data[2]=RIGHTWALKID;
				//上位机设置ID和信道:同时设置ID和信道或者单独设置信道	
				if(Protocol433.data[4]==RIGHTWALKID || Protocol433.data[6]!=0)
				{
					FootRCannelFlag++;
					if(FootRCannelFlag>=5)
					{
						FootRCannelFlag=0;
						Protocol433.data[6]=0;//信道
						Protocol433.data[4]=0;//ID
					}
				}
				break;
		case 5:Protocol433.data[2]=SPONTOON;break;
		case 6:Protocol433.data[2]=SHIELD;break;
		case 7:Protocol433.data[2]=PAPPER_SPRAY;break;
		default : break;
	}
	Protocol433.data[14]=crc8_maxim(Protocol433.data,14);
	Protocol433.data[15]=0x64;

	SI446x_Send_Packet(dataPayload.nrf_channel, 0x80,(uint8_t *)&Protocol433, sizeof(Protocol433));
	//等待从机信道设置完后再设置总成板信道220107
	if(funtionFlag.IsSetChannel==true && Protocol433.data[5]==0 && Protocol433.data[6]==0 && Protocol433.data[7]==0)
	{
		funtionFlag.IsSetChannel=false;
		dataPayload.nrf_channel = NewChannel;//得到信道
		flash_config.pair_device.pair_ch=dataPayload.nrf_channel;//保存信道到flash
		GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));
	}
//	printf("%x\r\n",Protocol433.data[8]);
}

SLAVE_VERSION_MSG SLAVE_VERSION_MSG_Struct;
void Si433_Rcecv_Buff(void)
{
	uint8_t i,crc_data;
	uint8_t j;

	SI446x_Interrupt_Status( g_SI4463ItStatus );		//读中断状态
	
	if(g_SI4463ItStatus[3] & (0x01<<4))//中断状态
    {
		i = SI446x_Read_Packet( g_SI4463RxBuffer );		//读FIFO数据
		if( i != 0 )
		{
			p_Msg=(Msg_t *)g_SI4463RxBuffer;
			
//			for(j=0;j<=27;j++)
//			{
//				printf("%x ",p_Msg->data[j]);
//				if(j>=27)
//				{
//					j=0;
//					printf("\r\n");
//					
//				}
//			}
			
			if(p_Msg->data[0]==0x77&& p_Msg->data[1]==0x76 && p_Msg->data[27]==0x75)
			{
				crc_data = crc8_maxim(p_Msg->data,26);
				if(p_Msg->data[26]!=crc_data)
				{
					memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
					return;
				}
					
				switch(p_Msg->data[2])
				{
					case HANDGUNID:
							SI4438DataStruct.gunflg=true;
							dataPayload.key_data[5] = p_Msg->data[7];
					
							SLAVE_VERSION_MSG_Struct.Gun_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
							SLAVE_VERSION_MSG_Struct.Gun_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
							SLAVE_VERSION_MSG_Struct.Gun_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case LEFTWALKID:
							SI4438DataStruct.leftwalkflg=true;		
							attitude_data.LeftWalk[0]=p_Msg->data[7];//脚部运动状态
							attitude_data.LeftWalk[1]=p_Msg->data[8];//脚部数据
							attitude_data.LeftWalk[2]=p_Msg->data[9];
							gamePadBatteryData[2]=p_Msg->data[11];//脚部电量
							
							dataPayload.Auto_Shutdown_Flg=p_Msg->data[10];//脚部返回的自动关机状态 220225
					
							SLAVE_VERSION_MSG_Struct.Foot_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
							SLAVE_VERSION_MSG_Struct.Foot_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
							SLAVE_VERSION_MSG_Struct.Foot_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case RIGHTWALKID:
							SI4438DataStruct.righttwalkflg=true;	
							attitude_data.RightWalk[0]=p_Msg->data[7];//脚部运动状态		
							attitude_data.RightWalk[1]=p_Msg->data[8];
							attitude_data.RightWalk[2]=p_Msg->data[9];
							gamePadBatteryData[3]=p_Msg->data[11];
							
							dataPayload.Auto_Shutdown_Flg=p_Msg->data[10];

							SLAVE_VERSION_MSG_Struct.Foot_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
							SLAVE_VERSION_MSG_Struct.Foot_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
							SLAVE_VERSION_MSG_Struct.Foot_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case BELTID:
							SI4438DataStruct.beltflg=true;
							attitude_data.QuaternionsData[20]=p_Msg->data[3];//腰部4元素
							attitude_data.QuaternionsData[21]=p_Msg->data[4];
							attitude_data.QuaternionsData[22]=p_Msg->data[5];
							attitude_data.QuaternionsData[23]=p_Msg->data[6];
					
							//运动趋势
							if(p_Msg->data[7])//后退状态
								attitude_data.MotionState=0x20;
							else
								attitude_data.MotionState=0x10;
					
							attitude_data.EulerData[0]=p_Msg->data[8];//腰部欧拉角
							attitude_data.EulerData[1]=p_Msg->data[9];
							attitude_data.EulerData[2]=p_Msg->data[10];
							attitude_data.EulerData[3]=p_Msg->data[11];
							attitude_data.EulerData[4]=p_Msg->data[12];
							attitude_data.EulerData[5]=p_Msg->data[13];
							
							SLAVE_VERSION_MSG_Struct.Back_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
							SLAVE_VERSION_MSG_Struct.Back_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
							SLAVE_VERSION_MSG_Struct.Back_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case SPONTOON:
							SI4438DataStruct.spontoonflg=true;
							dataPayload.key_data[6] = p_Msg->data[7];//警棍状态
//							printf("SPONTOON\r\n");
					
							SLAVE_VERSION_MSG_Struct.Gun_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
							SLAVE_VERSION_MSG_Struct.Gun_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
							SLAVE_VERSION_MSG_Struct.Gun_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case SHIELD:
							SI4438DataStruct.shieldflg=true;
							dataPayload.key_data[7] = p_Msg->data[7];
//							printf("SHIELD\r\n");
//							SLAVE_VERSION_MSG_Struct.Gun_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
//							SLAVE_VERSION_MSG_Struct.Gun_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
//							SLAVE_VERSION_MSG_Struct.Gun_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					case PAPPER_SPRAY:
							SI4438DataStruct.pappersprayflg=true;
							dataPayload.key_data[8] = p_Msg->data[7];
//							printf("PAPPER_SPRAY\r\n");
//							SLAVE_VERSION_MSG_Struct.Gun_Hardware_Version=p_Msg->data[14]<<24 | p_Msg->data[15]<<16 | p_Msg->data[16]<<8 | p_Msg->data[17];
//							SLAVE_VERSION_MSG_Struct.Gun_Soft_Version=p_Msg->data[18]<<24 | p_Msg->data[19]<<16 | p_Msg->data[20]<<8 | p_Msg->data[21];
//							SLAVE_VERSION_MSG_Struct.Gun_Protocol_Version=p_Msg->data[22]<<24 | p_Msg->data[23]<<16 | p_Msg->data[24]<<8 | p_Msg->data[25];
					break;
					default :break;
				}
			}
			else
				memset(&p_Msg,0,sizeof(p_Msg));
		}
		SI446x_Change_Status( RXTUNE_STATE );
		while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
		SI446x_Start_Rx(dataPayload.nrf_channel, 0, PACKET_LENGTH,0,0,3 );
		memset(&p_Msg,0,sizeof(p_Msg));
	}
}



