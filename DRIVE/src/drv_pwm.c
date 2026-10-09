#include "drv_pwm.h"
#include "parameter.h"
#define CHANNLLOW       4  //TIME1 TIME2-CCMR1,CCMR2
#define CHANNLHIGH      5  //TIME1 TIME2-CCMR1,CCMR2
#define PWM1ENBLE       6  //TIME1 TIME2-CCMR1,CCMR2
#define OC1M_MASK      0x0070U
#define OC1M_POS        (4)
#define OC2M_MASK      0x7000U
#define OC2M_POS        (12)
#define OC3M_MASK      0x0070U
#define OC3M_POS        (4)
void Drv_pwm_init(TIM_TypeDef * pTim, uint16_t u16Period,uint16_t u16DeadTime)
{

  /** Define the struct of the PWM configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /** Enable the TIM1 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM1_Msk, ENABLE);

    /**
     * Sets the value of the automatic reload register Period for the next update event load activity  
     * Set the Prescaler value used as the divisor of the TIMx clock frequency
     * Set clock split :TDTS = TIM_CKD_DIV1
     * TIM center aligned mode1  
     */
    TIM_TimeBaseStructure.TIM_Period        = u16Period - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 1;
  
    TIM_TimeBaseInit(pTim, &TIM_TimeBaseStructure);

    /**
     * Enable state selection in running mode  
     * Enable state selection in idle mode  
     * Software error lock configuration: lock closed without protection
     * DTG[7:0] dead zone generator configuration (dead zone time DT)  
     */
     /**
     * TDTS = 125nS(8MHz)
     * DTG[7: 5] = 0xx => DT = DTG[7: 0] * Tdtg, Tdtg = TDTS;
     * DTG[7: 5] = 10x => DT =(64+DTG[5: 0]) * Tdtg, Tdtg = 2 * TDTS;
     * DTG[7: 5] = 110 => DT =(32+DTG[4: 0]) * Tdtg, Tdtg = 8 * TDTS;
     * DTG[7: 5] = 111=> DT =(32 + DTG[4: 0]) *  Tdtg, Tdtg = 16 * TDTS;
     */
    TIM_BDTRInitStruct.TIM_OSSRState    = TIM_OSSRState_Enable;
    TIM_BDTRInitStruct.TIM_OSSIState    = TIM_OSSIState_Enable;
    TIM_BDTRInitStruct.TIM_LOCKLevel    = TIM_LockLevel_OFF;
    TIM_BDTRInitStruct.TIM_DeadTime     = u16DeadTime;

    /**
     * Brake configuration: disable brake
     * Brake input polarity: active in low level   
     * Auto output enable configuration: Disable MOE bit hardware control
     */
		 #if PWM_IO_BKIN_EN
    TIM_BDTRInitStruct.TIM_Break            = TIM_Break_Enable;
		#else 
		 TIM_BDTRInitStruct.TIM_Break            = TIM_Break_Disable;
		#endif
    TIM_BDTRInitStruct.TIM_BreakPolarity    = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct.TIM_AutomaticOutput  = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(pTim, &TIM_BDTRInitStruct);

//		TIM_BreakInputFilterConfig(pTim, TIM_COMPBKIN_COMP1, TIM_BKINF_2);
//		TIM_BreakInputFilterCmd(pTim, ENABLE);
		
    /**
     * Mode configuration: PWM mode 1  
     * Output status setting: enable output  
     * Complementary channel output status setting: enable output
     * Sets the pulse value to be loaded into the capture comparison register
     * Output polarity is high
     * N Output polarity is high
     */
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 10;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity     = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState    = TIM_OCNIdleState_Reset;
    
    TIM_OC1Init(pTim, &TIM_OCInitStructure);
    TIM_OC2Init(pTim, &TIM_OCInitStructure);
    TIM_OC3Init(pTim, &TIM_OCInitStructure);
    pTim->CCER=0X0000;
    /** Initialize the CCR4 trigger point */
    TIM_OCInitStructure.TIM_Pulse           = 5;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;
    TIM_OC4Init(pTim, &TIM_OCInitStructure);

    /** Enable CH1, 2, and 3 to be preloaded */
    TIM_OC1PreloadConfig(pTim, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(pTim, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(pTim, TIM_OCPreload_Enable);
    
    /** Enable TIMx's preloaded register on ARR */
    TIM_ARRPreloadConfig(pTim, ENABLE);

    /** Enable the TIM1 */
    TIM_Cmd(pTim, ENABLE);
    /** Main Output Enable:Disable the MOE bit */
    TIM_CtrlPWMOutputs(pTim, ENABLE);



}

//void UH_VL_HPwmLOn(void )
//{	
//	TIM1->CCER = 0x5041;								// UH 输出使能, 高有效
//	TIM1->CCMR1&=((~OC1M_MASK)&(~OC2M_MASK));
//	TIM1->CCMR1|=((PWM1ENBLE<<OC1M_POS)|(CHANNLHIGH<<OC2M_POS));
//	TIM1->CCMR2&=(~OC3M_MASK);	
//	TIM1->CCMR2|=(CHANNLLOW<<OC3M_POS);
//}


//U+V-=A+B-
void UH_VL_HPwmLOn(void )
{	
	TIM1->CCER = 0x5041;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = PWM1ENBLE;		// U+Pwm
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;		// V-H;	
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;		
}

//U+W-=A+C-
void UH_WL_HPwmLOn(void)
{	
	TIM1->CCER = 0x5401;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = PWM1ENBLE;		// U+Pwm
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;		
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		// W-H	
}

//V+W-=B+C-
void VH_WL_HPwmLOn(void)
{
	TIM1->CCER = 0x5410;									// VH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;			
	TIM1->CCMR1BIT.OC2M = PWM1ENBLE;			// V+pwm
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;			// W-H
}

//V+U-=B+A-
void VH_UL_HPwmLOn(void)
{
	TIM1->CCER = 0x5014;									// VH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
	TIM1->CCMR1BIT.OC2M = PWM1ENBLE;			// V+pwm
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;				
}

//W+U-=C+A-
void WH_UL_HPwmLOn(void)
{	
	TIM1->CCER = 0x5104;									// WH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;	
	TIM1->CCMR2BIT.OC3M = PWM1ENBLE;			// W+PWM
}

//W+V-=C+B-
void WH_VL_HPwmLOn(void)
{	
	TIM1->CCER = 0x5140;									// WH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;				
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;			// V-H		
	TIM1->CCMR2BIT.OC3M = PWM1ENBLE;		
}

//--------------ipd-----------



void UH_VL_HOnLOn(void )
{	
	TIM1->CCER = 0x5041;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;		// U+Pwm
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;		// V-H;	
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;		
}

//U+W-=A+C-
void UH_WL_HOnLOn(void)
{	
	TIM1->CCER = 0x5401;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;		// U+H
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;		
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		// W-H	
}

//V+W-=B+C-
void VH_WL_HOnLOn(void)
{
	TIM1->CCER = 0x5410;									// VH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;			
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;			// V+H
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;			// W-H
}

//V+U-=B+A-
void VH_UL_HOnLOn(void)
{
	TIM1->CCER = 0x5014;									// VH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;			// V+H
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;				
}

//W+U-=C+A-
void WH_UL_HOnLOn(void)
{	
	TIM1->CCER = 0x5104;									// WH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;	
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;			// W+H
}

//W+V-=C+B-
void WH_VL_HOnLOn(void)
{	
	TIM1->CCER = 0x5140;									// WH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;				
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;			// V-H		
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		
}

//------------------------------------


//--------------mos check-----------



void UL_HOffLOn(void )
{	
	TIM1->CCER = 0x5004;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;		// 
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;		// 
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;		
}


void VL_HOffLOn(void)
{	
	TIM1->CCER = 0x5040;								// 
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;		// 
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;		
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;		// 
}


void WL_HOffLOn(void)
{
	TIM1->CCER = 0x5400;									// 
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;			
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;			// 
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;			// 
}


void UH_HOnLOff(void)
{
	TIM1->CCER = 0x5001;									// 
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// 	
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;			// 
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;				
}
///--------------------------------------------------------------------

void VH_HOnLOff(void)
{	
	TIM1->CCER = 0x5010;									// 
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;			// 
	TIM1->CCMR1BIT.OC2M = CHANNLHIGH;	
	TIM1->CCMR2BIT.OC3M = CHANNLLOW;			// 
}


void WH_HOnLOff(void)
{	
	TIM1->CCER = 0x5100;									// 
	TIM1->CCMR1BIT.OC1M = CHANNLLOW;				
	TIM1->CCMR1BIT.OC2M = CHANNLLOW;			// 	
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		
}

//------------------------------------




// 三相导通相序
void UH_VL_WL_HPwmLOn(void )
{
		TIM1->CCER = 0x5441;								// UH 输出使能, 高有效
		TIM1->CCMR1BIT.OC1M = PWM1ENBLE;		// U+Pwm
		TIM1->CCMR1BIT.OC2M = CHANNLHIGH;		// V-H;	
		TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		
}

//U+W-=A+C-
void UH_VH_WL_HPwmLOn(void)
{
		TIM1->CCER = 0x5411;								// UH 输出使能, 高有效
		TIM1->CCMR1BIT.OC1M = PWM1ENBLE;		// U+Pwm
		TIM1->CCMR1BIT.OC2M = PWM1ENBLE;		
		TIM1->CCMR2BIT.OC3M = CHANNLHIGH;		// W-H	
}

//V+W-=B+C-
void UL_VH_WL_HPwmLOn(void)
{
	TIM1->CCER = 0x5414;									// VH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			
	TIM1->CCMR1BIT.OC2M = PWM1ENBLE;			// V+pwm
	TIM1->CCMR2BIT.OC3M = CHANNLHIGH;			// W-H
}

//V+U-=B+A-
void UL_VH_WH_HPwmLOn(void)
{
		TIM1->CCER = 0x5114;									// VH 输出使能, 高有效
		TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
		TIM1->CCMR1BIT.OC2M = PWM1ENBLE;			// V+pwm
		TIM1->CCMR2BIT.OC3M = PWM1ENBLE;				
}

//W+U-=C+A-
void UL_VL_WH_HPwmLOn(void)
{
		TIM1->CCER = 0x5144;									// WH 输出使能, 高有效
		TIM1->CCMR1BIT.OC1M = CHANNLHIGH;			// U-H	
		TIM1->CCMR1BIT.OC2M = CHANNLHIGH;	
		TIM1->CCMR2BIT.OC3M = PWM1ENBLE;			// W+PWM
}

//W+V-=C+B-
void UH_VL_WH_HPwmLOn(void)
{
		TIM1->CCER = 0x5141;									// WH 输出使能, 高有效
		TIM1->CCMR1BIT.OC1M = PWM1ENBLE;				
		TIM1->CCMR1BIT.OC2M = CHANNLHIGH;			// V-H		
		TIM1->CCMR2BIT.OC3M = PWM1ENBLE;		
}

/*------------------------------------------*/
void Start_Motor(void)
{
    TIM1->CCR1 = 0x00;
    TIM1->CCR2 = 0x00;
    TIM1->CCR3 = 0x00;
    //OC1/OC1N/OC2/OC2N/OC3/OC3N
    TIM1->CCER |= 0x5555;
}

void UVWL_HOffLPwm(void )
{	
	TIM1->CCER = 0x5444;								// UH 输出使能, 高有效
	TIM1->CCMR1BIT.OC1M = PWM1ENBLE;		// 
	TIM1->CCMR1BIT.OC2M = PWM1ENBLE;		// 
	TIM1->CCMR2BIT.OC3M = PWM1ENBLE;		
}
void Stop_Motor(void)
{
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
    //OC1/OC1N/OC2/OC2N/OC3/OC3N
    TIM1->CCER &= 0xAAAA;
}

void Brake_Motor(void)
{
    TIM1->CCR1 = 0x00;
    TIM1->CCR2 = 0x00;
    TIM1->CCR3 = 0x00;
    TIM1->CCMR1BIT.OC1M = CHANNLLOW;//U+H
    TIM1->CCMR1BIT.OC2M = CHANNLLOW;//V_H
    TIM1->CCMR2BIT.OC3M = CHANNLLOW;//W_H
    TIM1->CCER |= 0x5555;
}

void Motor_Stop_Break(void)
{
	#if EN_MOTOR_BREAK
		Brake_Motor();
	#else
		Stop_Motor();
	#endif
}

void PwmDutyUpdate(uint16_t u16Duty)
{
//    if(u16Duty > MAX_DUTY)
//    {
//        u16Duty = MAX_DUTY;
//    }
//    else if(u16Duty < Motor_1st.BLDC.u16StartUp_PwmValue)
//    {
//        u16Duty = Motor_1st.BLDC.u16StartUp_PwmValue;
//    }

    TIM1->CCR1 = u16Duty;
    TIM1->CCR2 = u16Duty;
    TIM1->CCR3 = u16Duty;
}
