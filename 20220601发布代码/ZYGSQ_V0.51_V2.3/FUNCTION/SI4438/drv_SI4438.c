#include "drv_SI4438.h"
#include "usart.h"

extern SI4438_SPI_API Si4438_Spi;
extern u8 Si4438ChannelSet;

const static uint8_t config_table[ ] = RADIO_CONFIGURATION_DATA_ARRAY;
 
SI4438_API Si4438_Api_Port;

/**
  * @brief :SI446x等待CTS状态
  * @param :无
  * @note  :无
  * @retval:无
  */ 
void SI446x_Wait_Cts( void )
{
    uint8_t l_Cts;
	uint16_t l_ReadCtsTimes = 0;
	
    do
    {
        SI_SET_CSN_LOW( );		//SPI片选
		
		//读CTS状态
        drv_spi_read_write_byte( READ_CMD_BUFF );
        l_Cts = drv_spi_read_write_byte( 0xFF );		
		
        SI_SET_CSN_HIGH( );		//取消SPI片选
		
		if( 1000 == l_ReadCtsTimes++ )
		{
			SI446x_Init( );
			break;
		}
		
	}while( l_Cts != 0xFF );	//直到读CTS的返回值等于0xFF
}

/**
  * @brief :SI446x写命令
  * @param :
  *			@pCmd:命令首地址
  *			@CmdNumber：命令个数
  * @note  :无
  * @retval:无
  */ 
void SI446x_Write_Cmds( uint8_t *pCmd, uint8_t CmdNumber )
{
    SI446x_Wait_Cts( );			//查询CTS状态
	
    SI_SET_CSN_LOW( );			//SPI片选
	
    while( CmdNumber -- )
    {
        drv_spi_read_write_byte( *pCmd );	//发送命令
		pCmd++;
    }
	
    SI_SET_CSN_HIGH( );			//取消SPI片选
}

/*!
 * This function is used to initialize after power-up the radio chip.
  * @brief :SI446x POWER_UP
  * @param :
  *			@Xo_Freq:晶振频率
  * @note  :SI446x复位之后需要调用
  * @retval:无
  */
void SI446x_Power_Up( uint32_t Xo_Freq )
{
    uint8_t l_Cmd[7] = { 0 };
	
    l_Cmd[0] = POWER_UP;		//Power_Up命令
    l_Cmd[1] = 0x01;
    l_Cmd[2] = 0x00;
    l_Cmd[3] = Xo_Freq >> 24;
    l_Cmd[4] = Xo_Freq >> 16;
    l_Cmd[5] = Xo_Freq >> 8;
    l_Cmd[6] = Xo_Freq;
    SI446x_Write_Cmds( l_Cmd, 7 );	//写命令
}

/**
  * @brief :SI446x复位
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset( void )
{
    SI_SET_SDN_HIGH( );		//关设备
		delay_us( 20 );		//延时 等待设备完全断电
    SI_SET_SDN_LOW( );		//开设备
    SI_SET_CSN_HIGH( );		//取消SPI片选
//	drv_delay_us( 35 );
}
/**
  * @brief :SI446x读CTS和命令应答
  * @param :
  *			@pRead:返回数据首地址
  *			@Length:长度
  * @note  :无
  * @retval:无
  */
void SI446x_Read_Response( uint8_t *pRead, uint8_t Length )
{
    SI446x_Wait_Cts( );		//查询CTS状态
    SI_SET_CSN_LOW( );		//SPI片选
	
	drv_spi_read_write_byte( READ_CMD_BUFF );	//发送读命令
	while( Length-- )
    {
      *pRead = drv_spi_read_write_byte( 0xFF );	//交换数据
			pRead++;
    }
	
    SI_SET_CSN_HIGH( );		//SPI取消片选
}

