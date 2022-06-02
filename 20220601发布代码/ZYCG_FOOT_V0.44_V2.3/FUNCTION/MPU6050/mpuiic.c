#include "mpuiic.h"
#include "hal_iic.h"
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include <stdbool.h>


void MPU_Delay_Ms(u16 ms);
//
unsigned short Get_I2C_Retry(void);
static void Soft_I2C_Delay(uint32_t dly);
static uint8_t Soft_I2C_START(void);
static void Soft_I2C_STOP(void);
static void Soft_I2C_SendACK(void);
static void Soft_I2C_SendNACK(void);
static uint8_t Soft_I2C_SendByte(uint8_t anbt_i2c_data);
static uint8_t Soft_I2C_ReceiveByte_WithACK(void);
static uint8_t Soft_I2C_ReceiveByte(void);
static uint8_t Soft_DMP_I2C_Write(uint8_t soft_dev_addr, uint8_t soft_reg_addr, uint8_t soft_i2c_len,unsigned char *soft_i2c_data_buf);
static uint8_t Soft_DMP_I2C_Read(uint8_t soft_dev_addr, uint8_t soft_reg_addr, uint8_t soft_i2c_len,unsigned char *soft_i2c_data_buf);

#define ST_Sensors_I2C_WriteRegister  Soft_DMP_I2C_Write
#define ST_Sensors_I2C_ReadRegister Soft_DMP_I2C_Read

/**
  * @brief  向IIC设备的寄存器连续写入数据，带超时重试设置，供mpu接口调用
  * @param  Address: IIC设备地址
  * @param  RegisterAddr: 寄存器地址
  * @param  RegisterLen: 要写入数据的长度
  * @param  RegisterValue: 要指向写入数据的指针
  * @retval 0正常，非0异常
  */
int Sensors_I2C_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len,
                                        const unsigned char *data_ptr)
{
  char retries=0;
  int ret = 0;
  unsigned short retry_in_mlsec = Get_I2C_Retry();

tryWriteAgain:
  ret = 0;
  ret = ST_Sensors_I2C_WriteRegister( slave_addr, reg_addr, len, ( unsigned char *)data_ptr);

  if(ret && retry_in_mlsec)
  {
    if( retries++ > 4 )
        return ret;

    MPU_Delay_Ms(retry_in_mlsec);
    goto tryWriteAgain;
  }
  return ret;
}


/**
  * @brief  向IIC设备的寄存器连续读出数据,带超时重试设置，供mpu接口调用
  * @param  Address: IIC设备地址
  * @param  RegisterAddr: 寄存器地址
  * @param  RegisterLen: 要读取的数据长度
  * @param  RegisterValue: 指向存储读出数据的指针
  * @retval 0正常，非0异常
  */
int Sensors_I2C_ReadRegister(unsigned char slave_addr,
                                       unsigned char reg_addr,
                                       unsigned short len,
                                       unsigned char *data_ptr)
{
  char retries=0;
  int ret = 0;
  unsigned short retry_in_mlsec = Get_I2C_Retry();

tryReadAgain:
  ret = 0;
  ret = ST_Sensors_I2C_ReadRegister( slave_addr, reg_addr, len, ( unsigned char *)data_ptr);

  if(ret && retry_in_mlsec)
  {
    if( retries++ > 4 )
        return ret;

    MPU_Delay_Ms(retry_in_mlsec);
    goto tryReadAgain;
  }
  return ret;
}

static unsigned short RETRY_IN_MLSEC  = 55;

/**
  * @brief  设置iic重试时间
  * @param  ml_sec：重试的时间，单位毫秒
  * @retval 重试的时间，单位毫秒
  */
void Set_I2C_Retry(unsigned short ml_sec)
{
  RETRY_IN_MLSEC = ml_sec;
}

/**
  * @brief  获取设置的iic重试时间
  * @param  none
  * @retval none
  */
unsigned short Get_I2C_Retry(void)
{
  return RETRY_IN_MLSEC;
}


