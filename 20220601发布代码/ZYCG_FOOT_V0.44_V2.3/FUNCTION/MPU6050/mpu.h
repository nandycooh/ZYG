#ifndef __MPU_H
#define __MPU_H
#include "func_config.h"

//IIC管理器
typedef struct
{
	/**
		* @brief  MPU初始化
		* @param  None
		* @retval : None
	**/
	uint8_t (*Init)(uint32_t);
	/**  
	 *	@brief      Enters LP accel motion interrupt mode.
	 *  @param[in]  thresh      Motion threshold in mg.
	 *  @param[in]  time        Duration in milliseconds that the accel data must
	 *                          exceed @e thresh before motion is reported.
	 *  @param[in]  lpa_freq    Minimum sampling rate, or zero to disable.
	 *  @return     0 if successful.
  **/
	int (*LpMotionInterrupt)(unsigned short, unsigned char, unsigned char);
	/**
	 *  @brief      Get one packet from the FIFO.
	 *  If @e sensors does not contain a particular sensor, disregard the data
	 *  returned to that pointer.
	 *  \n @e sensors can contain a combination of the following flags:
	 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
	 *  \n INV_XYZ_GYRO
	 *  \n INV_XYZ_ACCEL
	 *  \n If the FIFO has no new data, @e sensors will be zero.
	 *  \n If the FIFO is disabled, @e sensors will be zero and this function will
	 *  return a non-zero error code.
	 *  @param[out] gyro        Gyro data in hardware units.
	 *  @param[out] accel       Accel data in hardware units.
	 *  @param[out] timestamp   Timestamp in milliseconds.
	 *  @param[out] sensors     Mask of sensors read from FIFO.
	 *  @param[out] more        Number of remaining packets.
	 *  @return     0 if successful.
	**/
	int (*ReadFIFO)(short*, short*, unsigned long*, unsigned char*, unsigned char*);
	/**
	 *  @brief      Get Gyroscope from register.
	 *  @param[out] gx        Gyro data in x axis.
	 *  @param[out] gy        Gyro data in y axis..
	 *  @param[out] gz        Gyro data in z axis.
	 *  @return     0 if successful.
	**/
	uint8_t (*GetGyroscope)(short*, short*, short*);
	/**
	 *  @brief      Get Accelerometer from register.
	 *  @param[out] ax        Accel data in x axis.
	 *  @param[out] ay        Accel data in y axis..
	 *  @param[out] az        Accel data in z axis.
	 *  @return     0 if successful.
	**/
	uint8_t (*GetAccelerometer)(short*, short*, short*);
	/**
	 *  @brief      Get GetTemperature from register.
	 *  @param  		None
	 *  @return     Temperature
	**/
	float (*GetTemperature)(void);
	/**
	 *  @brief      Get GetTemperature from register.
	 *  @param  		None
	 *  @return     Temperature
	**/
	bool (*isGenerateNewData)(void);
}FuncAdapter_Management_MPU_t;

FuncAdapter_Management_MPU_t* GetFuncAdapter_MPU(void);
	  
#endif
