/*!
 * Sends NOP command to the radio. Can be used to maintain SPI communication.
  * @brief :SI446x空操作
  * @param :无
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Nop( void )
{
    uint8_t l_Cts;
	
    SI_SET_CSN_LOW( );		//SPI片选
	
    l_Cts = drv_spi_read_write_byte( NOP );	//空操作命令
	
    SI_SET_CSN_HIGH( );		//SPI取消片选
	
	return l_Cts;
}

/**
  * @brief :SI446x读设备基本信息
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Part_Informatoin( uint8_t *pRead )
{
    uint8_t l_Cmd = PART_INFO;

    SI446x_Write_Cmds( &l_Cmd, 1 );		//命令
    SI446x_Read_Response( pRead, 8 );	//读设备基本信息
}

/**
  * @brief :SI446x读设备功能版本信息
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Fun_Informatoin( uint8_t *pRead )
{
    uint8_t l_Cmd = FUNC_INFO;

    SI446x_Write_Cmds( &l_Cmd, 1 );		//命令
    SI446x_Read_Response( pRead, 7 );	//读设备功能版本信息
}

/**
  * @brief :SI446x读中断状态
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Interrupt_Status( uint8_t *pRead )
{
    uint8_t l_Cmd[ 4 ] = { 0 };
	
    l_Cmd[0] = GET_INT_STATUS;		
    l_Cmd[1] = 0;
    l_Cmd[2] = 0;
    l_Cmd[3] = 0;

    SI446x_Write_Cmds( l_Cmd, 4 );		//发送中断读取命令
    SI446x_Read_Response( pRead, 9 );	//读取状态 回复流9字节
}

/**
  * @brief :SI446x读取属性值
  * @param :
  *			@Group_Num:属性组(参考SI446X_PROPERTY)
  *			@Num_Props:读取的属性个数
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Property( SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pRead  )
{
    uint8_t l_Cmd[ 4 ] = { 0 };

    l_Cmd[ 0 ] = GET_PROPERTY;
    l_Cmd[ 1 ] = Group_Num >> 8;
    l_Cmd[ 2 ] = Num_Props;
    l_Cmd[ 3 ] = Group_Num;

    SI446x_Write_Cmds( l_Cmd, 4 );		//发送读取属性命令
    SI446x_Read_Response( pRead, Num_Props + 1 );	//读属性
}

/**
  * @brief :SI446x设置属性
  * @param :
  *			@Group_Num:属性组(参考SI446X_PROPERTY)
  *			@Num_Props:设置的属性个数
  *			@pWrite:写地址设备
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property( SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pWrite )
{
    uint8_t l_Cmd[ 20 ] = { 0 }, i = 0;
	
    if( Num_Props >= 16 )   
	{ 
		return; 	//数量不大于16
	}
	
    l_Cmd[ i++ ] = SET_PROPERTY;		//设置属性命令
    l_Cmd[ i++ ] = Group_Num >> 8;
    l_Cmd[ i++ ] = Num_Props;
    l_Cmd[ i++ ] = Group_Num;
	
    while( Num_Props-- )
    {
        l_Cmd[ i++ ] = *pWrite;
		pWrite++;
    }
    SI446x_Write_Cmds( l_Cmd, i );		//发送命令及数据
}

/**
  * @brief :SI446x设置属性组1属性
  * @param :
  *			@ Group_Num:属性组
  *			@ Start_Prop:开始设置的属性号(参考SI446X_PROPERTY)
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property_1(SI446X_PROPERTY Group_Num, uint8_t Start_Prop)
{
    uint8_t l_Cmd[ 5 ] = { 0 };

    l_Cmd[ 0 ] = SET_PROPERTY;		//命令0x11
    l_Cmd[ 1 ] = Group_Num >> 8;	//属性组
    l_Cmd[ 2 ] = 1;								//要写入的连续属性的数量
    l_Cmd[ 3 ] = Group_Num;				//选择指定组内属性的偏移号或索引
    l_Cmd[ 4 ] = Start_Prop;			//数据
	
    SI446x_Write_Cmds( l_Cmd, 5 );	//发送命令设置属性
}
/**
  * @brief :SI446x设置属性组2属性
  * @param :
  *			@ Group_Num:属性组
  *			@ Start_Prop:开始设置的属性号(参考SET_PROPERTY)
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property_2(SI446X_PROPERTY Group_Num, uint16_t Start_Prop)
{
    uint8_t l_Cmd[ 6 ] = { 0 };

    l_Cmd[ 0 ] = SET_PROPERTY;		//命令0x11
    l_Cmd[ 1 ] = Group_Num>>8;		//属性组
    l_Cmd[ 2 ] = 2;								//要写入的连续属性的数量
    l_Cmd[ 3 ] = Group_Num;				//选择指定组内属性的偏移号或索引
    l_Cmd[ 4 ] = Start_Prop>>8;		//数据0
		l_Cmd[ 5 ] = Start_Prop;			//数据1
	
    SI446x_Write_Cmds( l_Cmd,6);	//发送命令设置属性
}
/**
  * @brief :SI446x设置属性组3属性
  * @param :
  *			@ Group_Num:属性组
  *			@ Start_Prop:开始设置的属性号(参考SET_PROPERTY)
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property_3(SI446X_PROPERTY Group_Num, uint32_t Start_Prop)
{
    uint8_t l_Cmd[ 7 ] = { 0 };

    l_Cmd[ 0 ] = SET_PROPERTY;		//命令0x11
    l_Cmd[ 1 ] = Group_Num >> 8;	//属性组
    l_Cmd[ 2 ] = 3;								//要写入的连续属性的数量
    l_Cmd[ 3 ] = Group_Num;				//选择指定组内属性的偏移号或索引
    l_Cmd[ 4 ] = Start_Prop>>16;	//数据0
		l_Cmd[ 5 ] = Start_Prop>>8;		//数据1
		l_Cmd[ 6 ] = Start_Prop;			//数据2
	
    SI446x_Write_Cmds( l_Cmd,7);	//发送命令设置属性
}

///*!
// * Send SET_PROPERTY command to the radio.
// *
// * @param GROUP       Property group.
// * @param NUM_PROPS   Number of property to be set. The properties must be in ascending order
// *                    in their sub-property aspect. Max. 12 properties can be set in one command.
// * @param START_PROP  Start sub-property address.
// */
//void si446x_set_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, ... )
//{
//    va_list argList;
//    U8 cmdIndex;

