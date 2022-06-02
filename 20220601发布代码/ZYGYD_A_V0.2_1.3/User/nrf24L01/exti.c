
#include "exti.h"

void EXTI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	 
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);

	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 

	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;					
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;							
	NVIC_Init(&NVIC_InitStructure);  
}


uint8_t IrqDynRxFlag;
uint8_t IrqDynTxFlag;


void EXTI4_IRQHandler(void)
{	
	uint8_t sta;
	if(!RF_getIrq())
    {
        sta = RF_irqRxTxAckF(rfRxChG);
        if(sta == RF_TX_OK)
        {
            rfIrqTxFlagG=1;
        }
    }
	EXTI_ClearITPendingBit(EXTI_Line4); 
}
