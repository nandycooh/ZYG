#include "../function/led/led.h"
#include "../function/spi/hal_spi.h"
#include "../function/si4438/drv_SI4438.h"
#include "../function/protocol/protocol.h"
#include "delay.h"
#include "usart.h"
#include "power.h"
#include "timer1.h"
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "USART2.h"
#include "main.h"
#include "../function/crc/crc.h"
#include "hal_flash.h"

#define TIM_arr 999
#define TIM_psc 71
#define LEVEL_0  320    //0级震动强度，防止传0级时，电机无震动

ProtocolFormat_t* pP_Instance;				//协议层

extern R485_t Recv_485_Msg;//接收腰带数据缓存
extern Protocol433_t *Protocol4438;
extern Msg_t p_Msg;

static u8 connectstas;
extern uint8_t g_SI4463RxBuffer[64];	//接收数据缓存buffer

u8 shakeData[54];
u8 shakeDataLen;
u8 Si4438ChannelSet=0;

//static u8 lastcanncel;
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;

/*
tim1:ch1头部前额，ch2枕部，  ch3左胸，  ch4右胸
tim2:ch1左肋，    ch2右肋，  ch3左腹，  ch4右腹
tim3:ch1左背，    ch2右背，  ch3左后腰，ch4右后腰
tim4:ch1左大臂，  ch2左小臂，ch3左手，  ch4右大臂
tim5:ch1右小臂，  ch2右手，  ch3左大腿，ch4左小腿
tim8:ch1左脚，    ch2右大腿，ch3右小腿，ch4右脚
 */
struct shakeType
Head, Occiput, Chest_L, Chest_R, Rib_L, Rib_R, Belly_L, Belly_R,
Back_L, Back_R, Waist_L, Waist_R, Arm_L, Wrist_L, Hand_L, 
Arm_R, Wrist_R, Hand_R, Thigh_L, Shank_L, Foot_L, 
Thigh_R, Shank_R, Foot_R, Shoulder_L, Shoulder_R;

void shakeData_Analysis(void)
{
    u8 location;
    u8 NUM;
    
    NUM = shakeDataLen / 2;//接收到协议为2字节 第一个字节为位置和强度 第二个字节为时间211207
    for(u8 i = 0; i < NUM; i++)
    {
        location = shakeData[i * 2] & 0xF8;
        location = location >> 3;
		
		switch(location)
        {
#ifdef  SHOULDER
            case 1://左肩
                Shoulder_L.shakeStrength = shakeData[i * 2] & 0x07;//振动强度
                Shoulder_L.shakeTime = (shakeData[i * 2 + 1] & 0x1F);//持续时间
                Shoulder_L.shakeTime = Shoulder_L.shakeTime * 100;//实际持续时间
                SHOULDER_L_TIM = Shoulder_L.shakeStrength * 60 + LEVEL_0;
                break;
            case 2://右肩
                Shoulder_R.shakeStrength = shakeData[i * 2] & 0x07;
                Shoulder_R.shakeTime = (shakeData[i * 2 + 1] & 0x1F);
                Shoulder_R.shakeTime = Shoulder_R.shakeTime * 100;
                SHOULDER_R_TIM = Shoulder_R.shakeStrength * 60 + LEVEL_0;
                break;
#endif
            
#ifdef CHEST
            case 3://左胸Chest_L
                Chest_L.shakeStrength = (shakeData[i * 2] & 0x07);  //接收到上位机下传的震动强度数值
                Chest_L.shakeTime = (shakeData[i * 2 + 1] & 0x1F);  //接收到上位机下传的震动时间数值
                Chest_L.shakeTime = Chest_L.shakeTime * 100;   //震动时间扩大100倍，单位ms
                CHEST_L_TIM = Chest_L.shakeStrength * 60 + LEVEL_0; //震动强度扩大60倍再加上520
                                                                //加上520是为了避免震动强度过小，没有震动效果，可修改520
                break;
            case 4://右胸
                Chest_R.shakeStrength = (shakeData[i * 2] & 0x07);
                Chest_R.shakeTime = (shakeData[i * 2 + 1] & 0x1F);
                Chest_R.shakeTime = Chest_R.shakeTime * 100;
                CHEST_R_TIM = Chest_R.shakeStrength * 60 + LEVEL_0;
                break;
#endif
#ifdef BACK
            case 9://左背Back_L
                Back_L.shakeStrength = (shakeData[i * 2] & 0x07);
                Back_L.shakeTime = (shakeData[i * 2 + 1] & 0x1F);
                Back_L.shakeTime = Back_L.shakeTime * 100;
                BACK_L_TIM = Back_L.shakeStrength * 60 + LEVEL_0;
                break;
#endif
            default:
                break;
        }   
    }
}

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

/**************************************************************************/
void SI443x_dataHandle(void)
{
    shakeDataLen = Protocol4438->data[8];//数据长度
    memcpy(shakeData, &Protocol4438->data[10], shakeDataLen);
}