//    Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
//    Pro2Cmd[1] = GROUP;
//    Pro2Cmd[2] = NUM_PROPS;
//    Pro2Cmd[3] = START_PROP;

//    va_start (argList, START_PROP);
//    cmdIndex = 4;
//    while(NUM_PROPS--)
//    {
//        Pro2Cmd[cmdIndex] = va_arg (argList, U8);
//        cmdIndex++;
//    }
//    va_end(argList);

//    radio_comm_SendCmd( cmdIndex, Pro2Cmd );
//}

/**
  * @brief :SI446x读取属性组1属性
  * @param :
  *			@Group_Num:开始的属性号(参考SI446X_PROPERTY)
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Get_Property_1( SI446X_PROPERTY Group_Num )
{
    uint8_t l_Cmd[ 4 ] = { 0 };

    l_Cmd[ 0 ] = GET_PROPERTY;
    l_Cmd[ 1 ] = Group_Num >> 8;
    l_Cmd[ 2 ] = 1;
    l_Cmd[ 3 ] = Group_Num;
    SI446x_Write_Cmds( l_Cmd, 4 );		//发送命令
	
    SI446x_Read_Response( l_Cmd, 2 );	//读取属性
	
    return l_Cmd[ 1 ];
}

/*!
 * Send GPIO pin config command to the radio and reads the answer into
 * @Si446xCmd union.
 *
 * @param GPIO0       GPIO0 configuration.
 * @param GPIO1       GPIO1 configuration.
 * @param GPIO2       GPIO2 configuration.
 * @param GPIO3       GPIO3 configuration.
 * @param NIRQ        NIRQ configuration.
 * @param SDO         SDO configuration.
 * @param GEN_CONFIG  General pin configuration.
 */
