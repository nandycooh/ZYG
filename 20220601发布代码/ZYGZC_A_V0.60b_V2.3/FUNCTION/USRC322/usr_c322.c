#include "usr_c322.h"
#include "attitude.h"
#include "FreeRTOS.h"
#include "task.h"
#include "attitude.h"

static u8 rcv_byte = 0;
static u8 rcv_byte_last = 0;
static int rx_num = 0;
u8 rx_buf[RX_NUM_SIZE];//串口3接受buff
u8 rx_len=0;
static u8 Version[48];
u8 BoardConnectFlg = 0;/*当前主板的连接状态*/
u8 NewChannel;
BroadCastData_t BroadCastData;/*广播数据上传*/
LoginOrBoardCastAckData_t  LoginOrBoardCastAckData;/*广播或者登录返回数据*/
funtionFlag_t  funtionFlag;/*功能标志位*/

extern MacDataDef Macstruct;
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;
extern HalAdapter_Management_Led_t halAdapter_Led;//API接口供外部使用
extern TaskHandle_t SI433Task_Handler;					//任务句柄
extern DataPayload_t dataPayload;
extern Protocol433_t Protocol433;

extern void Restart(void);

/***********************************************************************************************************************************
																											硬件驱动层
***********************************************************************************************************************************/
/************************************************************  	
函数名称：	Use_C322_Init
函数功能：	WIFI 引脚初始化
入口参数：	bound：波特率
返回参数：	无
说明：	    无
***********************************************************/ 
void RS232_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);	//使能USART1，GPIOA时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口2
}

/************************************************************  	
函数名称：	Usart_SendByte
函数功能：	发送一个字节
入口参数：	void
返回参数：	无
说明：	    无
***********************************************************/ 
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/************************************************************  	
函数名称：	Usart_SendString
函数功能：	发送字符串
入口参数：	void
返回参数：	无
说明：	    无
***********************************************************/ 
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/************************************************************  	
函数名称：	usartx_puchar
函数功能：	发送字符串
入口参数：	USARTX    : USART1 | USART2| USART3   (选择串口)
            buff      :"sadsad"| str              (字符串或数组首地址)
            len       :strlen(str)                (字符串或数组长度)
返回参数：	无
说明：	    无
***********************************************************/ 
void usartx_puchar(USART_TypeDef * USARTX,u8 *buff, u32 len)
{  
	USARTX->SR &=~(1<<6);
	while(len--)
	{
		USARTX->DR = *(buff++);
		while(!(USARTX->SR & (1<<6)));
	}
}


void USART1_IRQHandler(void)//210417
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		rcv_byte_last = rcv_byte;//上一次接收字节
		rcv_byte =USART_ReceiveData(USART1);	//读取接收到的数据
		
		rx_buf[rx_num++] = rcv_byte;
		
		if(rcv_byte_last==0x99 && rcv_byte==0x33)//接收上位机下发协议
		{
		  rx_buf[0] = rcv_byte_last;
		  rx_buf[1] = rcv_byte;
		  rx_num    = 2;			
		}
		
		//根据每次接收数据长度判断该数据是否接收完毕
		rx_len=rx_buf[8]+0x0C;//数据长度+12字节其他数据（头+版本号+包序号+ACK+数据长度1字节+命令+CRC）·
//		if(rx_num >= RX_NUM_SIZE || (rx_buf[rx_num-2] == 0x0D && rx_buf[rx_num-1] == 0x0A) || rx_num>=rx_len) //上位机下发帧长为12字节
//    {       
////			rcv_byte=0;
//			rx_num = 0;
//			rx_len=0;
//    }
	
			if( rx_num>=rx_len) //上位机下发帧长为12字节
			{       
		//			rcv_byte=0;
					rx_num = 0;
					rx_len=0;
			}
	
	}
}

/***********************************************************************************************************************************
																											  功能模块层
***********************************************************************************************************************************/

//清楚串口接收的字符串
void WIFI_Clear(void)
{
	memset(rx_buf, 0, sizeof(rx_buf));
	rx_num = 0;
}

//	入口参数：	cmd：命令
//	res：需要检查的返回指令
//	返回参数：	1-成功	0-失败
u8 WIFI_Send_Cmd_Check(u8 *cmd, u8 *res)
{
	unsigned char timeOut = 50;
	WIFI_Clear();
	
	usartx_puchar(USART2, (u8*)cmd, strlen((const char *)cmd));   //串口2发送AT指令的

	while(timeOut--)
	{
		if(rx_num > 0)
		{						             	
//			usartx_puchar(USART3, (u8*)rx_buf, strlen((const char *)rx_buf));//如果收到数据
			if(strstr((const char *)rx_buf, (const char *)res) != NULL)
			{								                      //清空缓存			
				return 1;
			}
			WIFI_Clear();	
		}
		delay_ms(10);
	}
		 
	return 0;
}


