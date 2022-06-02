#include <math.h>
#include "walk.h"
#include "Filter.h"
#include "Myqueue.h"
#include <stdio.h>
#include "string.h"

//------------------------------------------------------------------------
//IIR滤波器参数变量定义
IIR_coeff_Typedef Filter_Param_Gx;
IIR_coeff_Typedef *pFilter_Param_Gx = &Filter_Param_Gx;
IIR_coeff_Typedef Filter_Param_Gy;
IIR_coeff_Typedef *pFilter_Param_Gy = &Filter_Param_Gy;
IIR_coeff_Typedef Filter_Param_Gz;
IIR_coeff_Typedef *pFilter_Param_Gz = &Filter_Param_Gz;
IIR_coeff_Typedef Filter_Param_Ax;
IIR_coeff_Typedef *pFilter_Param_Ax = &Filter_Param_Ax;
IIR_coeff_Typedef Filter_Param_Ay;
IIR_coeff_Typedef *pFilter_Param_Ay = &Filter_Param_Ay;
IIR_coeff_Typedef Filter_Param_Az;
IIR_coeff_Typedef *pFilter_Param_Az = &Filter_Param_Az;
//------------------------------------------------------------------------
//队列定义
Que_typedef s;
Que_typedef *q=&s;
Que_typedef_small ss;
Que_typedef_small *qs=&ss;
Que_typedef_accel a;
Que_typedef_accel *qa = &a;
//------------------------------------------------------------------------
//值转化参数
float gyro_range_scale = (0.0174532f / 16.4f); //正负2000度/秒  将原始数据转换成弧度/秒
float avg_speed_scale_gyrof = (10.0f /25.0f);
float avg_speed_scale_accel = (10.0f /50.0f);
float accel_range_scale = MPU9250_ONE_G * (1.0f / 8192.0f); //正负4g
//------------------------------------------------------------------------
//步行算法
static float sum_gyrof = {0.0f};
static float sum_gyrof_start = {0.0f};
static float avg_gyrof_start_last = 0.0f;
static float sum_accel[3] = {0.0f};
static float sum_accelf_subG = 0.0f;
static int count_j = 0;
//步行结果
static uint8_t flag_Walk = 0;
static float avg_Speed = 0.0f;
//差值比较
short last_gyro[3]; 
static int count_last_gyro = 0;

//void VoidGear_Init(uint32_t rate)
//{
//	//--------------------------------------------------------------------   
//	cal_iir_coeff(pFilter_Param_Gx, rate, 15); //计算滤波器系数
//	cal_iir_coeff(pFilter_Param_Gy, rate, 15);
//	cal_iir_coeff(pFilter_Param_Gz, rate, 15);
//	cal_iir_coeff(pFilter_Param_Ax, rate, 15); //计算滤波器系数
//	cal_iir_coeff(pFilter_Param_Ay, rate, 15);
//	cal_iir_coeff(pFilter_Param_Az, rate, 15);

//	init_que(q); //初始化队列
//	init_que_small(qs);
//	init_que_accel(qa);
//	sum_gyrof_start = 0.0f;
//	sum_gyrof = 0.0f;
//	//---------------------------------------------------------------------
//}

void VoidGear_Init(uint32_t rate)
{
	memset(&Filter_Param_Gx,0,sizeof(Filter_Param_Gx));
    memset(&Filter_Param_Gy,0,sizeof(Filter_Param_Gy));
    memset(&Filter_Param_Gz,0,sizeof(Filter_Param_Gz));
    memset(&Filter_Param_Ax,0,sizeof(Filter_Param_Ax));
    memset(&Filter_Param_Ay,0,sizeof(Filter_Param_Ay));
    memset(&Filter_Param_Az,0,sizeof(Filter_Param_Az));
	//--------------------------------------------------------------------   
	cal_iir_coeff(pFilter_Param_Gx, rate, 15); //计算滤波器系数
	cal_iir_coeff(pFilter_Param_Gy, rate, 15);
	cal_iir_coeff(pFilter_Param_Gz, rate, 15);
	cal_iir_coeff(pFilter_Param_Ax, rate, 15); //计算滤波器系数
	cal_iir_coeff(pFilter_Param_Ay, rate, 15);
	cal_iir_coeff(pFilter_Param_Az, rate, 15);

	init_que(q); //初始化队列
	init_que_small(qs);
	init_que_accel(qa);
  
    sum_gyrof=0.0f;
    sum_gyrof_start=0.0f;
    sum_gyrof=0.0f;
    sum_accelf_subG=0.0f;
    count_j=0;
    sum_accel[0]=0.0f;
    sum_accel[1]=0.0f;
    sum_accel[2]=0.0f;
}