void SI446x_Config_Gpio( uint8_t Gpio_0, uint8_t Gpio_1, uint8_t Gpio_2, uint8_t Gpio_3, uint8_t Irq, uint8_t Sdo, uint8_t Gen_Config )
{
    uint8_t l_Cmd[ 10] = { 0 };
	
    l_Cmd[ 0 ] = GPIO_PIN_CFG;
    l_Cmd[ 1 ] = Gpio_0;
    l_Cmd[ 2 ] = Gpio_1;
    l_Cmd[ 3 ] = Gpio_2;
    l_Cmd[ 4 ] = Gpio_3;
    l_Cmd[ 5 ] = Irq;
    l_Cmd[ 6 ] = Sdo;
    l_Cmd[ 7 ] = Gen_Config;
	
    SI446x_Write_Cmds( l_Cmd, 8 );		//写配置
    SI446x_Read_Response( l_Cmd, 8 );	//读配置
}

/**
  * @brief :SI446x模块配置
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Config_Init( void )
{
    uint8_t i;
    uint16_t j = 0;

    while( ( i = config_table[j] ) != 0 )
    {
        j += 1;
        SI446x_Write_Cmds( (uint8_t *)config_table + j, i );
        j += i;
    }
#if PACKET_LENGTH > 0           //固定数据长度
	
    SI446x_Set_Property_1( PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH );
	
#else                           //动态数据长度
	
    SI446x_Set_Property_1( PKT_CONFIG1, 0x00 );
    SI446x_Set_Property_1( PKT_CRC_CONFIG, 0x00 );
    SI446x_Set_Property_1( PKT_LEN_FIELD_SOURCE, 0x01 );
    SI446x_Set_Property_1( PKT_LEN, 0x2A );
    SI446x_Set_Property_1( PKT_LEN_ADJUST, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_1_LENGTH_12_8, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_1_LENGTH_7_0, 0x01 );
    SI446x_Set_Property_1( PKT_FIELD_1_CONFIG, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_1_CRC_CONFIG, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_2_LENGTH_12_8, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_2_LENGTH_7_0, 0x20 );
    SI446x_Set_Property_1( PKT_FIELD_2_CONFIG, 0x00 );
    SI446x_Set_Property_1( PKT_FIELD_2_CRC_CONFIG, 0x00 );
	
#endif 
	
	//4463 的GDO2 GDO3控制射频开关 33 32
	// 发射：GDO2 = 0, GDO3 = 1
	// 接收：GDO2 = 1, GDO3 = 0
  // SI446x_Config_Gpio( 0, 0, 33|0x40, 32|0x40, 0, 0, 0 );    //4463才需要配置
}

/**
  * @brief :SI446x写TX FIFO
  * @param :
  *			@pWriteData：写数据首地址
  *			@Length：数据长度
  * @note  :无
  * @retval:无
  */
void SI446x_Write_TxFifo( uint8_t *pWriteData, uint8_t Length )
{
    SI_SET_CSN_LOW( );
    drv_spi_read_write_byte( WRITE_TX_FIFO );		//写命令
    while( Length-- )    
	{ 
		drv_spi_read_write_byte( *pWriteData++ ); 	//写数据
	}
    SI_SET_CSN_HIGH( );
}

/**
  * @brief :SI446x 复位RX FIFO
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset_RxFifo( void )
{
    uint8_t l_Cmd[ 2 ] = { 0 };

    l_Cmd[ 0 ] = FIFO_INFO;
    l_Cmd[ 1 ] = 0x02;
    SI446x_Write_Cmds( l_Cmd, 2 );
}

/**
  * @brief :SI446x 复位TX FIFO
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset_TxFifo( void )
{
    uint8_t l_Cmd[ 2 ] = { 0 };

    l_Cmd[0] = FIFO_INFO;
    l_Cmd[1] = 0x02;
    SI446x_Write_Cmds( l_Cmd, 2 );
}

/**
  * @brief :SI446x发送数据包
  * @param :
  *			@ Channel		：通道
  *			@ Condition	：开始TX条件
  *			@ pTxData		：发送数据首地址
  *			@ Length		：数据长度
  * @note  :无
  * @retval:无
  */
