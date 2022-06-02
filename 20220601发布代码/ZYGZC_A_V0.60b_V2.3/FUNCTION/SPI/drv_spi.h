#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__


#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
//如果定义了这个则使用软SPI，如果未定义则使用硬SPI
//#define HARD_SOFT_SPI_CHOISE

//SPI引脚定义
//CLK-SCK
#define SPI_CLK_GPIO_PORT			GPIOB
#define SPI_CLK_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_CLK_GPIO_PIN			GPIO_Pin_3//SCK
//MISO-SDO
#define SPI_MISO_GPIO_PORT		GPIOB
#define SPI_MISO_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_MISO_GPIO_PIN			GPIO_Pin_4//SDO
//MOSI-SDI
#define SPI_MOSI_GPIO_PORT		GPIOB
#define SPI_MOSI_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_MOSI_GPIO_PIN			GPIO_Pin_5//SDI
//片选-CS-SEL
#define SPI_NSS_GPIO_PORT			GPIOA
#define SPI_NSS_GPIO_CLK			RCC_APB2Periph_GPIOA
#define SPI_NSS_GPIO_PIN			GPIO_Pin_15

#define spi_set_nss_high( )			SPI_NSS_GPIO_PORT->ODR |= SPI_NSS_GPIO_PIN								//片选置高
#define spi_set_nss_low( )			SPI_NSS_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_NSS_GPIO_PIN ))	//片选置低



#ifdef HARD_SOFT_SPI_CHOISE			/** 只有使用软件SPI才需要的封装 */			

#define spi_set_clk_high( )			SPI_CLK_GPIO_PORT->ODR |= SPI_CLK_GPIO_PIN								//时钟置高
#define spi_set_clk_low( )			SPI_CLK_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_CLK_GPIO_PIN ))	//时钟置低

#define spi_set_mosi_hight( )		SPI_MOSI_GPIO_PORT->ODR |= SPI_MOSI_GPIO_PIN							//发送脚置高
#define spi_set_mosi_low( )			SPI_MOSI_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_MOSI_GPIO_PIN ))	//发送脚置低

#define spi_get_miso( )				(( SPI_MISO_GPIO_PORT->IDR & (uint32_t)SPI_MISO_GPIO_PIN) != SPI_MISO_GPIO_PIN ) ? 0 : 1 // 若相应输入位为低则得到0，相应输入位为高则得到1


//void drv_spi_init( void );
//uint8_t drv_spi_read_write_byte( uint8_t TxByte );
//void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length );


#else									/** 只有使用硬件SPI时会使用 */


//SPI接口定义
#define SPI_PORT					SPI1						//SPI接口
#define SPI_PORT_CLK				RCC_APB2Periph_SPI1			//SPI时钟


void drv_spi_init( void );
uint8_t drv_spi_read_write_byte( uint8_t TxByte );
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length );

#endif

/****************************功能API************************************/
typedef struct
{
	void (*SI4438_Spi_Init)(void);
	uint8_t (*SI4438_Spi_Read_Write_Byte)(uint8_t TxByte);
	void (*SI4438_Spi_Read_Write_String)(uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length);
}SI4438_SPI_API;



#endif

