#include "delay.h"
#include "usart.h"
#include "rgb_led.h"
#include "hal_spi.h"
#include "hal_adc.h"
#include "hal_timer.h"
#include "drv_SI4438.h"
#include "powerManage.h"
#include "protocol.h"
#include "hal_timer.h"
#include "walk.h"
#include "mpu.h"
#include "mpu_6050.h"
#include "sleep.h"
#include "hal_mpu.h"
#include "func_init.h"
#include "crc.h"
#include <stdio.h>
#include "math.h"
#include "hal_flash.h"

#define TEST_SI4438_FLAG
#define TEST_SLEEP_FLAG

HAL_adapter_t* pH_Instance;						//6050硬件层
FUNC_adapter_t* pF_Instance;					//MPU功能层
ProtocolFormat_t* pP_Instance;					//协议层
VoidGear_Walk_t* pW_Instance;					//脚步核心算法
Event_Flag_t p_Flag;							//标志位

typedef enum
{
	ON = 0,
	OFF,
}Work_Status;

static uint8_t BatteryQuantity = 0;		//电量
static uint8_t eliminationCount = 0;	//休眠计数
extern int connetflg;					//未通信计数
extern u8 dgbugcount;					//信息输出计数
extern u8 Low_Battery_Flg;

u8 Si4438ChannelSet=0;

//电源及低功耗模式结构体及指针申明
extern HalAdapter_Management_Power_t halAdapter_Power;
extern HalAdapter_Management_OperatingMode_t halAdapter_OptMode;
extern Msg_t p_Msg;//发送数据
extern LED_DRIVER_APP_API Led_Driver_App_Port;//API接口供外部使用

/************************************************定时器任务**************************************************/
//充电接口状态判断
void UpdateChargingInterfaceStatus(void)
{
	static uint8_t chargingInterfaceStatus = 0;
	chargingInterfaceStatus <<= 1;
	if(halAdapter_Power.HAL_Get_ChargePinStatus() == true)//1：充电口插入
	{
		chargingInterfaceStatus |= 1;
	} 
	else{
		chargingInterfaceStatus = 0;
	}
	
	if(chargingInterfaceStatus == 0xFF){
		p_Flag.is_ConnectChargingInterface = true;
	}
	else{
		p_Flag.is_ConnectChargingInterface = false;
	}
}

//低电量判断
void UpdateBatteryQuantity(void)
{
	static uint8_t BatteryTCount = 0;
	static uint8_t BatteryFCount = 0;				//由于电量获取不稳定，使用两个变量消抖210712
	
	BatteryQuantity = halAdapter_Power.HAL_Get_battery_power();//获取电池电量
	
	if(BatteryQuantity <= 20)
	{
		BatteryTCount++;											//低电量 消抖
		BatteryFCount=0;
		
		if(BatteryTCount > 10)
		{
			printf("p_Flag.is_LowBattery = true\r\n");
			p_Flag.is_LowBattery = true;				//电量低
			if(BatteryQuantity == 1){
				POWER_ON_OFF=0;//关机
			}
		}
	}
	else{
		BatteryTCount = 0;
		BatteryFCount++;											//非低电量 消抖
		if(BatteryFCount > 10)
		{
			p_Flag.is_LowBattery = false;				//清空标志位
		}
	}
}

