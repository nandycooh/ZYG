#ifndef __DRV_SI4463_H__
#define __DRV_SI4463_H__


#include "drv_spi.h"
#include "delay.h"
#include "radio_config_Si4438.h"

//#define 	SI4438_CHANNEL_SET 		14	//信道设置
//u8 SI4438_CHANNEL_SET=14;

#define  	PACKET_LENGTH      		0 		//0-64,  0:动态长度 1:固定长度

#define SI4463_SDN_PORT				GPIOA									
#define SI4463_SDN_CLK				RCC_APB2Periph_GPIOA
#define SI4463_SDN_PIN				GPIO_Pin_12

#define SI4463_IRQ_PORT				GPIOB									
#define SI4463_IRQ_CLK				RCC_APB2Periph_GPIOB
#define SI4463_IRQ_PIN				GPIO_Pin_1

//IO操作函数定义
#define SI_SET_SDN_HIGH( )			SI4463_SDN_PORT->ODR |= SI4463_SDN_PIN
#define SI_SET_SDN_LOW( )			SI4463_SDN_PORT->ODR &= (uint32_t)((uint32_t)( ~SI4463_SDN_PIN ))

#define SI_SET_CSN_HIGH( )			spi_set_nss_high( )
#define SI_SET_CSN_LOW( )			spi_set_nss_low( )


/** SI4463 命令定义 */
typedef enum
{
    /*BOOT COMMANDS-------------------------------------*/
	POWER_UP                = 0x02, // 上电

    /*COMMON COMMANDS-----------------------------------*/
    NOP                     = 0x00, //没有操作命令.
    PART_INFO               = 0x01, //报告设备的基本信息.
    FUNC_INFO               = 0x10, //返回设备的函数修订信息.
    GET_INT_STATUS          = 0x20, //返回所有可能中断事件的中断状态(状态和待定)。
																		//可选地，它可以用于清除挂起的中断事件.
    SET_PROPERTY            = 0x11, //设置属性的值.
    GET_PROPERTY            = 0x12, //检索属性的值.
    FIFO_INFO               = 0x15, //提供访问发送和接收FIFO计数和重置.
    GPIO_PIN_CFG            = 0x13, //配置gpio引脚.
    CHANGE_STATE            = 0x34, //更新状态机条目.
    REQUEST_DEVICE_STATE    = 0x33, //请求当前设备状态.
    READ_CMD_BUFF           = 0x44, //用于读取CTS和命令响应.
    FRR_A_READ              = 0x50, //读取以A开始的快速响应寄存器.
    FRR_B_READ              = 0x51, //读取从B开始的快速响应寄存器.
    FRR_C_READ              = 0x53, //读取从C开始的快速响应寄存器.
    FRR_D_READ              = 0x57, //读取从D开始的快速响应寄存器.

    /*IR_CAL_COMMANDS----------------------------------*/
    IRCAL                   = 0x17, //镜频抑制校准.

    /*TX_COMMANDS--------------------------------------*/
    START_TX                = 0x31, //切换到TX状态，开始数据包传输.
    WRITE_TX_FIFO           = 0x66, //写TX FIFO.

    /*RX_COMMANDS--------------------------------------*/
    GET_MODEM_STATUS        = 0x22, //返回调制解调器中断组的中断状态(状态和待定)。
																		//可选地，它可以被用于清除锁存(挂起)中断事件。
    PACKET_INFO             = 0x16, //返回关于接收到的最后一个数据包的信息可选覆盖字段长度。
    START_RX                = 0x32, //切换到RX状态。但是命令参数被保留休眠状态，所以只有当它们改变时才需要写入。
    RX_HOP                  = 0x36, //在RX时跳到一个新的频率.
    READ_RX_FIFO            = 0x77, //读RX_FIFO

    /*ADVANCED_COMMANDS-------------------------------*/
    GET_PH_STATUS           = 0x21, //返回数据包处理程序中断的中断状态组(状态和挂起)。
																		//可选地，它可以被使用清除锁存(挂起)中断事件。
    GET_CHIP_STATUS         = 0x23, //返回芯片中断组的中断状态(状态和待定)。
																		//可选地，它可以用于清除挂起的中断事件.

    PROTOCOL_CFG            = 0x18, //为指定的协议设置芯片.
    GET_ADC_READING         = 0x14  //执行和检索可能ADC转换的结果.

}SI446X_CMD_X;

