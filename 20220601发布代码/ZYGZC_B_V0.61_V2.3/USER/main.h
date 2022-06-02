#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"

#define v0_44
struct shakeType
{
    uint16_t shakeStrength;//Õñ¶¯Á¦Á¿
    uint16_t shakeFreq;//Õñ¶¯ÆµÂÊ
    uint16_t shakeTime;//
    uint8_t  shakeMode;//
};
extern struct shakeType
Head, Occiput, Chest_L, Chest_R, Rib_L, Rib_R, Belly_L, Belly_R,
 Back_L, Back_R, Waist_L, Waist_R, Arm_L, Wrist_L, Hand_L, 
 Arm_R, Wrist_R, Hand_R, Thigh_L, Shank_L, Foot_L, 
 Thigh_R, Shank_R, Foot_R, Shoulder_L, Shoulder_R;

#define SHOULDER        //¼ç²¿
#define CHEST           //ÐØ²¿/¼ç²¿
//#define RIB             //Àß²¿
//#define BELLY           //¸¹²¿/ÐØ
#define BACK            //±³²¿
//#define WAIST           //Ñü²¿    
//#define ARM             //ÉÏÖ«
//#define THIGH           //ÏÂÖ«

#ifdef v0_34
#define CHEST_L_TIM     TIM2->CCR1
#define CHEST_R_TIM     TIM2->CCR2
#define RIB_L_TIM       TIM4->CCR1
#define RIB_R_TIM       TIM4->CCR2
#define BELLY_L_TIM     TIM2->CCR4
#define BELLY_R_TIM     TIM2->CCR3
#define BACK_L_TIM      TIM1->CCR4
#define BACK_R_TIM      TIM1->CCR2
#define WAIST_L_TIM     TIM1->CCR3
#define WAIST_R_TIM     TIM1->CCR1
#endif

#ifdef v0_40
#define CHEST_L_TIM     TIM1->CCR2     //¼ç²¿
#define CHEST_R_TIM     TIM2->CCR2
#define BELLY_L_TIM     TIM1->CCR1     //¸¹²¿
#define BELLY_R_TIM     TIM2->CCR1
#define BACK_L_TIM      TIM1->CCR3     //±³²¿
#define BACK_R_TIM      TIM2->CCR3     
#define WAIST_L_TIM     TIM1->CCR4     //Ñü²¿
#define WAIST_R_TIM     TIM2->CCR4
#endif


#ifdef v0_40_1

#define CHEST_L_TIM    TIM1->CCR2       //¼ç²¿
#define CHEST_R_TIM    TIM1->CCR4       //¼ç²¿
#define BELLY_L_TIM    TIM1->CCR1       //ÐØ
#define BELLY_R_TIM    TIM2->CCR1       //ÐØ
#define BACK_L_TIM     TIM1->CCR3       //±³
#define BACK_R_TIM     TIM2->CCR2

#endif

#ifdef v0_42

/*
G¶Ë    ×ó        ÓÒ       B¶Ë  ×ó         ÓÒ   
       ¸¹1       ¸¹5           ÐØ9        ÐØ10
       ¼ç2       ¼ç6           ¼ç2        ¼ç6
       ±³3       ±³7                 ±³3
       Ñü4       Ñü8
       1-4£ºTimer1    5-8£ºTimer2     9-10£ºTimer4
*/


#define SHOULDER_L_TIM    TIM1->CCR2     //¼ç²¿  
#define SHOULDER_R_TIM    TIM2->CCR2
#define CHEST_L_TIM       TIM4->CCR1     //ÐØ²¿
#define CHEST_R_TIM       TIM4->CCR2
#define BELLY_L_TIM       TIM1->CCR1     //¸¹²¿
#define BELLY_R_TIM       TIM2->CCR1
#define BACK_L_TIM        TIM1->CCR3     //±³²¿
#define BACK_R_TIM        TIM2->CCR3     
#define WAIST_L_TIM       TIM1->CCR4     //Ñü²¿
#define WAIST_R_TIM       TIM2->CCR4

#endif

#ifdef v0_43

#define SHOULDER_L_TIM    TIM4->CCR1     //¼ç²¿  
#define SHOULDER_R_TIM    TIM1->CCR3
#define CHEST_L_TIM       TIM3->CCR1     //ÐØ²¿
#define CHEST_R_TIM       TIM4->CCR2
#define BACK_L_TIM        TIM1->CCR2     //±³²¿

#endif
#ifdef v0_44

#define SHOULDER_L_TIM    TIM1->CCR3   
#define SHOULDER_R_TIM    TIM3->CCR1  //¼ç²¿
#define CHEST_L_TIM       TIM4->CCR2
#define CHEST_R_TIM       TIM4->CCR1     //ÐØ²¿
#define BACK_L_TIM        TIM1->CCR2     //±³²¿
//#define BACK_R_TIM        TIM2->CCR3     

#endif

#endif