void Soft_I2C_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
	//
	//
  GPIO_InitStructure.GPIO_Pin = Soft_I2C_SCL | Soft_I2C_SDA;					//配置使用的I2C口
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   //设置I2C口最大允许输出速度
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	  //设置I2C为开漏输出
  GPIO_Init(Soft_I2C_PORT, &GPIO_InitStructure); 
	//
	//
	Soft_I2C_SCL_1; 
	Soft_I2C_SDA_1;
	Soft_I2C_DELAY; 
}

static void Soft_I2C_Delay(uint32_t dly) 
{
	while(--dly);	//dly=100: 8.75us; dly=100: 85.58 us (SYSCLK=72MHz)
}

static uint8_t Soft_I2C_START(void)
{ 
	Soft_I2C_SDA_1; 
 	Soft_I2C_NOP;
  // 
 	Soft_I2C_SCL_1; 
 	Soft_I2C_NOP;    
	//
 	if(!Soft_I2C_SDA_STATE) return Soft_I2C_BUS_BUSY;
	//
 	Soft_I2C_SDA_0;
 	Soft_I2C_NOP;
  //
 	Soft_I2C_SCL_0;  
 	Soft_I2C_NOP; 
	//
 	if(Soft_I2C_SDA_STATE) return Soft_I2C_BUS_ERROR;
	//
 	return Soft_I2C_READY;
}

static void Soft_I2C_STOP(void)
{
 	Soft_I2C_SDA_0; 
 	Soft_I2C_NOP;
  // 
 	Soft_I2C_SCL_1; 
 	Soft_I2C_NOP;    
	//
 	Soft_I2C_SDA_1;
 	Soft_I2C_NOP;
}

static void Soft_I2C_SendACK(void)
{
 	Soft_I2C_SDA_0;
 	Soft_I2C_NOP;
 	Soft_I2C_SCL_1;
 	Soft_I2C_NOP;
 	Soft_I2C_SCL_0; 
 	Soft_I2C_NOP;  
}

static void Soft_I2C_SendNACK(void)
{
	Soft_I2C_SDA_1;
	Soft_I2C_NOP;
	Soft_I2C_SCL_1;
	Soft_I2C_NOP;
	Soft_I2C_SCL_0; 
	Soft_I2C_NOP;  
}




/**
  * @brief  等待应答信号到来
  * @retval 返回值：1，接收应答失败
	*									0，接收应答成功
  */
uint8_t Soft_I2C_Wait_Ack(void)
{
	uint8_t ucErrTime=0;

	Soft_I2C_SDA_1;
	Soft_I2C_NOP;	   
	Soft_I2C_SCL_1;
	Soft_I2C_NOP;	 
	
	while(Soft_I2C_SDA_STATE)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			Soft_I2C_STOP();
			return Soft_I2C_BUS_ERROR;
		}
	}
	Soft_I2C_SCL_0;//时钟输出0 	   
	return 0;  
} 

static uint8_t Soft_I2C_SendByte(uint8_t soft_i2c_data)
{
 	uint8_t i;
 	
	Soft_I2C_SCL_0;
 	for(i=0;i<8;i++)
 	{  
  		if(soft_i2c_data&0x80) Soft_I2C_SDA_1;
   		else Soft_I2C_SDA_0;
			//
  		soft_i2c_data<<=1;
  		Soft_I2C_NOP;
			//
  		Soft_I2C_SCL_1;
  		Soft_I2C_NOP;
  		Soft_I2C_SCL_0;
  		Soft_I2C_NOP; 
 	} 
	return Soft_I2C_Wait_Ack();  
}

static uint8_t Soft_I2C_ReceiveByte(void)
{
	uint8_t i,soft_i2c_data;
	//
 	Soft_I2C_SDA_1;
 	Soft_I2C_SCL_0; 
 	soft_i2c_data=0;
	//
 	for(i=0;i<8;i++)
 	{
  		Soft_I2C_SCL_1;
  		Soft_I2C_NOP; 
  		soft_i2c_data<<=1;
			//
  		if(Soft_I2C_SDA_STATE)	soft_i2c_data|=0x01; 
  
  		Soft_I2C_SCL_0;  
  		Soft_I2C_NOP;         
 	}
	Soft_I2C_SendNACK();
 	return soft_i2c_data;
}

