#include "led.h"
#include "delay.h"
/*********************************硬件驱动层****************************/
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN | LED_RED_PIN | LED_GREEN_PIN;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BLUE_LED_OFF;
	GREEN_LED_OFF;
	RED_LED_OFF;
}


void Led_ShutDown(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN | LED_RED_PIN | LED_GREEN_PIN; 
	//选择GPIO响应速度
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	//设置为输出           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	//初始化 
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure); 
	
	BLUE_LED=1;
	GREEN_LED=1;
	RED_LED=1;
}

/*********************************功能实现层****************************/
/**
  * @brief 颜色管理器
  * @param  None
  * @retval : None
**/
void led_ctrl(u8 colour, bool state)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
	switch(colour){
		case RED:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_PIN);
			}
			else {
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_PIN);
			}
			break;
		case GREEN:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_GREEN_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_GREEN_PIN);
			}
			break;
		case BLUE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_PIN);
			}
			else {
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_PIN);
			}
			break;
		case YELLOW:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN);
			}
			break;
		case PURPLE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_PIN | LED_RED_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_PIN | LED_RED_PIN);
			}
			break;
		case NAVY:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_PIN | LED_GREEN_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_PIN | LED_GREEN_PIN);
			}
			break;
		case WHITE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
			}
			break;
		default:
			break;
	}
}

void led_purple_turn(void)
{
	led_ctrl(PURPLE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_PIN | LED_RED_PIN));
}

void led_yellow_turn(void)
{
	led_ctrl(YELLOW, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_RED_PIN | LED_GREEN_PIN));
}

void led_green_turn(void)
{
	led_ctrl(GREEN, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GREEN_PIN));
}

void led_red_turn(void)
{
	led_ctrl(RED, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_RED_PIN));
}

void led_blue_turn(void)
{
	led_ctrl(BLUE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_PIN));
}

void led_white_turn(void)
{
	led_ctrl(WHITE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_PIN | LED_RED_PIN | LED_GREEN_PIN));
}

void led_ctl(u8 colour)
{
	RED_LED = 1;GREEN_LED = 1;BLUE_LED = 1;
	
	if(colour == BLUE){
		BLUE_LED = 0;
	}
	if(colour == RED){
		RED_LED = 0;
	}
	if(colour == GREEN){
		GREEN_LED = 0;
	}
}

void led_green_off(void)
{
	led_ctrl(GREEN, (bool)LED_OFF);
}

void led_red_off(void)
{
	led_ctrl(RED, (bool)LED_OFF);
}

void led_blue_off(void)
{
	led_ctrl(BLUE, (bool)LED_OFF);
}
void led_blue_on(void)
{
	led_ctrl(BLUE, (bool)LED_ON);
}
void led_white_off(void)
{
    led_ctrl(WHITE, (bool)LED_OFF); 
}

void led_white_on(void)
{
    led_ctrl(WHITE, (bool)LED_ON); 
}

void led_purple_on(void)
{
	led_ctrl(PURPLE, (bool)LED_ON);
}

HalAdapter_Management_Led_t halAdapter_Led = 
{
	.isInit = false,
	.HAL_LedInit = Led_Init,
	.HAL_LedShutDown = Led_ShutDown,
	
	.HAL_LedGreenTurn=led_green_turn,
	.HAL_LedRedTurn=led_red_turn,
	.HAL_LedBlueTurn=led_blue_turn,
	.HAL_LedWhiteTurn=led_white_turn,
	.HAL_LedPurpleTurn=led_purple_turn,
	.HAL_LedYellowTurn=led_yellow_turn,

	.HAL_LedWhiteOn=led_white_on,
	.HAL_LedBlueOn=led_blue_on,
	.HAL_LedGreenOff=led_green_off,
	.HAL_LedredOff=led_red_off,
	.HAL_LedblueOff=led_blue_off,
	.HAL_LedwhiteOff=led_white_off,
	.HAL_LedpurpleOn=led_purple_on
};