void SI446x_Send_Packet(uint8_t Channel, uint8_t Condition,uint8_t *pTxData, uint8_t Length)
{
    uint8_t tx_len = Length;

    SI446x_Reset_TxFifo( );		//清空TX FIFO

    SI_SET_CSN_LOW( );		
	
    drv_spi_read_write_byte( WRITE_TX_FIFO );	//写TX FIFO命令
	
#if PACKET_LENGTH == 0			//动态数据长度
	
    tx_len ++;
    drv_spi_read_write_byte( Length );
	
#endif
	
  while( Length-- )    
	{
		drv_spi_read_write_byte( *pTxData++ ); 	//写数据到TX FIFO
	}
	
    SI_SET_CSN_HIGH( );//片选拉高
		
		SI446x_Start_Tx(Channel,Condition,tx_len);//启动发送 210204
}

/**
  * @brief :SI446x启动发送
  * @param :
  *			@ Length		：数据长度
  *			@ Channel		：通道
  *			@ Condition	：开始TX条件
  * @note  :无
  * @retval:无
  */
void SI446x_Start_Tx( uint8_t Channel, uint8_t Condition, uint16_t Length )
{
    uint8_t l_Cmd[7] = { 0 };

    l_Cmd[ 0 ] = START_TX;		
    l_Cmd[ 1 ] = Channel;
    l_Cmd[ 2 ] = Condition;
    l_Cmd[ 3 ] = (u8)Length >> 8;
    l_Cmd[ 4 ] = (u8)Length;
		l_Cmd[ 5 ]=0x00;//210204
		l_Cmd[ 6 ]=0x00;//210207
    SI446x_Write_Cmds( l_Cmd, 7);
}

/**
  * @brief :SI446x读RX FIFO数据
  * @param :
  *			@pRxData：数据首地址
  * @note  :无
  * @retval:数据个数
  */
uint8_t SI446x_Read_Packet( uint8_t *pRxData )
{
    uint8_t length = 0, i = 0;
	
    SI446x_Wait_Cts( );
    SI_SET_CSN_LOW( );

    drv_spi_read_write_byte( READ_RX_FIFO );	//读FIFO命令
	
#if PACKET_LENGTH == 0
	
    length = drv_spi_read_write_byte( 0xFF );	//读数据长度
	
#else
	
    length = PACKET_LENGTH;
	
#endif
    i = length;

    while( length -- )
    {
        *pRxData++ = drv_spi_read_write_byte( 0xFF );	//读数据
    }
	
    SI_SET_CSN_HIGH( );		//返回数据个数
	
    return i;
}

/**
  * @brief :SI446x启动接收
  * @param :
  *			@Channel：通道
  *			@Condition：开始接收状态
  *			@Length：接收长度
  *			@Next_State1：当前导超时发生时的下一个状态
  *			@Next_State2：当收到一个有效的数据包时的下一个状态
  *			@Next_State3：接收到无效数据包时的下一个状态(如CRC错误)
  * @note  :无
  * @retval:无
  */
void SI446x_Start_Rx( uint8_t Channel, uint8_t Condition, uint16_t Length, uint8_t Next_State1, uint8_t Next_State2, uint8_t Next_State3 )
{
    uint8_t l_Cmd[ 8 ] = { 0 };
	
    SI446x_Reset_RxFifo( );
    SI446x_Reset_TxFifo( );
	
    l_Cmd[ 0 ] = START_RX;
    l_Cmd[ 1 ] = Channel;
    l_Cmd[ 2 ] = Condition;
    l_Cmd[ 3 ] = (u8)Length >> 8;
    l_Cmd[ 4 ] = (u8)Length;
    l_Cmd[ 5 ] = Next_State1;
    l_Cmd[ 6 ] = Next_State2;
    l_Cmd[ 7 ] = Next_State3;
	
    SI446x_Write_Cmds( l_Cmd, 8 );
}