static uint8_t Soft_I2C_ReceiveByte_WithACK(void)
{
	uint8_t i,soft_i2c_data;
	//
 	Soft_I2C_SDA_1;
 	Soft_I2C_SCL_0; 
 	soft_i2c_data=0;
	//
 	for(i=0;i<8;i++)
 	{
  		Soft_I2C_SCL_1;
  		Soft_I2C_NOP; 
  		soft_i2c_data<<=1;
			//
  		if(Soft_I2C_SDA_STATE)	soft_i2c_data|=0x01; 
  
  		Soft_I2C_SCL_0;  
  		Soft_I2C_NOP;         
 	}
	Soft_I2C_SendACK();
 	return soft_i2c_data;
}
static uint8_t Soft_DMP_I2C_Write(uint8_t soft_dev_addr, uint8_t soft_reg_addr, uint8_t soft_i2c_len,unsigned char *soft_i2c_data_buf)
{		
		uint8_t i, result=0;
		Soft_I2C_START();
		result  = Soft_I2C_SendByte(soft_dev_addr << 1 | I2C_Direction_Transmitter);	
		if(result != 0) return result;
	
		result = Soft_I2C_SendByte(soft_reg_addr);  
		if(result != 0) return result;
	
		for (i=0;i<soft_i2c_len;i++) 
		{
			result = Soft_I2C_SendByte(soft_i2c_data_buf[i]); 
			if (result != 0) return result;
		}
		Soft_I2C_STOP();
		return 0x00;
}

static uint8_t Soft_DMP_I2C_Read(uint8_t soft_dev_addr, uint8_t soft_reg_addr, uint8_t soft_i2c_len,unsigned char *soft_i2c_data_buf)
{
		uint8_t result;
	
		Soft_I2C_START();
		result  = Soft_I2C_SendByte(soft_dev_addr << 1 | I2C_Direction_Transmitter);			
		if(result != 0) return result;

		result = Soft_I2C_SendByte(soft_reg_addr); 
		if(result != 0) return result;

		Soft_I2C_START();
		result = Soft_I2C_SendByte(soft_dev_addr << 1 | I2C_Direction_Receiver);
		if(result != 0) return result;
	
		//
    while (soft_i2c_len)
		{
			if (soft_i2c_len==1) *soft_i2c_data_buf =Soft_I2C_ReceiveByte();  
      else *soft_i2c_data_buf =Soft_I2C_ReceiveByte_WithACK();
      soft_i2c_data_buf++;
      soft_i2c_len--;
    }
		Soft_I2C_STOP();
    return 0x00;
}

HalAdapter_Management_IIC_t* p_HalAdapter_IIC;

void MPU_Delay_Ms(u16 ms){
	delay_ms(ms);
}

void MPU_Delay_Us(u32 us)
{
	delay_us(us);
}

void MPU_Get_Ms(unsigned long *time)
{
	get_tick_count(time);
}

void MPU_IIC_Init(void)
{
	p_HalAdapter_IIC = GetHalAdapter_IIC();
}

//产生IIC起始信号
void MPU_IIC_Start(void)
{
	p_HalAdapter_IIC->HAL_IIC_Start();
}	  
//产生IIC停止信号
void MPU_IIC_Stop(void)
{
	p_HalAdapter_IIC->HAL_IIC_Stop();			   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MPU_IIC_Wait_Ack(void)
{
	return p_HalAdapter_IIC->HAL_IIC_Wait_Ack();
} 
//产生ACK应答
void MPU_IIC_Ack(void)
{
	p_HalAdapter_IIC->HAL_IIC_Ack();
}
//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
	p_HalAdapter_IIC->HAL_IIC_NAck();
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(u8 txd)
{                        
  p_HalAdapter_IIC->HAL_IIC_Write_Byte(txd);
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	return p_HalAdapter_IIC->HAL_IIC_Read_Byte(ack);
}


