/** SI4463属性定义 */
typedef enum
{
    GLOBAL_XO_TUNE          = 0x0000,
    GLOBAL_CLK_CFG          = 0x0001,
    GLOBAL_LOW_BATT_THRESH  = 0x0001,
    GLOBAL_CONFIG           = 0x0003,
    GLOBAL_WUT_CONFIG       = 0x0004,
    GLOBAL_WUT_M_15_8       = 0x0005,
    GLOBAL_WUT_M_7_0        = 0x0006,
    GLOBAL_WUT_R            = 0x0007,
    GLOBAL_WUT_LDC          = 0x0008,
    GLOBAL_WUT_CAL          = 0x0009,

    INT_CTL_ENABLE          = 0x0100,
    INT_CTL_PH_ENABLE       = 0x0101,
    INT_CTL_MODEM_ENABLE    = 0x0102,
    INT_CTL_CHIP_ENABLE     = 0x0103,

    FRR_CTL_A_MODE          = 0x0200,
    FRR_CTL_B_MODE          = 0x0201,
    FRR_CTL_C_MODE          = 0x0202,
    FRR_CTL_D_MODE          = 0x0203,

    PREAMBLE_TX_LENGTH      = 0x1000,
    PREAMBLE_CONFIG_STD_1   = 0x1001,
    PREAMBLE_CONFIG_NSTD    = 0x1002,
    PREAMBLE_CONFIG_STD_2   = 0x1003,
    PREAMBLE_CONFIG         = 0x1004,
    PREAMBLE_PATTERN_31_24  = 0x1005,
    PREAMBLE_PATTERN_23_16  = 0x1006,
    PREAMBLE_PATTERN_15_8   = 0x1007,
    PREAMBLE_PATTERN_7_0    = 0x1008,
    PREAMBLE_POSTAMBLE_CONFIG           = 0x1009,
    PREAMBLE_POSTAMBLE_PATTERN_31_24    = 0x100A,
    PREAMBLE_POSTAMBLE_PATTERN_23_16	= 0x100B,
    PREAMBLE_POSTAMBLE_PATTERN_15_8     = 0x100C,
    PREAMBLE_POSTAMBLE_PATTERN_7_0      = 0x100D,

    SYNC_CONFIG             = 0x1100,
    SYNC_BITS_31_24         = 0x1101,
    SYNC_BITS_23_16         = 0x1102,
    SYNC_BITS_15_8          = 0x1103,
    SYNC_BITS_7_0           = 0x1104,

    PKT_CRC_CONFIG          = 0x1200,
    PKT_CONFIG1             = 0x1206,
    PKT_LEN                 = 0x1208,
    PKT_LEN_FIELD_SOURCE    = 0x1209,
    PKT_LEN_ADJUST          = 0x120A,
    PKT_TX_THRESHOLD        = 0x120B,
    PKT_RX_THRESHOLD        = 0x120C,
    PKT_FIELD_1_LENGTH_12_8 = 0x120D,
    PKT_FIELD_1_LENGTH_7_0  = 0x120E,
    PKT_FIELD_1_CONFIG      = 0x120F,
    PKT_FIELD_1_CRC_CONFIG  = 0x1210,
    PKT_FIELD_2_LENGTH_12_8 = 0x1211,
    PKT_FIELD_2_LENGTH_7_0  = 0x1212,
    PKT_FIELD_2_CONFIG      = 0x1213,
    PKT_FIELD_2_CRC_CONFIG  = 0x1214,
    PKT_FIELD_3_LENGTH_12_8 = 0x1215,
    PKT_FIELD_3_LENGTH_7_0  = 0x1216,
    PKT_FIELD_3_CONFIG      = 0x1217,
    PKT_FIELD_3_CRC_CONFIG  = 0x1218,
    PKT_FIELD_4_LENGTH_12_8 = 0x1219,
    PKT_FIELD_4_LENGTH_7_0  = 0x121A,
    PKT_FIELD_4_CONFIG      = 0x121B,
    PKT_FIELD_4_CRC_CONFIG  = 0x121C,
    PKT_FIELD_5_LENGTH_12_8 = 0x121D,
    PKT_FIELD_5_LENGTH_7_0  = 0x121E,
    PKT_FIELD_5_CONFIG      = 0x121F,
    PKT_FIELD_5_CRC_CONFIG  = 0x1220,
    PKT_RX_FIELD_1_LENGTH_12_8  = 0x1221,
    PKT_RX_FIELD_1_LENGTH_7_0   = 0x1222,
    PKT_RX_FIELD_1_CONFIG       = 0x1223,
    PKT_RX_FIELD_1_CRC_CONFIG   = 0x1224,
    PKT_RX_FIELD_2_LENGTH_12_8  = 0x1225,
    PKT_RX_FIELD_2_LENGTH_7_0   = 0x1226,
    PKT_RX_FIELD_2_CONFIG   = 0x1227,
    PKT_RX_FIELD_2_CRC_CONFIG   = 0x1228,
    PKT_RX_FIELD_3_LENGTH_12_8	= 0x1229,
    PKT_RX_FIELD_3_LENGTH_7_0   = 0x122A,
    PKT_RX_FIELD_3_CONFIG       = 0x122B,
    PKT_RX_FIELD_3_CRC_CONFIG   = 0x122C,
    PKT_RX_FIELD_4_LENGTH_12_8	= 0x122D,
    PKT_RX_FIELD_4_LENGTH_7_0   = 0x122E,
    PKT_RX_FIELD_4_CONFIG   = 0x122F,
    PKT_RX_FIELD_4_CRC_CONFIG   = 0x1230,
    PKT_RX_FIELD_5_LENGTH_12_8	= 0x1231,
    PKT_RX_FIELD_5_LENGTH_7_0   = 0x1232,
    PKT_RX_FIELD_5_CONFIG       = 0x1233,
    PKT_RX_FIELD_5_CRC_CONFIG   = 0x1234,

    MODEM_MOD_TYPE          = 0x2000,
    MODEM_MAP_CONTROL       = 0x2001,
    MODEM_DATA_RATE_2       = 0x2003,
    MODEM_DATA_RATE_1       = 0x2004,
    MODEM_DATA_RATE_0       = 0x2005,
    MODEM_TX_NCO_MODE_3     = 0x2006,
    MODEM_TX_NCO_MODE_2     = 0x2007,
    MODEM_TX_NCO_MODE_1     = 0x2008,
    MODEM_TX_NCO_MODE_0     = 0x2009,
    MODEM_FREQ_DEV_2        = 0x200A,
    MODEM_FREQ_DEV_1        = 0x200B,
    MODEM_FREQ_DEV_0        = 0x200C,
    MODEM_FREQ_OFFSET_1     = 0x200D,
    MODEM_FREQ_OFFSET_0     = 0x200E,
    MODEM_TX_FILTER_COEFF_8 = 0x200F,
    MODEM_TX_FILTER_COEFF_7 = 0x2010,
    MODEM_TX_FILTER_COEFF_6 = 0x2011,
    MODEM_TX_FILTER_COEFF_5 = 0x2012,
    MODEM_TX_FILTER_COEFF_4 = 0x2013,
    MODEM_TX_FILTER_COEFF_3 = 0x2014,
    MODEM_TX_FILTER_COEFF_2 = 0x2015,
    MODEM_TX_FILTER_COEFF_1 = 0x2016,
    MODEM_TX_FILTER_COEFF_0 = 0x2017,
    MODEM_TX_RAMP_DELAY     = 0x2018,
    MODEM_MDM_CTRL          = 0x2019,
    MODEM_IF_CONTROL        = 0x201A,
    MODEM_IF_FREQ_2         = 0x201B,
    MODEM_IF_FREQ_1         = 0x201C,
    MODEM_IF_FREQ_0         = 0x201D,
    MODEM_DECIMATION_CFG1   = 0x201E,
    MODEM_DECIMATION_CFG0   = 0x201F,
    MODEM_BCR_OSR_1         = 0x2022,
    MODEM_BCR_OSR_0         = 0x2023,
    MODEM_BCR_NCO_OFFSET_2  = 0x2024,
    MODEM_BCR_NCO_OFFSET_1  = 0x2025,
    MODEM_BCR_NCO_OFFSET_0  = 0x2026,
    MODEM_BCR_GAIN_1        = 0x2027,
    MODEM_BCR_GAIN_0        = 0x2028,
    MODEM_BCR_GEAR          = 0x2029,
    MODEM_BCR_MISC1         = 0x202A,
    MODEM_BCR_MISC0         = 0x202B,
    MODEM_AFC_GEAR          = 0x202C,
    MODEM_AFC_WAIT          = 0x202D,
    MODEM_AFC_GAIN_1        = 0x202E,
    MODEM_AFC_GAIN_0        = 0x202F,
    MODEM_AFC_LIMITER_1     = 0x2030,
    MODEM_AFC_LIMITER_0     = 0x2031,
    MODEM_AFC_MISC          = 0x2032,
    MODEM_AFC_ZIFOFF        = 0x2033,
    MODEM_ADC_CTRL          = 0x2034,
    MODEM_AGC_WINDOW_SIZE   = 0x2038,
    MODEM_AGC_RFPD_DECAY    = 0x2039,
    MODEM_AGC_IFPD_DECAY    = 0x203A,
    MODEM_FSK4_GAIN1        = 0x203B,
    MODEM_FSK4_GAIN0        = 0x203C,
    MODEM_FSK4_TH1          = 0x203D,
    MODEM_FSK4_TH0          = 0x203E,
    MODEM_FSK4_MAP          = 0x203F,
    MODEM_OOK_PDTC          = 0x2040,
    MODEM_OOK_BLOPK         = 0x2041,
    MODEM_OOK_CNT1          = 0x2042,
    MODEM_OOK_MISC          = 0x2043,
    MODEM_RAW_SEARCH        = 0x2044,
    MODEM_RAW_CONTROL       = 0x2045,
    MODEM_RAW_EYE_1         = 0x2046,
    MODEM_RAW_EYE_0         = 0x2047,
    MODEM_ANT_DIV_MODE      = 0x2048,
    MODEM_ANT_DIV_CONTROL   = 0x2049,
    MODEM_RSSI_THRESH       = 0x204A,
    MODEM_RSSI_JUMP_THRESH  = 0x204B,
    MODEM_RSSI_CONTROL      = 0x204C,
    MODEM_RSSI_CONTROL2     = 0x204D,
    MODEM_RSSI_COMP         = 0x204E,
    MODEM_CLKGEN_BAND       = 0x2051,

    MODEM_CHFLT_RX1_CHFLT_COE13_7_0 = 0x2100,
    MODEM_CHFLT_RX1_CHFLT_COE12_7_0 = 0x2101,
    MODEM_CHFLT_RX1_CHFLT_COE11_7_0 = 0x2102,
    MODEM_CHFLT_RX1_CHFLT_COE10_7_0	= 0x2103,
    MODEM_CHFLT_RX1_CHFLT_COE9_7_0  = 0x2104,
    MODEM_CHFLT_RX1_CHFLT_COE8_7_0  = 0x2105,
    MODEM_CHFLT_RX1_CHFLT_COE7_7_0  = 0x2106,
    MODEM_CHFLT_RX1_CHFLT_COE6_7_0  = 0x2107,
    MODEM_CHFLT_RX1_CHFLT_COE5_7_0  = 0x2108,
    MODEM_CHFLT_RX1_CHFLT_COE4_7_0  = 0x2109,
    MODEM_CHFLT_RX1_CHFLT_COE3_7_0	= 0x210A,
    MODEM_CHFLT_RX1_CHFLT_COE2_7_0  = 0x210B,
    MODEM_CHFLT_RX1_CHFLT_COE1_7_0  = 0x210C,
    MODEM_CHFLT_RX1_CHFLT_COE0_7_0	= 0x210D,
    MODEM_CHFLT_RX1_CHFLT_COEM0     = 0x210E,
    MODEM_CHFLT_RX1_CHFLT_COEM1     = 0x210F,
    MODEM_CHFLT_RX1_CHFLT_COEM2     = 0x2110,
    MODEM_CHFLT_RX1_CHFLT_COEM3     = 0x2111,
    MODEM_CHFLT_RX2_CHFLT_COE13_7_0	= 0x2112,
    MODEM_CHFLT_RX2_CHFLT_COE12_7_0	= 0x2113,
    MODEM_CHFLT_RX2_CHFLT_COE11_7_0	= 0x2114,
    MODEM_CHFLT_RX2_CHFLT_COE10_7_0	= 0x2115,
    MODEM_CHFLT_RX2_CHFLT_COE9_7_0  = 0x2116,
    MODEM_CHFLT_RX2_CHFLT_COE8_7_0  = 0x2117,
    MODEM_CHFLT_RX2_CHFLT_COE7_7_0  = 0x2118,
    MODEM_CHFLT_RX2_CHFLT_COE6_7_0  = 0x2119,
    MODEM_CHFLT_RX2_CHFLT_COE5_7_0  = 0x211A,
    MODEM_CHFLT_RX2_CHFLT_COE4_7_0  = 0x211B,
    MODEM_CHFLT_RX2_CHFLT_COE3_7_0  = 0x211C,
    MODEM_CHFLT_RX2_CHFLT_COE2_7_0  = 0x211D,
    MODEM_CHFLT_RX2_CHFLT_COE1_7_0	= 0x211E,
    MODEM_CHFLT_RX2_CHFLT_COE0_7_0  = 0x211F,
    MODEM_CHFLT_RX2_CHFLT_COEM0     = 0x2120,
    MODEM_CHFLT_RX2_CHFLT_COEM1     = 0x2121,
    MODEM_CHFLT_RX2_CHFLT_COEM2     = 0x2122,
    MODEM_CHFLT_RX2_CHFLT_COEM3     = 0x2123,

    PA_MODE                 = 0x2200,
    PA_PWR_LVL              = 0x2201,
    PA_BIAS_CLKDUTY         = 0x2202,
    PA_TC                   = 0x2203,
    PA_RAMP_EX              = 0x2204,
    PA_RAMP_DOWN_DELAY      = 0x2205,

    SYNTH_PFDCP_CPFF        = 0x2300,
    SYNTH_PFDCP_CPINT       = 0x2301,
    SYNTH_VCO_KV            = 0x2302,
    SYNTH_LPFILT3           = 0x2303,
    SYNTH_LPFILT2           = 0x2304,
    SYNTH_LPFILT1           = 0x2305,
    SYNTH_LPFILT0           = 0x2306,
    SYNTH_VCO_KVCAL         = 0x2307,

    MATCH_VALUE_1           = 0x3000,
    MATCH_MASK_1            = 0x3001,
    MATCH_CTRL_1            = 0x3002,
    MATCH_VALUE_2           = 0x3003,
    MATCH_MASK_2            = 0x3004,
    MATCH_CTRL_2            = 0x3005,
    MATCH_VALUE_3           = 0x3006,
    MATCH_MASK_3            = 0x3007,
    MATCH_CTRL_3            = 0x3008,
    MATCH_VALUE_4           = 0x3009,
    MATCH_MASK_4            = 0x300A,
    MATCH_CTRL_4            = 0x300B,

    FREQ_CONTROL_INTE       = 0x4000,
    FREQ_CONTROL_FRAC_2     = 0x4001,
    FREQ_CONTROL_FRAC_1     = 0x4002,
    FREQ_CONTROL_FRAC_0     = 0x4003,
    FREQ_CONTROL_CHANNEL_STEP_SIZE_1    = 0x4004,
    FREQ_CONTROL_CHANNEL_STEP_SIZE_0	= 0x4005,
    FREQ_CONTROL_W_SIZE     = 0x4006,
    FREQ_CONTROL_VCOCNT_RX_ADJ          = 0x4007,

    RX_HOP_CONTROL          = 0x5000,
    RX_HOP_TABLE_SIZE       = 0x5001,
    RX_HOP_TABLE_ENTRY_0    = 0x5002,
    RX_HOP_TABLE_ENTRY_1    = 0x5003,
    RX_HOP_TABLE_ENTRY_2    = 0x5004,
    RX_HOP_TABLE_ENTRY_3    = 0x5005,
    RX_HOP_TABLE_ENTRY_4    = 0x5006,
    RX_HOP_TABLE_ENTRY_5    = 0x5007,
    RX_HOP_TABLE_ENTRY_6    = 0x5008,
    RX_HOP_TABLE_ENTRY_7    = 0x5009,
    RX_HOP_TABLE_ENTRY_8    = 0x500A,
    RX_HOP_TABLE_ENTRY_9    = 0x500B,
    RX_HOP_TABLE_ENTRY_10   = 0x500C,
    RX_HOP_TABLE_ENTRY_11   = 0x500D,
    RX_HOP_TABLE_ENTRY_12   = 0x500E,
    RX_HOP_TABLE_ENTRY_13   = 0x500F,
    RX_HOP_TABLE_ENTRY_14   = 0x5010,
    RX_HOP_TABLE_ENTRY_15   = 0x5011,
    RX_HOP_TABLE_ENTRY_16   = 0x5012,
    RX_HOP_TABLE_ENTRY_17   = 0x5013,
    RX_HOP_TABLE_ENTRY_18   = 0x5014,
    RX_HOP_TABLE_ENTRY_19   = 0x5015,
    RX_HOP_TABLE_ENTRY_20   = 0x5016,
    RX_HOP_TABLE_ENTRY_21   = 0x5017,
    RX_HOP_TABLE_ENTRY_22   = 0x5018,
    RX_HOP_TABLE_ENTRY_23   = 0x5019,
    RX_HOP_TABLE_ENTRY_24   = 0x501A,
    RX_HOP_TABLE_ENTRY_25   = 0x501B,
    RX_HOP_TABLE_ENTRY_26   = 0x501C,
    RX_HOP_TABLE_ENTRY_27   = 0x501D,
    RX_HOP_TABLE_ENTRY_28   = 0x501E,
    RX_HOP_TABLE_ENTRY_29   = 0x501F,
    RX_HOP_TABLE_ENTRY_30   = 0x5020,
    RX_HOP_TABLE_ENTRY_31   = 0x5021,
    RX_HOP_TABLE_ENTRY_32   = 0x5022,
    RX_HOP_TABLE_ENTRY_33   = 0x5023,
    RX_HOP_TABLE_ENTRY_34   = 0x5024,
    RX_HOP_TABLE_ENTRY_35   = 0x5025,
    RX_HOP_TABLE_ENTRY_36   = 0x5026,
    RX_HOP_TABLE_ENTRY_37   = 0x5027,
    RX_HOP_TABLE_ENTRY_38   = 0x5028,
    RX_HOP_TABLE_ENTRY_39   = 0x5029,
    RX_HOP_TABLE_ENTRY_40   = 0x502A,
    RX_HOP_TABLE_ENTRY_41   = 0x502B,
    RX_HOP_TABLE_ENTRY_42   = 0x502C,
    RX_HOP_TABLE_ENTRY_43   = 0x502D,
    RX_HOP_TABLE_ENTRY_44   = 0x502E,
    RX_HOP_TABLE_ENTRY_45   = 0x502F,
    RX_HOP_TABLE_ENTRY_46   = 0x5030,
    RX_HOP_TABLE_ENTRY_47   = 0x5031,
    RX_HOP_TABLE_ENTRY_48   = 0x5032,
    RX_HOP_TABLE_ENTRY_49   = 0x5033,
    RX_HOP_TABLE_ENTRY_50   = 0x5034,
    RX_HOP_TABLE_ENTRY_51   = 0x5035,
    RX_HOP_TABLE_ENTRY_52   = 0x5036,
    RX_HOP_TABLE_ENTRY_53   = 0x5037,
    RX_HOP_TABLE_ENTRY_54   = 0x5038,
    RX_HOP_TABLE_ENTRY_55   = 0x5039,
    RX_HOP_TABLE_ENTRY_56   = 0x503A,
    RX_HOP_TABLE_ENTRY_57   = 0x503B,
    RX_HOP_TABLE_ENTRY_58   = 0x503C,
    RX_HOP_TABLE_ENTRY_59   = 0x503D,
    RX_HOP_TABLE_ENTRY_60   = 0x503E,
    RX_HOP_TABLE_ENTRY_61   = 0x503F,
    RX_HOP_TABLE_ENTRY_62   = 0x5040,
    RX_HOP_TABLE_ENTRY_63   = 0x5041

}SI446X_PROPERTY;