//wifi接收的数据是否为空
//ptr：返回的数据
//		0-校验失败
int WIFI_Cmd_Check(char *ptr)
{
	if(strstr((char*)rx_buf,ptr)!= NULL)
		return 1;
	else 
		return 0;
}

/**
  * @brief 心跳包处理
  * @param
  * @retval
**/
//void HeartCheckCallBack(void)
//{	
//	if(BoardConnectFlg==1)//连接状态
//	{
//		Rebootcnt=0;
//		BoardConnectFlg=0;
//		BLUE_LED_ON;
//	}
//	else
//	{
//		Rebootcnt++;
//		//1分钟亮白灯
//		if(Rebootcnt >= 30*100)
//		{
//			WHITE_LED_ON;
//			/*清除板子的连接状态*/
//			BoardConnectFlg = 0;
//		}
//	}
//}

/**
  * @brief 获取单片机MAC地址
  * @param
  * @retval: None
*/
u8* Get_MCU_MAC(void)
{
	static u8 ID[12];
	static u32 UID_ADDR = 0x1FFFF7E8;
	for(u8 i=0;i<12;i++) {
		 ID[i] = (*(__IO u32 *)(UID_ADDR+(i/4)*4) >>((i%4)*8))&0x000000ff;
	}
	return ID;
}
/**
  * @brief wifi mac 获取
  * @param
  * @retval
**/
uint8_t* Wifi_Get_Mac(void)
{
	static uint8_t MAC[6];
	unsigned char decrypt[16];
	MD5_CTX md5;
  MD5Init(&md5);
	u8* pMac = Get_MCU_MAC();
	MD5Update(&md5,pMac,strlen((char *)pMac));
	MD5Final(&md5,decrypt);
	
	//16位MD5
	memcpy(MAC,&decrypt[4],6);
	
	return MAC;
}
extern SLAVE_VERSION_MSG SLAVE_VERSION_MSG_Struct;
u8* GetVerison(void){
	int i = 0;
	//总成板
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
	Version[i++] = 	 BYTE0(temp);
	//脚部
	temp = SLAVE_VERSION_MSG_Struct.Foot_Hardware_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Foot_Soft_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Foot_Protocol_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	//手枪
	temp = SLAVE_VERSION_MSG_Struct.Gun_Hardware_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Gun_Soft_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Gun_Protocol_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	//背心
	temp = SLAVE_VERSION_MSG_Struct.Back_Hardware_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Back_Soft_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i++] = 	 BYTE0(temp);
	temp = SLAVE_VERSION_MSG_Struct.Back_Protocol_Version;
	Version[i++] = BYTE3(temp);
	Version[i++] = BYTE2(temp);
	Version[i++] = BYTE1(temp);
	Version[i] = 	 BYTE0(temp);
	return Version;
}

/***********************************************************************************************************************************
																											  应用层
***********************************************************************************************************************************/
void Get_Mcu_Mac(void)
{
	/*得到模块的MAC地址 和得到入网后的IP地址*/
	memcpy(BroadCastData.MAC,Wifi_Get_Mac(),6);//获取设备ID

	memcpy(Macstruct.MAC,BroadCastData.MAC,6);//备份设备ID 210708
	
	/*得到wifi的IP以及MAC地址*/
	BroadCastData.port = CLIENT_PORT;
	
	debug_printf_hex(16,"MAC:"," ",6,BroadCastData.MAC,"\r\n");
}

/**
  * @brief 	与上位机建立连接
  * @param  
  * @retval
**/
u8 Login_Or_Boardcast(uint8_t BoardcastMode)
{
	/*等待上位机的应答*/
	while(!funtionFlag.IsLoginOrBoardCastAck)//上位机应答标志 不应答一直等待
	{
		/*登录时带IP地址以及MAC  有线版本无IP*/
		protocol_sendData((u8*)&BroadCastData,sizeof(BroadCastData),LOGIN_DATA_CMD);
		
		halAdapter_Led.HAL_LedBlueTurn();

		delay_ms(BOARD_CAST_CYCLE);//广播时间间隔
	}
	
	/*保存到flash中*/
	/*上位机 给到的正确IP地址*/
	memcpy(flash_config.servercofig.port,SERVER_PORT,sizeof(SERVER_PORT));
	
	GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));
	
	return 1;
}

