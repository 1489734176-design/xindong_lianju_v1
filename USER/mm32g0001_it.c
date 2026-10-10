/***********************************************************************************************************************
    @file    mm32g0001_it.c
    @author  FAE Team
    @date    17-Nov-2023
    @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
  **********************************************************************************************************************
    @attention

    <h2><center>&copy; Copyright(c) <2023> <MindMotion></center></h2>

      Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
    following conditions are met:
    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
       the following disclaimer in the documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
       promote products derived from this software without specific prior written permission.

      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#define _MM32G0001_IT_C_

/* Files include */
#include "platform.h"
#include "mm32g0001_it.h"
#include "mc_core.h"
#include "board.h"
#include "drv_adc.h"
#include "user_control.h"
#include "parameter.h"
#include "motor_control.h"
#include "drv_pwm.h"
#include "led.h"
#	include "motor_config.h"
void Get_ADC_Result()
{		
	mc_core_list.Board_ADC_DATA.u16BemfU 			= (uint16_t)GET_ADC_VALUE(BEMF_U_CHANNEL);
//	if(mc_core_list.Board_ADC_DATA.u16BemfU>40)
//	{
//	mc_core_list.Board_ADC_DATA.u16BemfU-=40;
//	}
	mc_core_list.Board_ADC_DATA.u16BemfV 			= (uint16_t)GET_ADC_VALUE(BEMF_V_CHANNEL);
	mc_core_list.Board_ADC_DATA.u16BemfW 			= (uint16_t)GET_ADC_VALUE(BEMF_W_CHANNEL);
	mc_core_list.Board_ADC_DATA.u16NTC_MOS 		= (uint16_t)GET_ADC_VALUE(TMOS_CHANNEL);
	mc_core_list.Board_ADC_DATA.u16VR 				= (uint16_t)GET_ADC_VALUE(SPEED_CHANNEL);
	mc_core_list.Board_ADC_DATA.u16Vbus	 			= (uint16_t)GET_ADC_VALUE(VBUS_CHANNEL);
	mc_core_list.Board_ADC_DATA.u16I_Peak 	= (uint16_t)GET_ADC_VALUE(IPEAK_CHANNEL);
/* 数字方式不读取 PB0 的旧 ADC 值，按键电平由主循环读取。 */
#if !ON_OFF_DIGITAL_EN
    user_list.on_start_adc = (uint16_t)GET_ADC_VALUE(ON_START_CHANNEL);
#endif
}

/**
  * @addtogroup Peripheral
  * @{
  */

/**
  * @addtogroup Peripheral_SampleFunction
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  This function handles NMI exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void NMI_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles Hard Fault exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/***********************************************************************************************************************
  * @brief  This function handles SVCall exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SVC_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles PendSVC exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PendSV_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles SysTick Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SysTick_Handler(void)
{
	
//    if (0 != PLATFORM_DelayTick)
//    {
//        PLATFORM_DelayTick--;
//    }
}
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
 		static unsigned char u16PWMTimeCnt = 0;
    if (RESET != TIM_GetITStatus(TIM1, TIM_IT_Update))
    {
//				GPIO_Toggle(GPIOA,GPIO_Pin_4);
			#if PWM_IO_IT_BK_EN
			if(user_list.En_pin_dir==0)
			{
				if(GPIO_ReadInputDataBit(EN_PORT,EN_PIN)==0)
				{
						TIM_CtrlPWMOutputs(TIM1, DISABLE);
						machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
				    Stop_Motor();//?
				}
			}
			
			if(TIM1->BDTR&TIM_Break_Enable)
			{
				machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
			}
			#endif
			MC_Machine_State();
			#if (Block_Protect_En)
				motor_block_detect();
			#endif
			
						// 1ms时间片计数
			if(++u16PWMTimeCnt >= SLOWLOOP_1ms_CNT_LOAD)
			{
				user_list.u8_1ms_flag=1;
				u16PWMTimeCnt = 0;
				#if LED_SCAN_EN
				LED_Scan();
				#endif
			}
			//	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
			

		//		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
		
		 if (RESET != TIM_GetITStatus(TIM1, TIM_IT_Break))
		{
				machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
				TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
		}
}
void ADC_IRQHandler(void)
{	
	#if IBus_Peak_OCP_ONCE_EN
		u16 peak_cal_data=0;
	#endif
    if (RESET != ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
				
				Get_ADC_Result();
			#if IBus_Peak_OCP_ONCE_EN
			if(mc_core_list.Board_ADC_DATA.u16I_Peak>mc_core_list.CurrentOffset_ad_data)
			{
				peak_cal_data=mc_core_list.Board_ADC_DATA.u16I_Peak-mc_core_list.CurrentOffset_ad_data;
			}
			else 
			{
			peak_cal_data=0;
			}
			if(peak_cal_data>IBus_Peak_OCP_ONCE_AdcValue)
			{
					TIM_CtrlPWMOutputs(TIM1, DISABLE);
					machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
					Stop_Motor();//?
			
			}
		#endif
                /* 保留 ADC 采样及过流处理，锁定时禁止进入电机库换相。 */
                if(!motor_safety_poll())
                {
                    mc_core_adc_isr_handle();
                }
				
//				GPIO_Toggle(GPIOA,GPIO_Pin_4);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }

}

void TIM14_IRQHandler(void)
{
    if (RESET != TIM_GetITStatus(TIM14, TIM_IT_Update))
    {
		
            /* 已挂起的换相定时中断也必须服从安全锁定。 */
            if(!motor_safety_poll())
            {
                mc_core_timer14_isr_handle();
            }
      TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
    
	}
}


void EXTI4_15_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_Line4))
    {
			#if PWM_IO_IT_BK_EN
				TIM_CtrlPWMOutputs(TIM1, DISABLE);
				machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
			  Stop_Motor();//?
			#endif
        EXTI_ClearITPendingBit(EXTI_Line4);
    }


}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/********************************************** (C) Copyright MindMotion **********************************************/