//手动切换芯片到所需的工作状态 210204
#define CHANGE_STATE_CMD 0x34
#define NOCHANGE_STATE 	0
#define SLEEP_STATE 		1
#define SPIACTIV_STATEE 2
#define READY_STATE 		3
#define TXTUNE_STATE 		5
#define RXTUNE_STATE 		6
#define TX_STATE 				7
#define RX_STATE 				8

/****************************函数申明区************************************/
void SI446x_Wait_Cts( void );
void SI446x_Write_Cmds( uint8_t *pCmd, uint8_t CmdNumber );
void SI446x_Power_Up( uint32_t Xo_Freq );
void SI446x_Read_Response( uint8_t *pRead, uint8_t Length );
uint8_t SI446x_Nop( void );
void SI446x_Get_Part_Informatoin( uint8_t *pRead );
void SI446x_Get_Fun_Informatoin( uint8_t *pRead );
void SI446x_Interrupt_Status( uint8_t *pRead );
void SI446x_Get_Property( SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pRead  );
void SI446x_Set_Property( SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pWrite );
void SI446x_Set_Property_1( SI446X_PROPERTY Group_Num, uint8_t Start_Prop );
uint8_t SI446x_Get_Property_1( SI446X_PROPERTY Group_Num );
void SI446x_Reset( void );
void SI446x_Config_Gpio( uint8_t Gpio_0, uint8_t Gpio_1, uint8_t Gpio_2, uint8_t Gpio_3, uint8_t Irq, uint8_t Sdo, uint8_t Gen_Config );
void SI446x_Write_TxFifo( uint8_t *pWriteData, uint8_t Length );
void SI446x_Reset_RxFifo( void );
void SI446x_Reset_TxFifo( void );
void SI446x_Send_Packet(uint8_t Channel, uint8_t Condition,uint8_t *pTxData, uint8_t Length);
void SI446x_Start_Tx( uint8_t Channel, uint8_t Condition, uint16_t Length );
uint8_t SI446x_Read_Packet( uint8_t *pRxData );
void SI446x_Start_Rx( uint8_t Channel, uint8_t Condition, uint16_t Length, uint8_t Next_State1, uint8_t Next_State2, uint8_t Next_State3 );
void SI446x_Get_Packet_Information( uint8_t *pReadData, uint8_t FieldNumMask, uint16_t Length, uint16_t DiffLen );
void SI446x_Get_Fifo_Information( uint8_t *pReadData );
void SI446x_Change_Status( uint8_t NextStatus );
uint8_t SI446x_Get_Device_Status( void );
void SI446x_Set_Power( uint8_t PowerLevel );
void SI446x_Gpio_Init( void );
void SI446x_Config_Init( void );
void SI446x_Init( void );
void SI44xx_Power_Init(void);
/****************************功能API************************************/
typedef struct
{
	void (*SI4438_Gpio_Init)(void);
	void (*SI4438_Reset)(void);
	void (*SI4438_Power_Up)(uint32_t Xo_Freq);
	void (*SI4438_Config_Init)(void);
	void (*SI4438_Set_DataRate)(uint32_t DataRate);
	void (*SI4438_Set_Power)(uint8_t PowerLevel);
	/////////////////////////////////////////////////////////
	void (*SI4438_Init)(void);//SI4438初始化
	uint8_t (*SI4438_Get_Device_Status)(void);
	void (*SI4438_Status_Set)(uint8_t NextStatus);//SI4438状态切换
	void (*SI4438_Start_Rx)( uint8_t Channel, uint8_t Condition, uint16_t Length, uint8_t Next_State1, uint8_t Next_State2, uint8_t Next_State3);
	void (*SI4438_Start_Tx)(uint8_t Channel, uint8_t Condition, uint16_t Length);
	void (*SI4438_Send_Packet)(uint8_t Channel, uint8_t Condition,uint8_t *pTxData, uint8_t Lengt);//SI4438发送数据包
	uint8_t (*SI4438_Read_Packet)(uint8_t *pRxData);//SI4438读取数据包

}SI4438_API;

#endif
