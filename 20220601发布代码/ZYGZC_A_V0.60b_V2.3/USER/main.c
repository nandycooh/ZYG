#include "delay.h"
#include "drv_spi.h"
#include "drv_SI4438.h"
#include "FreeRTOS.h"
#include "task.h"
#include "power.h"
#include "usr_c322.h"
#include "led.h"
#include "hal_timer.h"
#include "attitude.h"
#include "motor.h"
#include "usart.h"	
#include "emserial.h"

#define START_TASK_PRIO		1							//任务优先级 越大优先级越高（与中断优先级相反）
#define START_STK_SIZE 		128  					//任务堆栈大小 因为是u16，所有大小应该是256个字节
TaskHandle_t StartTask_Handler;					//任务句柄
void start_task(void *pvParameters);		//任务函数

#define WIFIRECVE_TASK_PRIO		2					//任务优先级
#define WIFIRECVE_STK_SIZE 		128				//任务堆栈大小  
TaskHandle_t WIFIRECVETask_Handler;			//任务句柄

void wifirecve_task(void *pvParameters);//任务函数
#define MASTER_TASK_PRIO		3						//任务优先级 
#define MASTER_STK_SIZE 		128					//任务堆栈大小  
TaskHandle_t MASTERTask_Handler;				//任务句柄
void master_task(void *pvParameters);		//任务函数

#define SI433ACK_TASK_PRIO		4					//任务优先级
#define SI433ACK_STK_SIZE 		128				//任务堆栈大小  
TaskHandle_t SI433Task_Handler;					//任务句柄
void si433ack_task(void *pvParameters);	//任务函数

extern u8 rx_buf[RX_NUM_SIZE];
extern HalAdapter_Management_FLASH_t GFLASH;
extern flash_config_t flash_config;
extern HalAdapter_Management_Led_t halAdapter_Led;//API接口供外部使用
extern DataPayload_t dataPayload;

void Restart(void)
{
//	__disable_irq();
//	NVIC_SystemReset();
	__set_FAULTMASK(1);//关闭总中断
	NVIC_SystemReset();//请求单片机重启
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置系统中断优先级分组4	 
	delay_init();	    															//延时函数初始化 串口2
	uart_init(115200);															//调试使用
	Power_Init();																		//电源初始化
	halAdapter_Led.HAL_LedInit();										//初始化LED
	RS232_Init(921600);															//340初始化	 串口1	
	RS232_POWER_STATE			= 0;											//340通电 拉高断开
	delay_xms(100);
	RS232_RESET_STATU =1;
	
	SI4438_POWER_PIN_STATE=0;												//4438通电

	Get_Mcu_Mac();
	
	GFLASH.Read(0,(u8*)&flash_config,sizeof(flash_config));//读取离线数据
	if(flash_config.pair_device.pair_ch != 0)
		dataPayload.nrf_channel=flash_config.pair_device.pair_ch;
	else
		dataPayload.nrf_channel=0;
	SI446x_Init();																			//4438初始化
	
	GREEN_LED_ON;
	delay_xms(1000);

	//初始化成功
	//创建开始任务
	  xTaskCreate((TaskFunction_t )start_task,            //任务函数
				  (const char*    )"start_task",          //任务名称
				  (uint16_t       )START_STK_SIZE,        //任务堆栈大小
				  (void*          )NULL,                  //传递给任务函数的参数
				  (UBaseType_t    )START_TASK_PRIO,       //任务优先级
				  (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
	  vTaskStartScheduler();          										//开启任务调度
}

//开始任务的任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           										//进入临界区
    //创建任务
    xTaskCreate((TaskFunction_t )master_task,  					//任务函数   	
                (const char*    )"master_task", 				//任务名称  	
                (uint16_t       )MASTER_STK_SIZE, 			//任务堆栈大小
                (void*          )NULL,									//传递给任务函数的参数
                (UBaseType_t    )MASTER_TASK_PRIO,			//任务优先级
                (TaskHandle_t*  )&MASTERTask_Handler); 	//任务句柄
		 xTaskCreate((TaskFunction_t )wifirecve_task,     	
                (const char*    )"wifirecve_task",   	
                (uint16_t       )WIFIRECVE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )WIFIRECVE_TASK_PRIO,	
                (TaskHandle_t*  )&WIFIRECVETask_Handler);					
		xTaskCreate((TaskFunction_t )si433ack_task,     	
                (const char*    )"si433ack_task",   	
                (uint16_t       )SI433ACK_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SI433ACK_TASK_PRIO,	
                (TaskHandle_t*  )&SI433Task_Handler);
		vTaskSuspend(SI433Task_Handler);										//挂起主协议任务
    vTaskDelete(StartTask_Handler); 										//删除开始任务
    taskEXIT_CRITICAL();           	 										//退出临界区
}

void si433ack_task(void *pvParameters)
{	
	static u8 i=0;
	static u8 Ackcountflg;

	while(1)
  { 
		taskENTER_CRITICAL();//进入临界区保护
		if(Ackcountflg++ == 2)
		{
			i++;
			Send_Master_Cmd_Data(i);//轮询发送命令
	
			if(i>=7)
				i=0;
	
			Ackcountflg=0;
		}
		else
			Si433_Rcecv_Buff();		//数据接收处理
		
		taskEXIT_CRITICAL(); //退出临界区保护 	
		vTaskDelay(5);
  }
}

//主协议任务函数 
void master_task(void *pvParameters)
{
	static u16 temp_cnt = 0;
	
	if(Login_Or_Boardcast(1))//与上位机建立连接
	{
		vTaskResume(SI433Task_Handler );//恢复主协议任务
	}
				
	while(1)
	{
		/*主协议：姿态上传*/
		attitudeUploadHandler();
			
		/*电量上传*/
		if(++temp_cnt >= BAT_UPDATA_TIME)
		{
			temp_cnt = 0;
			protocol_sendData(GetbatteryPresent(),5,BAT_DATA_CMD);
		}

		vTaskDelay(35);//30ms发送主协议一次，不能过快
	}
}  

void wifirecve_task(void *pvParameters)
{
    while(1)
    {
		taskENTER_CRITICAL();//进入临界区保护
		DataCallBack_WIFI(rx_buf,sizeof(rx_buf));
		taskEXIT_CRITICAL(); //退出临界区保护
		vTaskDelay(5);
    }
}  



