#ifndef __POWERMANAGE_H
#define __POWERMANAGE_H

#include "stm32f10x.h"
#include "stdio.h"
#include "sys.h"
#include "delay.h"



#define BAT_CHARGE_PIN 	GPIO_Pin_12    //在插上USB后判断，充电中：低   充满：高
#define USB_CHECK_PIN 	GPIO_Pin_10
#define BAT_CHRG_IN     PBin(12)
#define USB_CHECK_IN    PBin(10)

#define READBAT  GPIO_ReadInputDataBit(GPIOB, BAT_CHARGE_PIN)//充电时的状态
#define READUSB  GPIO_ReadInputDataBit(GPIOB, USB_CHECK_PIN)//是否插上USB

#define BAT_CAPACITY_LV1   1.35     //对应显示电量80%
#define BAT_CAPACITY_LV2   1.30     //对应显示电量60%
#define BAT_CAPACITY_LV3   1.22    //对应显示电量40%
#define BAT_CAPACITY_LV4   1.15     //对应显示电量20%
#define BAT_CAPACITY_LV5   1.10     //对应显示电量空
#define BAT_CAPACITY_LV6   1.06     //对应强制关机电量

void charge_IC_RQ_shutdown(void);
void charge_led_dsp(void);
void Enter_Sleep(void);

#endif