/**
  * @brief :SI446x读取当前数据包信息
  * @param :
  *			@pReadData：数据存放地址
  *			@FieldNumMask：掩码域
  *			@Length：长度
  *			@DiffLen：不同长度
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Packet_Information( uint8_t *pReadData, uint8_t FieldNumMask, uint16_t Length, uint16_t DiffLen )
{
    uint8_t l_Cmd[ 6 ] = { 0 };
	
    l_Cmd[ 0 ] = PACKET_INFO;
    l_Cmd[ 1 ] = FieldNumMask;
    l_Cmd[ 2 ] = Length >> 8;
    l_Cmd[ 3 ] = Length;
    l_Cmd[ 4 ] = DiffLen >> 8;
    l_Cmd[ 5 ] = DiffLen;

    SI446x_Write_Cmds( l_Cmd, 6 );
    SI446x_Read_Response( pReadData, 3 );
}

/**
  * @brief :SI446x读取FIFO状态
  * @param :
  *			@pReadData：数据存放地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Fifo_Information( uint8_t *pReadData )
{
    uint8_t l_Cmd[ 2 ] = { 0 };
	
    l_Cmd[ 0 ] = FIFO_INFO;
    l_Cmd[ 1 ] = 0x03;

    SI446x_Write_Cmds( l_Cmd, 2 );
    SI446x_Read_Response( pReadData, 3 );
}

/**
  * @brief :SI446x更改状态命令 210207
  * @param :
  *			@NextStatus：下一个状态
  * @note  :无
  * @retval:无
  */
void SI446x_Change_Status( uint8_t NextStatus )
{
    uint8_t l_Cmd[ 2 ] = { 0 };
	
    l_Cmd[ 0 ] = CHANGE_STATE;
    l_Cmd[ 1 ] = NextStatus;
	
    SI446x_Write_Cmds( l_Cmd, 2 );
}

/**
  * @brief :SI446x获取设备当前状态
  * @param :
  * @note  :无
  * @retval:设备当前状态
  */
uint8_t SI446x_Get_Device_Status( void )
{
   uint8_t l_Cmd[ 3 ] = { 0 };
   
   l_Cmd [ 0 ] = REQUEST_DEVICE_STATE;
   
   SI446x_Write_Cmds( l_Cmd, 1 );
   SI446x_Read_Response( l_Cmd, 3 );
   
   return l_Cmd[ 1 ] & 0x0F;
}

/**
  * @brief :SI446x功率设置
  * @param :
  *			@PowerLevel：数据存放地址
  * @note  :无
  * @retval:设备当前状态
  */
void SI446x_Set_Power( uint8_t PowerLevel )
{
    SI446x_Set_Property_1( PA_PWR_LVL, PowerLevel );
}

/**
  * @brief :SI446x的TX数据速率设置
  * @param :
  *			@PowerLevel：数据存放地址
  * @note  :无
  * @retval:设备当前状态
  */
void SI446x_Set_Data_Rate( uint32_t DataRate )
{
    SI446x_Set_Property_3(MODEM_DATA_RATE_2, DataRate);
}


