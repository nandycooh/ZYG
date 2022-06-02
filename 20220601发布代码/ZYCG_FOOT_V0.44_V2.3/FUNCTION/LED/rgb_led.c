#include "rgb_led.h"

/************************************************************  	
函数名称：	Led_Init
函数功能：	RGB 初始化
入口参数：	void
返回参数：	无
说明：	    无
***********************************************************/ 
void Led_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
}

void Led_ShutDown(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//选择GPIO响应速度    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //设置为输出 
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);//初始化 
	
	BLUE_LED  = 1;
	GREEN_LED = 1;
	RED_LED   = 1;
}

/******************************************************************************************************  	
函数名称：	led_ctrl
函数功能：	RGB 颜色管理
入口参数：	colour：
            			RED     1
            			GREEN   2
            			BLUE    3
            			YELLOW  4
            			PURPLE  5
            			NAVY    6
            			WHITE   7
            state：
            			LED_OFF   1
            			LED_ON    0
返回参数：	无
说明：	    无
*****************************************************************************************************/
void led_ctrl(u8 colour, bool state)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN);
	switch(colour){
		case RED:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN);
			}
			else {
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN);
			}
			break;
		case GREEN:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_GREEN_GPIO_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_GREEN_GPIO_PIN);
			}
			break;
		case BLUE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN);
			}
			else {
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN);
			}
			break;
		case YELLOW:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN);
			}
			break;
		case PURPLE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_RED_GPIO_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_RED_GPIO_PIN);
			}
			break;
		case NAVY:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_GREEN_GPIO_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_GREEN_GPIO_PIN);
			}
			break;
		case WHITE:
			if(state){
				GPIO_ResetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN);
			}
			else{
				GPIO_SetBits(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN);
			}
			break;
		default:
			break;
	}
}

void led_green_turn(void)
{
	led_ctrl(GREEN, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GREEN_GPIO_PIN));
}

void led_red_turn(void)
{
	led_ctrl(RED, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_RED_GPIO_PIN));
}

void led_blue_turn(void)
{
	led_ctrl(BLUE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_GPIO_PIN));
}

void led_white_turn(void)
{
	led_ctrl(WHITE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN));
}

void led_yellow_turn(void)
{
	led_ctrl(YELLOW, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN));
}

void led_purple_turn(void)
{
	led_ctrl(PURPLE, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_RED_GPIO_PIN));
}

void led_navy_turn(void)
{
	led_ctrl(NAVY, (bool)GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_BLUE_GPIO_PIN | LED_GREEN_GPIO_PIN));
}

/************************************************************  	
函数名称：	led_turn_state
函数功能：	RGB 闪烁状态
入口参数：	colour：指定颜色
返回参数：	无
说明：	    无
***********************************************************/ 
void led_turn_state(u8 colour)
{
	switch(colour)
	{
		case RED	:	led_red_turn();break;
		case GREEN:	led_green_turn();break;
		case BLUE	:	led_blue_turn();break;
		case WHITE:	led_white_turn();break;
		case YELLOW:	led_yellow_turn();break;
		case PURPLE:	led_purple_turn();break;
		case NAVY:	led_navy_turn();break;
		default:
			break;
	}
}
/************************************************************  	
函数名称：	led_xxxx_state
函数功能：	指定RGB 亮或灭
入口参数：	status：状态
返回参数：	无
说明：	    无
***********************************************************/ 
void led_green_state(bool status)
{
	if(status == LED_OFF)
		led_ctrl(GREEN, (bool)LED_OFF);
	else
		led_ctrl(GREEN, (bool)LED_ON);
}

void led_red_state(bool status)
{
	if(status == LED_OFF)
		led_ctrl(RED, (bool)LED_OFF);
	else
		led_ctrl(RED, (bool)LED_ON);
}

void led_blue_state(bool status)
{
	if(status == LED_OFF)
		led_ctrl(BLUE, (bool)LED_OFF);
	else
		led_ctrl(BLUE, (bool)LED_ON);
}

void led_white_state(bool status)
{
	if(status == LED_OFF)
		led_ctrl(WHITE, (bool)LED_OFF);
	else
		led_ctrl(WHITE, (bool)LED_ON);
}

void led_yellow_state(bool status)
{
	if(status == LED_OFF)
		led_ctrl(YELLOW, (bool)LED_OFF);
	else
		led_ctrl(YELLOW, (bool)LED_ON);
}

/************************************************************  	
函数名称：	Led_Management
函数功能：	系统灯光指示管理
入口参数：	无
返回参数：	无
说明：	    无
***********************************************************/ 
void Led_Management(void)
{

}

LED_DRIVER_APP_API Led_Driver_App_Port = 
{
	.Driver_LedInit = Led_Init,
	.Driver_LedShutDown = Led_ShutDown,
	//LED闪烁状态
	.App_LedTurnState=led_turn_state,
	//LED亮灭状态
	.App_LedWhiteState=led_white_state,
	.App_LedBlueState=led_blue_state,
	.App_LedGreenState=led_green_state,
	.App_LedredState=led_red_state,
	//灯光管理
	.App_LedManagement=Led_Management,
};
