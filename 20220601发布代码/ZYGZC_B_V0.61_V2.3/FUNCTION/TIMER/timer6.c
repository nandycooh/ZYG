#include "timer6.h"
//#include "main.h"
//#include "nrf24l01p.h"

void Timer6_Init(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    TIM_TimeBaseStructure.TIM_Period = arr; 
    TIM_TimeBaseStructure.TIM_Prescaler = psc; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); 

    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE ); 

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 

    TIM_Cmd(TIM6, ENABLE); 		
}
u8 timer6Cnt;
u16 testCnt2;
u8 testCnt2Flag;
void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_FLAG_Update);
        
        timer6Cnt++;
        
#ifdef SHOULDER
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
#ifdef CHEST
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
#ifdef RIB
        if(Rib_L.shakeTime != 0)            
            Rib_L.shakeTime--;
        else
            RIB_L_TIM = 0;
        
        if(Rib_R.shakeTime != 0)    
            Rib_R.shakeTime--;
        else
            RIB_R_TIM = 0;
#endif
#ifdef BELLY
        if(Belly_L.shakeTime != 0)    
            Belly_L.shakeTime--;
        else
            BELLY_L_TIM = 0;
        
        if(Belly_R.shakeTime != 0)    
            Belly_R.shakeTime--;
        else
            BELLY_R_TIM = 0;
#endif
#ifdef BACK
        if(Back_L.shakeTime != 0)
            Back_L.shakeTime--;
        else
            BACK_L_TIM = 0;
            
        if(Back_R.shakeTime != 0)
            Back_R.shakeTime--;
        else
            BACK_R_TIM = 0;
#endif
#ifdef WAIST
        if(Waist_L.shakeTime != 0)    
            Waist_L.shakeTime--;
        else
            WAIST_L_TIM = 0;
            
        if(Waist_R.shakeTime != 0)
            Waist_R.shakeTime--;
        else
            WAIST_R_TIM = 0;
#endif
#ifdef ARM
        if(Arm_L.shakeTime != 0)    
            Arm_L.shakeTime--;
        else
            TIM3->CCR1 = 0;
            
        if(Wrist_L.shakeTime != 0)
            Wrist_L.shakeTime--;
        else
            TIM3->CCR2 = 0;
            
        if(Hand_L.shakeTime != 0)
            Hand_L.shakeTime--;
        else
            ;
            
        if(Arm_R.shakeTime != 0)
            Arm_R.shakeTime--;
        else
            TIM3->CCR3 = 0;
            
        if(Wrist_R.shakeTime != 0)
            Wrist_R.shakeTime--;
        else
            TIM3->CCR4 = 0;
            
        if(Hand_R.shakeTime != 0)
            Hand_R.shakeTime--;
        else
            ;
#endif
#ifdef THIGH
        if(Thigh_L.shakeTime != 0)
            Thigh_L.shakeTime--;
        else
            TIM5->CCR1 = 0;
            
        if(Shank_L.shakeTime != 0)
            Shank_L.shakeTime--;
        else
            TIM5->CCR2 = 0;
            
        if(Foot_L.shakeTime != 0)
            Foot_L.shakeTime--;
        else
            ;
            
        if(Thigh_R.shakeTime != 0)
            Thigh_R.shakeTime--;
        else
            TIM5->CCR3 = 0;
            
        if(Shank_R.shakeTime != 0)
            Shank_R.shakeTime--;
        else
            TIM5->CCR4 = 0;
            
        if(Foot_R.shakeTime != 0)
            Foot_R.shakeTime--;
        else
            ;
#endif
    }
}