/**
  * @brief :SI446x引脚初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Gpio_Init( void )
{
	GPIO_InitTypeDef GpioInitStructer;
	
	//打开引脚端口时钟
	RCC_APB2PeriphClockCmd( SI4463_SDN_CLK | SI4463_IRQ_CLK, ENABLE );
	
	//SDN 引脚配置为推挽输出
	GpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	GpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	GpioInitStructer.GPIO_Pin = SI4463_SDN_PIN;
	GPIO_Init( SI4463_SDN_PORT, &GpioInitStructer );	
	
	//IRQ GPIO0~GPIO3输入 可做外部信号中断输入 Demo程序采用查询方式 未配置成中断
	GpioInitStructer.GPIO_Mode = GPIO_Mode_IPU;
	
	GpioInitStructer.GPIO_Pin = SI4463_IRQ_PIN;
	GPIO_Init( SI4463_IRQ_PORT, &GpioInitStructer );	//IRQ
	
//	RCC_APB2PeriphClockCmd(SI4463_GPIO0_CLK | SI4463_GPIO1_CLK | SI4463_GPIO2_CLK | SI4463_GPIO3_CLK, ENABLE );
//	GpioInitStructer.GPIO_Pin = SI4463_GPIO0_PIN;
//	GPIO_Init( SI4463_GPIO0_PORT, &GpioInitStructer );	//GPIO0
//	
//	GpioInitStructer.GPIO_Pin = SI4463_GPIO1_PIN;
//	GPIO_Init( SI4463_GPIO1_PORT, &GpioInitStructer );	//GPIO1
//	
//	GpioInitStructer.GPIO_Pin = SI4463_GPIO2_PIN;
//	GPIO_Init( SI4463_GPIO2_PORT, &GpioInitStructer );	//GPIO2
//	
//	GpioInitStructer.GPIO_Pin = SI4463_GPIO3_PIN;
//	GPIO_Init( SI4463_GPIO3_PORT, &GpioInitStructer );	//GPIO3
	
	GPIO_ResetBits( SI4463_SDN_PORT, SI4463_SDN_PIN );	//SDN 置低 使能设备
}

/************************************************************  	
函数名称：	SI44xx_Power_Init
函数功能：	SI446x电源控制引脚初始化
入口参数：	void
返回参数：	无
说明：	    无
***********************************************************/ 
void SI44xx_Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//4438电源通
}

/**
  * @brief :SI446x初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Init( void )
{
	Si4438_Spi.SI4438_Spi_Init();										//SPI初始化
	Si4438_Api_Port.SI4438_Gpio_Init();							//SI4438引脚初始化
	Si4438_Api_Port.SI4438_Reset();									//SI4438复位
	//reset 后需要Power up设备 晶振30MHz
	Si4438_Api_Port.SI4438_Power_Up(RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ);
	Si4438_Api_Port.SI4438_Config_Init();						//SI4438模块初始化
//	Si4438_Api_Port.SI4438_Set_DataRate(5000000);		//设置TX通信速率500kbps TESTOK 210218
	Si4438_Api_Port.SI4438_Set_Power(0x7f);					//功率设置
	Si4438_Api_Port.SI4438_Status_Set(RXTUNE_STATE);//切换到RX状态
//		printf("787777\r\n");
	while( RXTUNE_STATE != Si4438_Api_Port.SI4438_Get_Device_Status());
//	printf("787777\r\n");
	Si4438_Api_Port.SI4438_Start_Rx(Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3);
}
//void SI446x_Init( void )
//{
//	drv_spi_init( );//SPI初始化
//	SI446x_Gpio_Init();			//SI4463引脚初始化
//	SI446x_Reset();					//SI4463复位
//	SI446x_Power_Up(RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ);//reset 后需要Power up设备 晶振30MHz
//	SI446x_Config_Init();		//SI4463模块初始化
	
//	SI446x_Set_Data_Rate(0xf4240);
//	SI446x_Set_Power(0x7F);	//功率设置
//	SI446x_Change_Status( RXTUNE_STATE );	//切换到RX状态
//	while( RXTUNE_STATE != SI446x_Get_Device_Status( ));
//	SI446x_Start_Rx( Si4438ChannelSet, 0, PACKET_LENGTH,0,0,3 );
//}


SI4438_API Si4438_Api_Port={
	.SI4438_Gpio_Init=SI446x_Gpio_Init,
	.SI4438_Reset=SI446x_Reset,
	.SI4438_Power_Up=SI446x_Power_Up,
	.SI4438_Config_Init=SI446x_Config_Init,
	.SI4438_Set_DataRate=SI446x_Set_Data_Rate,//设置TX数据速率
	.SI4438_Set_Power=SI446x_Set_Power,//设置功率
	.SI4438_Get_Device_Status=SI446x_Get_Device_Status,
	.SI4438_Init=SI446x_Init,
	.SI4438_Status_Set=SI446x_Change_Status,
	.SI4438_Start_Rx=SI446x_Start_Rx,
	.SI4438_Start_Tx=SI446x_Start_Tx,
	.SI4438_Send_Packet=SI446x_Send_Packet,
	.SI4438_Read_Packet=SI446x_Read_Packet,
};

