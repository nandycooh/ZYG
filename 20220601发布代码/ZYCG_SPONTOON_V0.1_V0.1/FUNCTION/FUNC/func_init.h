#ifndef __FUNC_INIT_H
#define __FUNC_INIT_H

#include "func_config.h"
#include "mpu.h"
#include "hal_iic.h"
#include "hal_mpu.h"


#define AL_SAMPLE_RATE 													300                 //MPU的采样评率（4~1000）
#define AL_IIR_RATE															282									//低通滤波的频率(根据MPU采样率调整)
/***************************RF*********************************/

#define SPONTOON		0x59
#define SHIELD			0x60
#define PAPPER_SPRAY	0x61

//#define RF433_ID        												LEFTWALKID								//0x55->left   0x56->right

							//0x55->left   0x56->right

/***************************MPU********************************/
//Low-power accel mode
#define MOTION_THRESHOLD												1000 								//运动检测阈值（32~8160）
#define MOTION_DURATION													1										//1ms的运动持续时间
#define MOTION_SAMPLING_RATE										1										//Low-power accel mode supports the following frequencies: 1.25Hz(1), 5Hz(<=5), 20Hz(<=20), 40Hz(else)
#define MOTION_DETECTION												MOTION_THRESHOLD,MOTION_DURATION,MOTION_SAMPLING_RATE


#define TOUCH_GPIO_STATE		PBin(8)
#define TOUCH_GPIO_POWER		PBout(9)
#define INSERANCE_GPIO_STATE	PAin(11)
#define SHOCK_GPIO_STATE		PAin(12)
#define FLEX_GPIO_STATE			PAin(15)

//硬件层
typedef struct
{
	bool isInit;
	HalAdapter_Management_IIC_t* _IIC;
	HalAdapter_Management_MPU_t* _MPU;
}HAL_adapter_t;

//功能层
typedef struct
{
	bool isInit;
	FuncAdapter_Management_MPU_t* MPU;

	void (*delay_ms)(u16 ms);
	void (*delay_us)(u32 us);
	void (*get_timestamp)(unsigned long*);
}FUNC_adapter_t;

void Vibrate_Gpio_Init(void);
void Touch_Gpio_Init(void);
FUNC_adapter_t* GetFuncAdapter(void);

#endif
