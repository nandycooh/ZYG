#include "../function/led/rgb_led.h"
#include "../function/spi/drv_spi.h"
#include "../function/time/hal_timer.h"
#include "../function/si4438/drv_SI4438.h"
#include "../function/power/powerManage.h"
#include "../function/peripheral/peripheral.h"
#include "../function/protocol/protocol.h"
#include "../function/recoil/recoil.h"
#include "../function/time/hal_timer.h"
#include "../function/crc/crc.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "hal_flash.h"

ProtocolFormat_t* pP_Instance;				//协议层
extern TIME_ControlDef TIME_ControlStruct;	//定时器封装
extern Msg_t p_Msg;							//发送数据结构体
extern Protocol433_t *Protocol4438;			//接收数据结构体
u8 recoi_mode=0;
static u8 recoicmd=0;

u8 Si4438ChannelSet=0;
extern u32 time_count;
extern u8 recoil_stop;
extern u32 sleep_count;
extern u8 recoil_abnormal;//后座力异常触发
extern u8 bullet_count;
extern uint8_t gun_statusflg[2];
extern uint8_t g_SI4463RxBuffer[64];	//接收数据缓存buffer

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

/*手枪协议任务*/
void Gun_Task(void){
	static uint8_t quat[4] = {0};
	static uint8_t quat1[6] = {0};
			/*自定义协议*/
			pP_Instance->FmtHead();
			pP_Instance->FmtU8(HANDGUNID);								//设备ID
			pP_Instance->FmtHex(quat,4);									//
			pP_Instance->FmtU8(gun_statusflg[0]);						//手枪状态
			pP_Instance->FmtHex(quat1,6);
			
			pP_Instance->FmtHex(GetVerison(),12);//220602
			
			pP_Instance->FmtCrc8();												//校验位
			pP_Instance->FmtU8(FORMAT_TAIL);							//数据尾帧
			p_Msg.length = pP_Instance->GetLength();
			memcpy(p_Msg.data,pP_Instance->GetBuffer(), p_Msg.length);
}

void recoi_E(void)
{
	if(!INSURANCE_BUTTON)
		TIM_Cmd(TIM3,ENABLE);
	else
		TIM_Cmd(TIM3,DISABLE);
}


static u8 lastcanncel;
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;
void RF_SI4438_Ack_Cmd_Data(void)
{
	u8 crcdata;
	
	if(Protocol4438->data[0] == 0x66 && Protocol4438->data[1] == 0x65 && Protocol4438->data[2] == HANDGUNID &&\
			Protocol4438->data[3] ==  Si4438ChannelSet && Protocol4438->data[15]==0x64)
	{
//		printf(" %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\r\n",Protocol4438->data[0],Protocol4438->data[1],Protocol4438->data[2],Protocol4438->data[3],
//			Protocol4438->data[4],Protocol4438->data[5],Protocol4438->data[6],Protocol4438->data[7],Protocol4438->data[8],Protocol4438->data[9],Protocol4438->data[10],Protocol4438->data[11],Protocol4438->data[12],
//			Protocol4438->data[13],Protocol4438->data[14],Protocol4438->data[15]);
		
			crcdata = crc8_maxim(Protocol4438->data,14);//掉线后校验不过导致无法重连
//			printf("%x %x\r\n",Protocol4438->data[14],crcdata);
			if(Protocol4438->data[14] != crcdata)
			{
				memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
				return;
			}
			
			//信道不为0并且不能两次反复设置信道，防止写入flash出意外
			if(Protocol4438->data[5]!=0 && lastcanncel != Protocol4438->data[5])
			{
				Si4438ChannelSet=Protocol4438->data[5];
				lastcanncel=Protocol4438->data[5];//保存上一次的信道
				flash_config.pair_device.pair_ch=Si4438ChannelSet;//保存信道到flash
				GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));
				SI446x_Init();
				printf("GChannelSet=%d\r\n",Si4438ChannelSet);
			}
			
			if(recoicmd)
			{
				switch(Protocol4438->data[13])
				{
					case 0x03:
						LED_ON;
						if(recoil_stop==1)//有扳机触发才产生后座力201111
						{
							recoil_stop=0;
							bullet_count=1;TIM_Cmd(TIM3,ENABLE);
						}
				break;
					default:
						LED_OFF;
					
						if(!bullet_count)//等待套筒执行完毕 以防复位
						{
							TIM_Cmd(TIM3,DISABLE);time_count = 0;
						}break;
				}
			}
			
			RF433_SendData(&p_Msg);//发送数据
			
			memset(&Protocol4438,0,sizeof(Protocol4438));//清空接收数据
		}
		else
			Si4438_Rcecv_Buff();//接收数据处理
}

/**
  * @brief :主函数 
  * @param :无
  * @note  :无
  * @retval:无
***/ 
int main( void )
{					
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init( );									//延时初始化
	uart_init( 115200 );							//串口1初始化 调试使用
	Led_Init();										//LED初始化
	Ziyuan_GunState_Init();							//枪初始化
	Ziyuan_Touch_Init();							//触摸初始化
	Recoil_Init();									//后坐力部分初始化
	
	GFLASH.Read(0,(u8*)&flash_config,sizeof(flash_config));//读取离线数据
	if(flash_config.pair_device.pair_ch != 0xff)
		Si4438ChannelSet=flash_config.pair_device.pair_ch;
	else
		Si4438ChannelSet=0;
	printf("Si4438ChannelSet=%d\r\n",Si4438ChannelSet);
	
	//SI4438初始化
	SI44xx_Power_Init();
	delay_ms(1000);//等待充电完成
	SI446x_Init();				
	
	TIME_ControlStruct.start(1,1000);				//初始化定时器2 1S 		后座力持续通电计数
	TIME_ControlStruct.start(2,1);					//初始化定时器4 1ms 	扳机消抖计数 睡眠计数
	
	pP_Instance = SendProtocolGetObj();				//实例化协议层
	
	while(1)
	{
		Gun_All_Status();							//手枪触发状态获取
		Gun_Task();									//协议封装
		RF_SI4438_Ack_Cmd_Data();					//数据应答
		
		if(!DOWN_CLIP_BUTTON && !REST_BUTTON)		//组合键-后座力控制模式切换
		{
			delay_ms(10);
			if(!DOWN_CLIP_BUTTON && !REST_BUTTON)
			{
				recoi_mode=(!recoi_mode);
				while(1)
				{
					LED_PIN_PORT=!LED_PIN_PORT;
					delay_ms(100);
					
					if(DOWN_CLIP_BUTTON && REST_BUTTON)
						break;
				}
			}				
		}
		
		if(recoi_mode)							//后座力控制方式：本地 or SDK指令
		{
			recoi_E();
			recoicmd=0;
		}
		else
			recoicmd=1;
			
		
		/*持续上膛异常情况处理*/
		if(CHAMBERING_BUTTON)						//检测到上膛  后座力持续异常清零
		{
			TIM_Cmd(TIM2,ENABLE);
			if(recoil_abnormal>=2)
			{
				TIM1->CCER = 0;//清空
				TIM_Cmd(TIM1,DISABLE);
			}	
		}
		else
		{
			TIM_Cmd(TIM2,DISABLE);
			recoil_abnormal=0;
		}
		
		/*休眠部分功能*/	
		if(sleep_count>10*1000*60)					//10分钟无手持自动休眠 手持时自动唤醒
		{
			sleep_count=0;
			Enter_Sleep();
		}
	}
}



