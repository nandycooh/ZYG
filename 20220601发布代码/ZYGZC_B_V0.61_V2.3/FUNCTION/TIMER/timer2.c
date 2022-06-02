#include "timer2.h"
#include "main.h"

void Timer2_Init(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseStructure.TIM_Period = arr; 
    TIM_TimeBaseStructure.TIM_Prescaler = psc; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); 

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 

    TIM_Cmd(TIM2, ENABLE); 		
    
}
u8 timer2Cnt;
static u16 testCnt2;
static u8 testCnt2Flag;
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
        
        timer2Cnt++;
        
#ifdef SHOULDER//¼ç
        if(Shoulder_L.shakeTime != 0)
            Shoulder_L.shakeTime--;
        else
            SHOULDER_L_TIM = 0;
        
        if(Shoulder_R.shakeTime != 0)
        {
            Shoulder_R.shakeTime--;
        }
        else
        {
            SHOULDER_R_TIM = 0;
        }
        
#endif
#ifdef CHEST//ÐØ
        if(Chest_L.shakeTime != 0)
        {
            Chest_L.shakeTime--;
        }
        else
        {
            CHEST_L_TIM = 0;
        }
        
        if(Chest_R.shakeTime != 0)
        {
            testCnt2Flag = 1;
            Chest_R.shakeTime--;
        }
        else
        {
            if(testCnt2Flag == 1)
            {
                testCnt2++;
                testCnt2Flag = 0;
            }

            CHEST_R_TIM = 0;
        }
#endif

#ifdef BACK//±³
        if(Back_L.shakeTime != 0)
            Back_L.shakeTime--;
        else
            BACK_L_TIM = 0;
            
//        if(Back_R.shakeTime != 0)
//            Back_R.shakeTime--;
//        else
//            BACK_R_TIM = 0;
#endif
    }
}
