#include "board.h"
#include "drv_adc.h"
#include "drv_pwm.h"
#include "parameter.h"
#include "hal_tim.h"
#include "led.h"
#include "user_control.h"
#include "systick.h"
void GPIO_Toggle(GPIO_TypeDef *GPIOn, uint16_t PINn)
{
    if (Bit_RESET == GPIO_ReadOutputDataBit(GPIOn, PINn))
    {
        GPIO_SetBits(GPIOn, PINn);
    }
    else
    {
        GPIO_ResetBits(GPIOn, PINn);
    }
}

void Bsp_Adc_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    RCC_AHBPeriphClockCmd(ADC_GPIO_CLK, ENABLE);			// OK

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;

    GPIO_InitStructure.GPIO_Pin     = SPEED_PIN;
    GPIO_Init(SPEED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = VBUS_PIN;
    GPIO_Init(VBUS_PORT, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin     = IPEAK_PIN;
    GPIO_Init(IPEAK_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin     = TMOS_PIN;
    GPIO_Init(TMOS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = BEMF_U_PIN;
    GPIO_Init(BEMF_U_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = BEMF_V_PIN;
    GPIO_Init(BEMF_V_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = BEMF_W_PIN;
    GPIO_Init(BEMF_W_PORT, &GPIO_InitStructure);
		
#if ON_OFF_DIGITAL_EN
    /* ON/OFF：PB0 高电平按下、低电平松开；下拉使未驱动时默认为松开。 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
#endif
    GPIO_InitStructure.GPIO_Pin = ON_START_PIN;
    GPIO_Init(ON_START_PORT, &GPIO_InitStructure);
}

/* PA14 按下为低，使用上拉输入；周期读取 IO，不配置外部中断。 */
static void Bsp_Safety_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    GPIO_PinAFConfig(SAFETY_PORT, SAFETY_PIN_SOURCE, GPIO_AF_7);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SAFETY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SAFETY_PORT, &GPIO_InitStructure);
    /* 上电已按住安全开关，先锁定，后续不得执行桥臂通电自检。 */
    if(SAFETY_PRESSED())
    {
        safety_stop_latched = 1;
    }
}

void Bsp_Pwm_Low(void)
{
    /* 本板六路桥臂为 PA0/5/6/8/9/10，切成低电平 GPIO，防止库重新使能 PWM。 */
    GPIO_ResetBits(GPIOA, BLDC1_UH_PIN | BLDC1_VH_PIN | BLDC1_WH_PIN |
                         BLDC1_UL_PIN | BLDC1_VL_PIN | BLDC1_WL_PIN);
    MODIFY_REG(GPIOA->CRL,
        GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
        GPIO_CRL_MODE6 | GPIO_CRL_CNF6,
        GPIO_CRL_MODE0_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0);
    MODIFY_REG(GPIOA->CRH,
        GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9 | GPIO_CRH_CNF9 |
        GPIO_CRH_MODE10 | GPIO_CRH_CNF10,
        GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE10_0);
}

/* 调用方先关闭 MOE 和通道；安全锁定时仅专用硬刹车入口可恢复桥臂复用。 */
void Bsp_Pwm_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
	
		/* PWM Clock Enable */
    RCC_AHBPeriphClockCmd(BLDC1_GPIO_CLK, ENABLE);
	
		/*selects the pin to used as Alternate function of PWM*/
    GPIO_PinAFConfig(BLDC1_UH_PORT, BLDC1_UH_PIN_SRC, BLDC1_UH_PIN_AF);
    GPIO_PinAFConfig(BLDC1_VH_PORT, BLDC1_VH_PIN_SRC, BLDC1_VH_PIN_AF);
    GPIO_PinAFConfig(BLDC1_WH_PORT, BLDC1_WH_PIN_SRC, BLDC1_WH_PIN_AF);

    GPIO_PinAFConfig(BLDC1_UL_PORT, BLDC1_UL_PIN_SRC, BLDC1_UL_PIN_AF);
    GPIO_PinAFConfig(BLDC1_VL_PORT, BLDC1_VL_PIN_SRC, BLDC1_VL_PIN_AF);
    GPIO_PinAFConfig(BLDC1_WL_PORT, BLDC1_WL_PIN_SRC, BLDC1_WL_PIN_AF);	

		/*Configure GPIO pin : PWM_Pin */
    GPIO_InitStructure.GPIO_Pin     = BLDC1_UH_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(BLDC1_UH_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BLDC1_VH_PIN;
    GPIO_Init(BLDC1_VH_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BLDC1_WH_PIN;
    GPIO_Init(BLDC1_WH_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BLDC1_UL_PIN;
    GPIO_Init(BLDC1_UL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BLDC1_VL_PIN;
    GPIO_Init(BLDC1_VL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BLDC1_WL_PIN;
    GPIO_Init(BLDC1_WL_PORT, &GPIO_InitStructure);	
}

void Bsp_dir_init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

	
	
			/* LED Clock Enable */
    RCC_AHBPeriphClockCmd(DIR_RCC_CLOCKGPIO, ENABLE);		// OK
	//	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_7);
		/*Configure GPIO pin : LED_Pin */
		GPIO_InitStructure.GPIO_Pin     =  DIR_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
    GPIO_Init(DIR_PORT, &GPIO_InitStructure);	
}


void Bsp_EN_Port_init(void)
{

	u16 i=0;
	#if PWM_IO_IT_BK_EN
	  EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
	#endif
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

	
	
			/* LED Clock Enable */
    RCC_AHBPeriphClockCmd(EN_PIN_RCC_CLOCKGPIO, ENABLE);		// OK
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SYSCFG, ENABLE);
//		GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_7);
		/*Configure GPIO pin : LED_Pin */
		GPIO_InitStructure.GPIO_Pin     =  EN_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_FLOATING;
//		GPIO_SetBits(EN_PORT, EN_PIN);
    GPIO_Init(EN_PORT, &GPIO_InitStructure);	
		TIM1->BDTR&=( ~TIM_Break_Enable);
		#if PWM_IO_BKIN_EN
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_2); /* TIM1_BKIN */
		#endif
		 i=5000;
		while((GPIO_ReadInputDataBit(EN_PORT,EN_PIN)==0))
		{
			i--;
			if(i==0)
			{
				break;
			}
		}
		

	
		user_list.En_pin_dir=0;

	#if PWM_IO_IT_BK_EN
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line4;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
		
		
		  NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
	#endif
	
//		GPIO_SetBits(EN_PORT, EN_PIN);
}

void EN_Port_in(void)
{
	
		
	
	u16 i=0;
	  GPIO_InitTypeDef GPIO_InitStructure;
	if(user_list.En_pin_dir==1)
	{
    GPIO_StructInit(&GPIO_InitStructure);
		/*Configure GPIO pin : LED_Pin */
		GPIO_InitStructure.GPIO_Pin     =  EN_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_FLOATING;
//		GPIO_SetBits(EN_PORT, EN_PIN);
    GPIO_Init(EN_PORT, &GPIO_InitStructure);	
	  #if PWM_IO_BKIN_EN
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_2); /* TIM1_BKIN */
		#endif
	  i=5000;
		while((GPIO_ReadInputDataBit(EN_PORT,EN_PIN)==0))
		{
			i--;
			if(i==0)
			{
				break;
			}
		}
	 #if PWM_IO_BKIN_EN
  TIM_ClearFlag(TIM1, TIM_FLAG_Break);
	TIM1->BDTR|=TIM_Break_Enable;
	#endif
		user_list.En_pin_dir=0;

	}
}

void EN_Port_Out_L(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
	if(user_list.En_pin_dir==0)
	{
		 user_list.En_pin_dir=1;//先置一防止中断读取问题
		 
		TIM1->BDTR&=( ~TIM_Break_Enable);   /*!< Break inputs (BRK and CSS clock failure event) enabled */
		/*Configure GPIO pin : LED_Pin */
		GPIO_InitStructure.GPIO_Pin     =  EN_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
		GPIO_ResetBits(EN_PORT, EN_PIN);
    GPIO_Init(EN_PORT, &GPIO_InitStructure);	

		GPIO_ResetBits(EN_PORT, EN_PIN);
	
	}
	else 
	{
	
		GPIO_ResetBits(EN_PORT, EN_PIN);
	}
	

}

void Bsp_Gpio_Init(void)
{
	
    /* 上电先采样安全开关并保持六路输出低电平，再初始化其余 IO。 */
    Bsp_Safety_Init();
    Bsp_Pwm_Low();
    Bsp_Adc_Init();
	led_init();
//	Bsp_dir_init();
	Bsp_EN_Port_init();

}




void TIM14_Configure(void)
{
//    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM14, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler         = 3;
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period            = 0xffff;
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM14, TIM_FLAG_Update);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

//    NVIC_InitStruct.NVIC_IRQChannel = TIM14_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM14, DISABLE);
}


void Peripheral_Init(void)
{
	drv_adc_init();
	Drv_pwm_init(TIM1, PWM_PRIOD_LOAD, DEADTIME_LOAD);
	TIM14_Configure();
}


void NVIC_Configure(uint8_t ch, uint8_t pri)
{
    NVIC_InitTypeDef  NVIC_InitStruct;

    /** Initialization ADC interrupt */
    NVIC_InitStruct.NVIC_IRQChannel = ch;
    NVIC_InitStruct.NVIC_IRQChannelPriority = pri;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
void Interrupt_Init(void)
{
//		/** Initialization Systick interrupt */
//    NVIC_SetPriority(SysTick_IRQn, 2);
	
    /** Initialization ADC interrupt */
    NVIC_Configure(ADC1_IRQn, 1);
    /** ADC EOF interrupt enabled */
//    ADC_ITConfig(ADC1, ADC_IT_EOS, ENABLE);
//    ADC_ClearITPendingBit(ADC1, ADC_IT_EOS);
	
	   ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
//    /** Initialization TIM interrupt */
    NVIC_Configure(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    /** TIM Break interrupt enabled */
    TIM_ClearFlag(TIM1, TIM_FLAG_Break);
		TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
	
    /** TIM Update interrupt enabled */
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
//	
//		/** Initialization COMP interrupt */
		NVIC_Configure(TIM14_IRQn, 1);
		/** TIM14 Update interrupt enabled */
		TIM_ClearFlag(TIM14, TIM_FLAG_Update);
		TIM_ITConfig(TIM14, TIM_IT_Update, DISABLE);
		
		#if PWM_IO_IT_BK_EN
		NVIC_Configure(EXTI4_15_IRQn,0);
		#endif
//		
//			NVIC_Configure(TIM6_IRQn, 2);
//		/** TIM6 Update interrupt enabled */
//		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
//		TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
//		
//		
//		
//					NVIC_Configure(TIM13_IRQn, 2);
//			/** TIM13 Update interrupt enabled */
//			TIM_ClearFlag(TIM13, TIM_FLAG_Update);
//			TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);		
		
		#if EN_MOTOR_LOCK_DETECT
			/** Initialization COMP interrupt */
			NVIC_Configure(TIM13_IRQn, 2);
			/** TIM13 Update interrupt enabled */
			TIM_ClearFlag(TIM13, TIM_FLAG_Update);
			TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);		
		#endif
}
