#include <stdio.h>
#include "mpu.h"
#include "mpu_6050.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "hal_mpu.h"
#include "sleep.h"

uint8_t Func_MPU_Init(uint32_t rate)
{
	int result = 0;
//	Soft_I2C_Configuration();
	MPU_IIC_Init();//初始化IIC总线
	printf("mpu 9250 start...\r\n");
	result=MPU_Read_Byte(MPU_DEVICE_ID_REG); 
	if(result != MPU_ADDR)//器件ID正确
	{
		printf("MPU_ADDR failed: errno = %d", result);
		GetHalAdapter_OptMode()->HAL_Restart();//软重启
		return 1;
	}
  result = mpu_init();
  if(result) {
     printf("mpu_init failed: errno = %d", result);
	}
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
  mpu_set_sample_rate(rate);
	return 0;
}
int Func_MPU_LpMotionInterrupt(uint16_t thresh, uint8_t time, uint8_t lpa_freq)
{
	return mpu_lp_motion_interrupt(thresh, time, lpa_freq);
}

int Func_MPU_read_fifo(short *gyro, short *accel, unsigned long *timestamp,
        unsigned char *sensors, unsigned char *more)
{
	return mpu_read_fifo(gyro, accel, timestamp, sensors, more);
}

uint8_t Func_MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
	return MPU_Get_Gyroscope(gx, gy, gz);
}

uint8_t Func_MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
	return MPU_Get_Accelerometer(ax, ay, az);
}

float Func_MPU_Get_Temperature(void)
{
	return MPU_Get_Temperature();
}

bool Func_MPU_Is_NewData(void)
{
	return GetHalAdapter_MPU()->HAL_MPU_GetInterrupStatus();
}

FuncAdapter_Management_MPU_t funcAdapter_MPU =
{
.Init = Func_MPU_Init,
.LpMotionInterrupt = Func_MPU_LpMotionInterrupt,
.ReadFIFO = Func_MPU_read_fifo,
.GetGyroscope = Func_MPU_Get_Gyroscope,
.GetAccelerometer = Func_MPU_Get_Accelerometer,
.GetTemperature = Func_MPU_Get_Temperature,
.isGenerateNewData = Func_MPU_Is_NewData,
};
/**
  * @brief 获取MPU实例
  * @param
  * @retval: None
*/
FuncAdapter_Management_MPU_t* GetFuncAdapter_MPU(void)
{
	return &funcAdapter_MPU;
}