void VoidGear_Algorithm_Walk(short* gyro, short* accel)
{
	if(last_gyro[0] - gyro[0] > 10000 || last_gyro[1] - gyro[1] > 10000 || last_gyro[2] - gyro[2] > 10000)
	{
		if(++count_last_gyro < 3){
			return;
		}
	}
	count_last_gyro = 0;
	/*Algorithm*/
	float gyrof[3] = {0.0f};
	float accelf[3] = {0.0f};
	float avg_accelf[3] = {0.0f};
	float accelf_subG[3] = {0.0f};
	
	last_gyro[0] = gyro[0];
	last_gyro[1] = gyro[1];
	last_gyro[2] = gyro[2];
	
	gyrof[0] = gyro[0] * gyro_range_scale; //将陀螺仪原始数据变换到弧度
	gyrof[1] = gyro[1] * gyro_range_scale;
	gyrof[2] = gyro[2] * gyro_range_scale;

	accelf[0] = accel[0] * accel_range_scale;
	accelf[1] = accel[1] * accel_range_scale;
	accelf[2] = accel[2] * accel_range_scale;

	gyrof[0] = get_iir_output(pFilter_Param_Gx, gyrof[0]); //将弧度数据滤波
	gyrof[1] = get_iir_output(pFilter_Param_Gy, gyrof[1]);
	gyrof[2] = get_iir_output(pFilter_Param_Gz, gyrof[2]);

	accelf[0] = get_iir_output(pFilter_Param_Ax, accelf[0]); 
	accelf[1] = get_iir_output(pFilter_Param_Ay, accelf[1]);
	accelf[2] = get_iir_output(pFilter_Param_Az, accelf[2]);

	float gyrof_length = pow(gyrof[0],2)+pow(gyrof[1],2)+pow(gyrof[2],2); //角速度向量各分量平方和

	//计算队列均值、MAXSIZE为队列长度
	q->data[q->rear] = gyrof_length;
	float error = q->data[q->rear] - q->data[q->front];
	sum_gyrof_start += error;
	float avg_gyrof_start = sum_gyrof_start/(MAXSIZE-1);
	q->rear = (q->rear+1)%MAXSIZE;
	q->front = (q->front+1)%MAXSIZE;

	qs->data[qs->rear] = gyrof_length;
	float errors = qs->data[qs->rear] - qs->data[qs->front];
	sum_gyrof += errors;
	float avg_gyrof = sum_gyrof/(MAXSIZES-1);
	qs->rear = (qs->rear+1)%MAXSIZES;
	qs->front = (qs->front+1)%MAXSIZES;

	//计算加速度向量
	qa->data0[qa->rear] = accelf[0];
	qa->data1[qa->rear] = accelf[1];
	qa->data2[qa->rear] = accelf[2];
	float error_a0 = qa->data0[qa->rear] - qa->data0[qa->front];
	float error_a1 = qa->data1[qa->rear] - qa->data1[qa->front];
	float error_a2 = qa->data2[qa->rear] - qa->data2[qa->front];
	sum_accel[0] += error_a0;
	sum_accel[1] += error_a1;
	sum_accel[2] += error_a2;
	avg_accelf[0] = sum_accel[0]/(MAXSIZE_A-1);
	avg_accelf[1] = sum_accel[1]/(MAXSIZE_A-1);
	avg_accelf[2] = sum_accel[2]/(MAXSIZE_A-1);
	float avg_accelf_length = sqrtf(pow(avg_accelf[0],2)+pow(avg_accelf[1],2)+pow(avg_accelf[2],2));
	float avg_accelf_length_range = MPU9250_ONE_G/avg_accelf_length;
	avg_accelf[0] = avg_accelf[0]*avg_accelf_length_range;
	avg_accelf[1] = avg_accelf[1]*avg_accelf_length_range;
	avg_accelf[2] = avg_accelf[2]*avg_accelf_length_range;
	accelf_subG[0] = accelf[0] - avg_accelf[0];
	accelf_subG[1] = accelf[1] - avg_accelf[1];
	accelf_subG[2] = accelf[2] - avg_accelf[2];

	float accelf_subG_length = pow(accelf_subG[0],2) + pow(accelf_subG[1],2) + pow(accelf_subG[2],2);
	qa->data[qa->rear] = accelf_subG_length;
	float error_a4 = qa->data[qa->rear] - qa->data[qa->front];
	sum_accelf_subG += error_a4;
	float avg_accelf_subG = sum_accelf_subG/(MAXSIZE_A-1);
	qa->rear = (qa->rear+1)%MAXSIZE_A;
	qa->front = (qa->front+1)%MAXSIZE_A;

	//脚步启停判断，flag_Walk为启停标签
	if(avg_gyrof_start_last<0.8f&&avg_gyrof_start<0.8f)  
	{
		count_j++;
		if(count_j>5000)
		{
				count_j = 300;
		}
	} 
	else
	{
		count_j = 0;
	}           
	if(count_j>=3&&avg_gyrof_start<0.8f)
	{
		flag_Walk = 0;
	}
	if(avg_gyrof_start>=0.8)
	{
		flag_Walk = 1;
//		printf("----------------------------------------------------\r\n");
	}
//	printf("avg_gyrof_start:%6.3f\r\n", avg_gyrof_start);
//	printf("gyro  :%d\t %d\t %d\r\n", gyro[0], gyro[1], gyro[2]);
//	printf("gyrof :%6.3f\t %6.3f\t %6.3f\r\n", gyrof[0], gyrof[1], gyrof[2]);
//	printf("accel :%d\t %d\t %d\r\n", accel[0], accel[1], accel[2]);
//	printf("accelf:%6.3f\t %6.3f\t %6.3f\r\n", accelf[0], accelf[1], accelf[2]);
	avg_gyrof_start_last = avg_gyrof_start;
	float avg_Speed_gyrof = avg_gyrof;
	if(avg_Speed_gyrof >= 25.0f){
	 avg_Speed_gyrof = 25.0f;
	}              
	avg_Speed_gyrof *= avg_speed_scale_gyrof;
	float avg_Speed_accel = avg_accelf_subG/10.0f;

	if(avg_Speed_accel >= 50.0f){
	 avg_Speed_accel = 50.0f;
	}
					
	avg_Speed_accel *= avg_speed_scale_accel;
	avg_Speed = 0.5f*avg_Speed_gyrof + 0.5f*avg_Speed_accel ;
	avg_Speed = avg_Speed * 2.0f;                   
	if(avg_Speed < 7.5f)
	{
		avg_Speed = avg_Speed * 1.0f; 
	}
	else if(avg_Speed>=7.0f&&avg_Speed<7.5f)
	{
		avg_Speed = avg_Speed * 1.0f + 1.0f; 
	}
	else if(avg_Speed>=7.5f&&avg_Speed<8.5f)
	{
		avg_Speed = avg_Speed * 1.0f + 1.5f; 
	}
	else
	{
		avg_Speed = avg_Speed * 1.0f;
	}

	if(avg_Speed>=10.0f)
	{
	 avg_Speed = 10.0f;
	}        
	avg_Speed *=100;
	
	if(isnan(avg_Speed)==true)//211217
	{
		VoidGear_Init(300);
	}
}

uint8_t VoidGear_GetWalk(void)
{
	return flag_Walk;
}

float VoidGear_GetSpeed(void)
{
	return avg_Speed;
}


VoidGear_Walk_t walk =
{
	false,
	VoidGear_Init,
	VoidGear_Algorithm_Walk,
	VoidGear_GetWalk,
	VoidGear_GetSpeed,
};


VoidGear_Walk_t* WalkGetObj(void)
{
	return &walk;
}