//静止关机
void DormancyState_Judge(void)
{
	static uint32_t stateCount = 0;
	if(pW_Instance->GetWalk() == 0)					//获取步行标志位 0静止 1运动
	{
		stateCount++;
		if(stateCount == 10*10*60)						//10分钟进休眠 -->定时器100ms
		{
//			p_Flag.is_WaitingStartUp = true;		//睡眠标志位
//			POWER_ON_OFF=0;
//			/*关机*/
			if(p_Flag.is_TurnOffFlag==true)
				POWER_ON_OFF=0;
		}
	}
	else{
		if(++eliminationCount > 50){
			stateCount = 0;
//			p_Flag.is_WaitingStartUp = false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
/*充电任务*/
void Charge_Task(void)
{
	ApplicationTimer_Stop();
	delay_ms(200);
	pF_Instance->MPU->LpMotionInterrupt(MOTION_DETECTION);
	Adc_ShutDown();//开关ADC
	GPIO_SetBits(GPIOA, GPIO_Pin_3);//4438电源断
	while(halAdapter_Power.HAL_Get_ChargePinStatus()==true)//是否在充电
	{	
		if(halAdapter_Power.HAL_Get_FullBatteryPin_InCharge()==true)//是否充满 判断芯片方式（涓流充电保护电池）
		{
			GREEN_LED_ON;
			POWER_ON_OFF=0;
		}
		else
		{
			RED_LED_ON;
			halAdapter_OptMode.HAL_EnterStop();//进入停止模
		}		
	}
//	p_Flag.is_ConnectChargingInterface = halAdapter_Power.HAL_Get_ChargePinStatus();//获取是否在充电IO状态
}

/*LED任务*/
void LED_Task(void)
{	
	if(p_Flag.is_UpDateLED == true)
	{
		if(Power_Shut_Down(30))//长按3S关机
		{
			delay_ms(100);
		}
		else if(p_Flag.is_LowBattery == true)
		{
//			printf("enter LowPower\r\n");
			if(Low_Battery_Flg>=10){
				Low_Battery_Flg=0;
				Led_Driver_App_Port.App_LedTurnState(RED);//red闪烁 1S/次
			}
		}
		else
		{
			Low_Battery_Flg=0;
			
		
			if(p_Flag.is_ConnectTerminal == true)//连接蓝灯
			{
				BLUE_LED_ON;
			}
			else
			{
				WHITE_LED_ON;
			}
		}
	}
}

u8 SI433_ID=0x00;
u8 Batons_State;
/*步行计算任务*/
void Walk_Task(void){
	unsigned long timestamp;
	unsigned char sensors, more;
	short accel[3] = {0}, gyro[3] = {0};
	static uint8_t quat[4] = {0};
	
	
	if(TOUCH_GPIO_STATE)
	{
		GREEN_LED_ON;
		Batons_State |= 0x01; 
	}
	else
		Batons_State &= (~0x01);
	
	if(pW_Instance->GetWalk())
	{
		RED_LED_ON;
		Batons_State |= 0x02;
	}
	else
		Batons_State &= (~0x02);
	
	if(pF_Instance->MPU->isGenerateNewData()) 									//从寄存器获取温度
	{
		pF_Instance->get_timestamp(&timestamp);										//获取时间
		if(!pF_Instance->MPU->ReadFIFO(gyro, accel, &timestamp, &sensors, &more))//从FIFO获取一个数据包
		{	
			pW_Instance->Algorithm_Walk(gyro,accel);								//数据算法
			/*自定义协议*/
			pP_Instance->FmtHead();
			pP_Instance->FmtU8(SI433_ID);														//设备ID
			pP_Instance->FmtHex(quat,4);														//四元数
			pP_Instance->FmtU8(Batons_State);												//警棍状态
			pP_Instance->FmtU16((uint16_t)pW_Instance->GetSpeed());	//步行速度
			pP_Instance->FmtU8(0);																	//精度
			pP_Instance->FmtU8(BatteryQuantity);										//电池电量
			pP_Instance->FmtU8(0);
			pP_Instance->FmtU8(0);
			
			pP_Instance->FmtHex(GetVerison(),12);//220125
			
			pP_Instance->FmtCrc8();																	//校验位
			pP_Instance->FmtU8(FORMAT_TAIL);												//数据尾帧
			p_Msg.length = pP_Instance->GetLength();
			memcpy(p_Msg.data,pP_Instance->GetBuffer(), p_Msg.length);
		}
	}
}
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;
/**********************************************************************
  * @brief :主函数 
  * @param :无
  * @note  :无
  * @retval:无
*************************************************************************/ 
int main( void )
{
//	uint8_t batval = 0;															//电量判断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	halAdapter_Power.HAL_Power_Init();							//充电部分初始化	
	delay_init( );//延时初始化
																	
	delay_ms(1000);
	uart_init(115200);															//串口初始化 调试使用
	delay_ms(1000);
	
	delay_ms(1000);
	POWER_ON_OFF=1;
	Led_Init();																			//LED初始化
	Touch_Gpio_Init();
	Adc_init();															//电量获取初始化
	pW_Instance = WalkGetObj();											//步行算法实例化
	pW_Instance->Init(AL_SAMPLE_RATE);
	pP_Instance = SendProtocolGetObj();							//实例化协议层
	halAdapter_Clock.HAL_ClockInit();								//实例化硬件层
	halAdapter_Clock.isInit = true;
	pF_Instance = GetFuncAdapter();									//实例化功能层
	pF_Instance->MPU->Init(AL_SAMPLE_RATE);					//功能配置
	
	TOUCH_GPIO_POWER=1;
	
	GFLASH.Read(0,(u8*)&flash_config,sizeof(flash_config));//读取离线数据
	if(flash_config.pair_device.pair_ch != 0xff)
		Si4438ChannelSet=flash_config.pair_device.pair_ch;
	else
		Si4438ChannelSet=0;
	
	if(flash_config.pair_device.device_num == SPONTOON ||
		flash_config.pair_device.device_num == SHIELD  || flash_config.pair_device.device_num == PAPPER_SPRAY)
		SI433_ID=flash_config.pair_device.device_num;
	else
		SI433_ID=SPONTOON;
	
#ifdef	TEST_SI4438_FLAG
	SI446x_Init();
#endif
	
	timer_start(1,100);//定时器2初始化100ms 充电接口、低电量和关机计时
	timer_start(2,100);//定时器3初始化100ms 信息调试输出计时
	Vibrate_Gpio_Init();
	
	p_Flag.is_UpDateLED = true;
	
	while(1)
	{	
		if(p_Flag.is_ConnectChargingInterface == true)	//定时器任务
		{
			Charge_Task();							/*充电状态任务*/
		}
		else
		{
			Walk_Task();						/*更新MPU数据*/
#ifdef	TEST_SI4438_FLAG
			Si4438_Rcecv_Buff();
#endif
			LED_Task();
				
			if(dgbugcount>=20)//2S钟一次
			{
//				printf("%d\r\n",Batons_State);
//				
				dgbugcount=0;
				printf("Batons_State:%d-speed:%f-channel:%dR-%x\r\n",Batons_State,pW_Instance->GetSpeed(),Si4438ChannelSet,p_Flag.is_TurnOffFlag);
			}
		}
	}
}