/*手枪协议任务*/
void Gun_Task(void){
			/*自定义协议*/
			pP_Instance->FmtHead();
			pP_Instance->FmtU8(BELTID);								//设备ID
			pP_Instance->FmtHex(Recv_485_Msg.belt,4);					//腰带数据
			pP_Instance->FmtU8(Recv_485_Msg.back);						//后退状态 1-后退
			pP_Instance->FmtHex(Recv_485_Msg.data,6);					//腰带xyz
			
			pP_Instance->FmtHex(GetVerison(),12);//220602
	
			pP_Instance->FmtCrc8();												//校验位
			pP_Instance->FmtU8(FORMAT_TAIL);							//数据尾帧
			p_Msg.length = pP_Instance->GetLength();
			memcpy(p_Msg.data,pP_Instance->GetBuffer(), p_Msg.length);
	
//			printf("%x %x %x %x-%x-%x %x %x %x %x %x\r\n",Recv_485_Msg.belt[0],Recv_485_Msg.belt[1],Recv_485_Msg.belt[2],Recv_485_Msg.belt[3],Recv_485_Msg.back,
//					Recv_485_Msg.data[0],Recv_485_Msg.data[1],Recv_485_Msg.data[2],\
//					Recv_485_Msg.data[3],Recv_485_Msg.data[4],Recv_485_Msg.data[5]);
}

void RF_SI4438_Ack_Cmd_Data(void)
{
	static u16 Reoil_Cont;
	u8 crcdata;

	if(Protocol4438->data[0] == 0x66 && Protocol4438->data[1] == 0x65 && Protocol4438->data[2] == BELTID &&\
			Protocol4438->data[3] ==  Si4438ChannelSet && Protocol4438->data[15]==0x64)
	{
		crcdata = crc8_maxim(Protocol4438->data,14);
		
		if(Protocol4438->data[14] != crcdata)
		{
			memset(g_SI4463RxBuffer,0,sizeof(g_SI4463RxBuffer));
			return;
		}
		
		if(Protocol4438->data[7]!=0)
		{
			Si4438ChannelSet=Protocol4438->data[7];
			
			flash_config.pair_device.pair_ch=Si4438ChannelSet;//保存信道到flash
			GFLASH.Write(0,(u8*)&flash_config,sizeof(flash_config));
			SI446x_Init();
			printf("GChannelSet=%d\r\n",Si4438ChannelSet);
		}
			
		Reoil_Cont=0;
		connectstas=1;
		

		RF433_SendData(&p_Msg);//发送数据
		memset(p_Msg.data,0,sizeof(p_Msg.data));//清空发送数据
		p_Msg.ID=0x00;
		memset(Protocol4438,0,sizeof(Protocol433_t));//清空接收数据
	}
	else if(Protocol4438->data[0] == 0x99 && Protocol4438->data[1] == 0x33)
	{
		SI443x_dataHandle();
		memset(Protocol4438,0,sizeof(Protocol433_t));
		shakeData_Analysis(); //根据震动数据配置pwm
	}
	else
	{
		Si4438_Rcecv_Buff();//接收数据处理
		Reoil_Cont++;
		if(Reoil_Cont>2000)
		{
			Reoil_Cont=0;
			connectstas=0;
		}
	}
}

/**
  * @brief :主函数 
  * @param :无
  * @note  :无
  * @retval:无
***/ 
int main( void )
{
	delay_init( );																	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
														
	Power_Init();//板子电源及无线电源初始化
	delay_ms(1000);
	uart_init( 115200 );	//串口3初始化 调试使用
	Initial_UART2(115200);
	delay_ms(1000);		
	Led_Init();
	delay_ms(1000);		
	POWER_PIN_STATE=1;															//总成板上电
	WHITE_LED_ON;																		//开机成功
	
	GFLASH.Read(0,(u8*)&flash_config,sizeof(flash_config));//读取离线数据
	if(flash_config.pair_device.pair_ch != 0xff)
		Si4438ChannelSet=flash_config.pair_device.pair_ch;
	else
		Si4438ChannelSet=0;
	printf("Si4438ChannelSet=%d\r\n",Si4438ChannelSet);
	
	SI485_POWER_PIN_STATE=0;												//485通电
	SI4438_POWER_PIN_STATE=0;												//4438通电
	delay_ms(1000);																//等待4438充电
	SI446x_Init();																	//SI4438初始化
	pP_Instance = SendProtocolGetObj();							//实例化协议层
	//初始化pwm
    Timer1_Init(TIM_arr, TIM_psc);
    Timer3_Init(TIM_arr, TIM_psc);
    Timer4_Init(TIM_arr, TIM_psc);
	//初始化定时器（计算震动时长）
	Timer2_Init(TIM_arr, TIM_psc);
    srand(113);
	
	while(1)
	{
		Gun_Task();																		//协议封装
		RF_SI4438_Ack_Cmd_Data();											//数据应答

		if(Power_Shut_Down(30))//长按3S关机
		{
			delay_ms(100);
		}
		else if(connectstas)
		{
			BLUE_LED_ON;
		}
		else
		{
			WHITE_LED_ON;
		}
	}
}

