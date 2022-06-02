#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c.h"
#include "./exti/bsp_exti.h"
#include "stm32f10x_usart.h"
#include "./usart/USART2.h"
#include "./nrf24L01/exti.h"
#include "./timer2/timer2.h"
#include "./nrf24L01/spi.h"
#include "./nrf24L01/nrf24l01p.h"
#include "delay.h"

#include "mltypes.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "log.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mpu.h"
#include "inv_mpu.h"
#include "log.h"
#include "packet.h"
#include "MadgwickAHRS.h"
#include "eMPL_outputs.h"
#include "key.h"
#include "timer2.h"
#include "iic2.h"
#include "crc.h"
#include "flash.h"

#define COMPASS_ENABLED
//#define MPU9250
/* Data read from MPL. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

#define DEFAULT_MPU_HZ (100)
#define COMPASS_READ_MS (10)
#define MAIN_FOR_MY_OWN 1

#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)
#define TEMP_READ_MS    (500)
#define PEDO_READ_MS    (1000)

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
/*后退标志位*/
unsigned char back_flag = 0;

struct rx_s {
	unsigned char header[3];
	unsigned char cmd;
};

struct hal_s {
	unsigned char lp_accel_mode;
	unsigned char sensors;
	unsigned char dmp_on;
	unsigned char wait_for_tap;
	volatile unsigned char new_gyro;
	unsigned char motion_int_mode;
	unsigned long no_dmp_hz;
	unsigned long next_pedo_ms;
	unsigned long nexttmp_ms;
	unsigned long next_compass_ms;
	unsigned int report;
	unsigned short dmp_features;
	struct rx_s rx;
};
static struct hal_s hal = {0};

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s {
	signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata = {

	.orientation = { 1, 0, 0,
		0, 1, 0,
		0, 0, 1}
};

static struct platform_data_s compass_pdata = {
	.orientation = { 0, 1, 0,
		1, 0, 0,
		0, 0, -1}
};

int new_gyro = 0;
void gyro_data_ready_cb(void)
{
	new_gyro = 1;
	hal.new_gyro = 1;
}

unsigned short inv_row_2_scale(const signed char *row)
{
	unsigned short b;

	if (row[0] > 0)
		b = 0;
	else if (row[0] < 0)
		b = 4;
	else if (row[1] > 0)
		b = 1;
	else if (row[1] < 0)
		b = 5;
	else if (row[2] > 0)
		b = 2;
	else if (row[2] < 0)
		b = 6;
	else
		b = 7;      // error
	return b;
}

void mget_ms(unsigned long *time)
{
	//*time=(unsigned long)HAL_GetTick();
}


float dmpinvSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

float dmpsafe_asin(float v)
{
	if(isnan(v)) 
	{
		return 0.0;
	}
	if(v >= 1.0)
	{
		return M_PI / 2;
	}
	if(v <= -1.0) 
	{
		return -M_PI / 2;
	}
	return asin(v);
}

void MPU9250_Config(void)
{
    unsigned char accel_fsr = 0;
	unsigned short gyro_rate = 0, gyro_fsr = 0;
	struct int_param_s int_param;
	unsigned short compass_fsr;

    int result = 0;

	printf("mpu 9250 start...\r\n");
	result = mpu_init(&int_param);
	if(result) {
		printf("mpu_init failed: errno = %d", result);
	}

	result = inv_init_mpl();
	inv_enable_quaternion();
	inv_enable_9x_sensor_fusion();
	inv_enable_fast_nomot();
	inv_enable_gyro_tc();
	inv_enable_vector_compass_cal();
	inv_enable_magnetic_disturbance();
	//inv_start_vector_compass_cal();
	inv_enable_eMPL_outputs();
	result = inv_start_mpl();
	if (result == INV_ERROR_NOT_AUTHORIZED)
		printf("Not authorized\n");
	if (result)
		printf("Could not start the MPL.\n");

	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(DEFAULT_MPU_HZ);
	mpu_set_compass_sample_rate(1000/COMPASS_READ_MS);

	mpu_get_sample_rate(&gyro_rate);
	mpu_get_gyro_fsr(&gyro_fsr);
	mpu_get_accel_fsr(&accel_fsr);
	mpu_get_compass_fsr(&compass_fsr);

	inv_set_gyro_sample_rate(1000000L / gyro_rate);
	inv_set_accel_sample_rate(1000000L / gyro_rate);
	inv_set_compass_sample_rate(COMPASS_READ_MS * 1000L);

	inv_set_gyro_orientation_and_scale(
			inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
			(long)gyro_fsr<<15);
	inv_set_accel_orientation_and_scale(
			inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
			(long)accel_fsr<<15);
	inv_set_compass_orientation_and_scale(
			inv_orientation_matrix_to_scalar(compass_pdata.orientation),
			(long)compass_fsr<<15);
}
u8 sumCheck(u8 *buf, u8 len)
{
	u8 sum=0;

	for(u8 i=0; i<len; i++){
		sum += buf[i];
	}
	return sum;
}
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
float roll_result, yaw_result, pitch_result;
float q_result[4];
int8_t accuracy_result;
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define DATA_LEN   8
unsigned char dmp_data[DATA_LEN];
u8 send458_buf[13];
u8 send458_ybuf[19];
u8 sendAccAndQuat_buf[16];
void Data_Send_Quat(float *q, int8_t accuracy, u8 *buf)
{
//	unsigned char cnt=0, sum=0;
	unsigned int _temp;
	u16 crc16 =0;
	u8 i = 0;
	send458_buf[i++] = 0x66;
	send458_buf[i++] = SLAVER_NUM;
	send458_buf[i++] = 11;//len
	_temp = (int)(q[0]*100);
	send458_buf[i++]=BYTE0(_temp);
	_temp = (int)(q[1]*100);
	send458_buf[i++]=BYTE0(_temp);
	_temp = (int)(q[2]*100);
	send458_buf[i++]=BYTE0(_temp);
	_temp = (int)(q[3]*100);
	send458_buf[i++]=BYTE0(_temp);
	send458_buf[i++] = (char)accuracy;//精度
	send458_buf[i++] = 0x00;//预留
	crc16 = Message_count_CRC_2(i, send458_buf);
	send458_buf[i++] = crc16&0xFF;
	send458_buf[i++] = ((crc16&0xFF00)>>8);
	send458_buf[i++] = 0x11;//end1
	send458_buf[i++] = 0x22;//end2
}

void yData_Send_Quat(float *q, float *euler, int8_t accuracy, u8 *buf)
{
//	unsigned char cnt=0, sum=0;
	unsigned int _temp;
	u16 crc16 =0;
	u8 i = 0;
	send458_ybuf[i++] = 0x66;
	send458_ybuf[i++] = SLAVER_NUM;
	send458_ybuf[i++] = 17;//len
	_temp = (int)(q[0]*100);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(q[1]*100);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(q[2]*100);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(q[3]*100);
	send458_ybuf[i++]=BYTE0(_temp);
	send458_ybuf[i++] = (char)accuracy;//精度
	_temp = (int)(euler[0]);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(euler[0]);
	send458_ybuf[i++]=BYTE1(_temp);
	_temp = (int)(euler[1]);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(euler[1]);
	send458_ybuf[i++]=BYTE1(_temp);
	_temp = (int)(euler[2]);
	send458_ybuf[i++]=BYTE0(_temp);
	_temp = (int)(euler[2]);
	send458_ybuf[i++]=BYTE1(_temp);
	//send458_ybuf[i++] = 0x00;//预留
	send458_ybuf[i++] = back_flag;
	crc16 = Message_count_CRC_2(i, send458_ybuf);
	send458_ybuf[i++] = crc16&0xFF;
	send458_ybuf[i++] = ((crc16&0xFF00)>>8);
	send458_ybuf[i++] = 0x11;//end1
	send458_ybuf[i++] = 0x22;//end2
}
#define FLASH_START_ADDR 0x0801FC00

size_t inv_data_size;
unsigned char SenseData[125];
u8 readFlashBUF[125];
u8 LoadSenseBUF[124];
u8 SaveFlashFlag = 1;
u8 LoadFlashFlag;
u16 accuracyTimeCnt;
void mpu_get_data()
{
	unsigned long timestamp;
	unsigned long next_compass_ms = 0;
	unsigned char sensors, more;
//	int  has_run_test = 0;
	int new_data = 0;
	int new_compass = 0;

	short compass[3] = {0}, accel[3] = {0}, gyro[3] = {0};
	long compass_l[3] = {0},accel_l[3] = {0};
	long data[9] = {0};
	int8_t accuracy;

	get_tick_count(&timestamp);
	if (timestamp / 100 > next_compass_ms) {
		next_compass_ms = timestamp / 100 + COMPASS_READ_MS;
		new_compass = 1;
	}

	if (new_gyro) {
		new_gyro = 0;
		new_data = 1;
		if (!mpu_read_fifo(gyro, accel, &timestamp, &sensors, &more)) 
		{
			if (more) new_gyro = 1;
			if (sensors & INV_XYZ_GYRO) {
				new_data = 1;
				inv_build_gyro(gyro, timestamp);
			}
			if (sensors & INV_XYZ_ACCEL) {
				new_data = 1;
				accel_l[0] = (long)accel[0];
				accel_l[1] = (long)accel[1];
				accel_l[2] = (long)accel[2];
				inv_build_accel(accel_l, 0, timestamp);
			}
		}
	}

	/* update compass */
	if (new_compass) {
		new_compass = 0;
		if (!mpu_get_compass_reg(compass, &timestamp)) {
			compass_l[0] = (long)compass[0];
			compass_l[1] = (long)compass[1];
			compass_l[2] = (long)compass[2];
			inv_build_compass(compass_l, 0, timestamp);
		}
	}

	if (new_data) {
		new_data = 0;
		inv_execute_on_data();

		if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t*)&timestamp)) {
			/* Sends a quaternion packet to the PC. Since this is used by the Python
			 * test app to visually represent a 3D quaternion, it's sent each time
			 * the MPL has new data.
			 */
			
			float q[4] = {0};
			float euler[3] = {0};
			q[0] = (float)data[0] / ((float)(1L << 30));  
			q[1] = (float)data[1] / ((float)(1L << 30));
			q[2] = (float)data[2] / ((float)(1L << 30));
			q[3] = (float)data[3] / ((float)(1L << 30));
			float norm = dmpinvSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
			q[0] = q[0] * norm;
			q[1] = q[1] * norm;
			q[2] = q[2] * norm;
			q[3] = q[3] * norm;  
			accuracy_result = accuracy;

			q_result[0] = q[0];
			q_result[1] = q[1];
			q_result[2] = q[2];
			q_result[3] = q[3];
			if(SLAVER_NUM == 5){
				if (inv_get_sensor_type_euler(data, &accuracy,(inv_time_t*)&timestamp))
				{
						euler[0] = data[0]*1.0/(1<<16) ;
						euler[1] = data[1]*1.0/(1<<16);
						euler[2] = data[2]*1.0/(1<<16);
						yData_Send_Quat(q, euler, accuracy, send458_ybuf);
//					printf("%d\r\n",__LINE__);
				}
			}
			else{
//				printf("%d\r\n",__LINE__);
				Data_Send_Quat(q, accuracy, send458_buf);
			}
			time_flag = 0;
//            printf("%d", accuracy);
            if(accuracy == 3)
            {
                accuracyTimeCnt++;
                if(SaveFlashFlag && accuracyTimeCnt == 100)
                {
                    accuracyTimeCnt = 0;
                    inv_get_mpl_state_size(&inv_data_size);
                    inv_save_mpl_states(SenseData, inv_data_size);
                    LoadFlashFlag = 1;
                    SaveFlashFlag = 0;
                    SenseData[124] = LoadFlashFlag;
                    flash_write(FLASH_START_ADDR, (u16*)SenseData, inv_data_size + 1);
//                    inv_load_mpl_states(SenseData, 124);

                }
            }
            else if(accuracy != 3)
            {
                accuracyTimeCnt = 0;
                SaveFlashFlag = 1;
            }
//            printf("accuracy = %d, q_result=%6.2f,%6.2f,%6.2f,%6.f\r\n", 
//                accuracy_result, q_result[0], q_result[1], q_result[2], q_result[3]);

		}
	}
}
void Enable_485(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启LED相关的GPIO外设时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;	  //3脚RE, 4脚DE
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    /*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    /*调用库函数，初始化GPIO*/
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    SLAVERRX;
    
//    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
//    GPIO_SetBits(GPIOB, GPIO_Pin_4);

}
//延时函数
void delay1ms(u32 time)
{
    u16 i;
	while(time--)
	{
		i=12000;
		while(i--);
	}
}
void delay1us(u32 time)
{
    u16 i;
	while(time--)
	{
		i=12;
		while(i--);
	}
}
u8 test1buf[8] = {0x66, SLAVER_NUM, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
extern u8 send485_flag;
int main() 
{
	u8 back_count = 0;
	RCC_ClocksTypeDef RCC_Clocks;

	SysTick_Init();
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	/* usart1 */
	USART_Config();//串口1 调试用
    Initial_UART2(115200);
//    Enable_485();
	
	/* mpu9250 interrupt */
	EXTI_Pxy_Config();//PA8
  /* mpu9250 IIC配置 */
	I2C_Bus_Init();
  MPU9250_Config();
	
	RCC_GetClocksFreq(&RCC_Clocks);
	printf("SYSCLK:%dHz\r\n",RCC_Clocks.SYSCLK_Frequency);
            
    flash_read(FLASH_START_ADDR, (u16*)readFlashBUF, 125);
    if(readFlashBUF[124] == 1)
    {
        LoadFlashFlag = 0;
        inv_load_mpl_states(readFlashBUF, 124);
    }
   if(SLAVER_NUM == 5){
			init_backKey();//后退初始化
	 }

  delay1ms(1000);
//	u32 printf_cnt = 0;
	
	while (1) 
	{
		if(SLAVER_NUM == 5){
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0){
				back_count++;
				if(back_count >= 5){//消抖
					back_count = 0;
					back_flag = 1;//后退标志位
				}
			}else{
				back_count = 0;
				back_flag = 0;
			}
		}
		
		mpu_get_data();
		
		if(SLAVER_NUM == 5)
		{
			//usart2SendBytes(send458_buf, 13);
			usart2SendBytes(send458_ybuf, 19);
		}
		else
		{
			usart2SendBytes(send458_buf, 13);
		}
//		delay1ms(100);//不能有延时,printf也不行，影响6050数据
//		if(++printf_cnt >= 0xFFF){
//			printf_cnt = 0;
//			printf("%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\r\n",
//			send458_ybuf[0],send458_ybuf[1],send458_ybuf[2],send458_ybuf[3],send458_ybuf[4],send458_ybuf[5],send458_ybuf[6],send458_ybuf[7],send458_ybuf[8],send458_ybuf[9],send458_ybuf[10],send458_ybuf[11],send458_ybuf[12],\
//			send458_ybuf[13],send458_ybuf[14],send458_ybuf[15],send458_ybuf[16],send458_ybuf[17],send458_ybuf[18]
//			);
//		}
	}
}