/**
  * @brief  wifi_DataCallBack 数据回调函数
  * @param
  * @retval
**/
void DataCallBack_WIFI(uint8_t* Data,uint16_t Len)
{
	static u16 crc16;
	static u8 receiveBuf[128];
	static u8 *p = NULL;
	if(Len < 10)
		return ;
	u16 receiveLen = 0;
	
	/*处理上位机返回的消息*/
	/*首先判断帧头*/
	if(Data[0] == 0x99 && Data[1] == 0x33)
	{
		Len=Data[8]+0x0C;//数据长度+12字节其他数据（头+版本号+包序号+ACD+数据长度1字节+命令+CRC）
//		debug_printf_hex(16,"cmd:"," ",Len,Data,"\r\n");
		/*CRC校验*/
		/*CRC一定要过*/
		crc16= Message_count_CRC_2(Len-2,Data);
		p = (u8*)&crc16;
		if(Data[Len-1]!= p[1] && Data[Len-2] != p[0])
			return;
		/*得到数据长度*/
		receiveLen = Data[8];
		/*获取数据区*/
		if(receiveLen < 128)
			memcpy(receiveBuf,Data+10,receiveLen);

		/*判断命令*/
		switch(Data[9])
		{
			case 0x01://建立连接有心跳				
				if(funtionFlag.IsWorkMode == 0)
				{
					/*广播返回或者登录返回,服务器会下发登录的密钥以及IP和MAC*/
					funtionFlag.IsLoginOrBoardCastAck = 0x01;
					
					memcpy((u8*)&LoginOrBoardCastAckData,receiveBuf,receiveLen);
					/*工作模式标志位直位*/
					funtionFlag.IsWorkMode = 1;
				}
				/*连接状态*/
				BoardConnectFlg = 1;
				
				funtionFlag.IsWorkMode = 1;
//				/*无心跳计数清零*/
//				Rebootcnt = 0;	
				break;
			case 0x05:
				vTaskSuspend(SI433Task_Handler);
				delay_xms(50);
				/*背心电机处理(力反馈)*/
				SI446x_Send_Packet(dataPayload.nrf_channel, 0x80,(uint8_t *)Data, Len);
				delay_xms(50);
				vTaskResume(SI433Task_Handler );	
				break;			
			case 0x06:
				/*广播返回或者登录返回,服务器会下发登录的密钥以及IP和MAC*/
				funtionFlag.IsLoginOrBoardCastAck = 0x01;
				
				memcpy((u8*)&LoginOrBoardCastAckData,receiveBuf,receiveLen);
				/*工作模式标志位直位*/
				funtionFlag.IsWorkMode = 1;BLUE_LED_ON;
				break;
			case 0x07://设置信道
				funtionFlag.IsSetChannel=true;
			
				if(receiveBuf[0]<255)
					Protocol433.data[4]=receiveBuf[0];
				if(receiveBuf[2]<255)
					Protocol433.data[5]=receiveBuf[2];
				if(receiveBuf[3]<255)
					Protocol433.data[6]=receiveBuf[3];
				if(receiveBuf[4]<255)
					Protocol433.data[7]=receiveBuf[4];
				if(receiveBuf[1]<255)
					NewChannel=receiveBuf[1];//得到信道
				
//				printf("->> %x %d %d %d %d\r\n",Protocol433.data[4],NewChannel,Protocol433.data[5],Protocol433.data[6],Protocol433.data[7]);
				break;
			case 0x08://重启设备
				
				if(receiveBuf[10] == 0x01){
					Restart();//总成板重启
				}
				else if(receiveBuf[10] == 0x00){
					protocol_sendData(GetVerison(),48,VERSION_DATA_CMD);
				}
				else if(receiveBuf[10] == 0x03){
					Protocol433.data[13]=0x03;
				}
				else if(receiveBuf[10] == 0x02){
					Protocol433.data[12]=0x01;//开启脚部自动关机
				}
				else if(receiveBuf[10] == 0x04){
					Protocol433.data[12]=0x00;//关闭脚部自动关机
				}
				printf("Usart recv:%x %x\r\n",receiveBuf[10],Protocol433.data[8]);
				memset(receiveBuf,0,sizeof(receiveBuf));
				memset(&SLAVE_VERSION_MSG_Struct,0,sizeof(SLAVE_VERSION_MSG_Struct));
				break;
			default:break;
		}		
	}

	memset(rx_buf,0,sizeof(rx_buf));//210930
}

