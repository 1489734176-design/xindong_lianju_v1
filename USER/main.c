/***********************************************************************************************************************
    @file    main.c
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
#define _MAIN_C_

/* Files include */
#include "platform.h"
#include "gpio_led_toggle.h"
#include "main.h"
#include "drv_pwm.h"
#include "parameter.h"
#include "board.h"

#include "mc_core.h"
#include "systick.h"
#include "user_control.h"
#include "motor_control.h"
#include "led.h"
#include "flash_data_save.h"
/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  This function is main entrance
  * @note   main
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void	delay_ms(u32 t)
{
	u16 i=0;
	u32 j=0;
	for(j=0;j<t;j++)
	{
		for(i=0;i<1000;i++);
	}

}
void debug_motor_on_off(void)
{
		static u16 count=0;
		count++;
		if(count>400)
		{
			count=0;
		}
		if(count<200)
		{
		user_list.flag.bits.gToolTrigger=1;
		}
		else 
		{
		user_list.flag.bits.gToolTrigger=0;
		}


}
int main(void)
{
//    PLATFORM_Init();

//    GPIO_LED_Toggle_Sample();
		u8 i=0;
        uint32_t ipd_primask;
//		LED1_ON;
//		LED2_ON;
//		LED3_ON;
//		LED4_ON;
//		LED5_ON;
//		LED6_ON;
			user_para_init();
			Systick_Init(0x00ffffff);
		fly_machine_init();
		systick_delay_us(10000);

//		systick_delay_us(1000000);
		Bsp_Gpio_Init();
		Peripheral_Init();
//		UH_VL_HPwmLOn();
//		PwmDutyUpdate(PWM_PRIOD_LOAD>>4);
		systick_delay_us(80000);//等待触发ADC采样

		mc_init();
		CurrentOffsetCalibration();//放user_para_init()完后
        /* PA7 原上电检测不变；安全允许时才恢复 PWM 复用并执行 MOS 自检。 */
        if(!motor_safety_poll())
        {
            TIM_CtrlPWMOutputs(TIM1, DISABLE);
            Stop_Motor();
            Bsp_Pwm_Init();
            PwmOutputsEnable();
            mose_check();
        }
        motor_safety_poll();	//放user_para_init()完后
		systick_delay_us(1001);
		read_user_data();//放user_para_init()完后
	
		for(i=0;i<20;i++)  //在mc_init（）和user_para_init()后，mc_init会先初始化方向
		{
				//user_direction_handle();
				user_list.flag.bits.gToolDir=Tool_Dir_CCW_Value;
				mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
		}
		Interrupt_Init();
		systick_delay_us(1000);
		Volt_Handler();
		Volt_Handler();
		Volt_Handler();
/* 仅保留故障报警，不再执行电量显示初始化。 */
#if LED_PA14_EN
		for(i=0;i<5;i++)
		{
			LedSoc_Run(user_list.VBus_Vx100,0,1);//电量初始化
		}
#endif
		for(i=0;i<20;i++) 
		{
			motor_on_off_control();
			systick_delay_us(1000); /* 上电按键消抖同样采用每毫秒一次采样。 */
		}
//		user_list.gears_level=2;
    while (1)
    {
			if(user_list.u8_1ms_flag)
			{
				//	GPIO_SetBits(GPIOA,GPIO_Pin_4);
				user_list.u8_1ms_flag=0;
			
//				user_direction_handle();
				Volt_Handler();
			//	debug_motor_on_off();
				motor_on_off_control();
				user_oc_handle();
				Mos_overload_handle();
				User_Temperature_Handler();
				pwm_duty_aim_deal();
				user_gears_handle();
//				user_WrenchKey_Handler();
				
				error_hold_updata();
				user_state_control();
				UO_Led_Handler();
				/* 最终电机使能再次受安全状态及全部原有故障约束。 */
                if(!motor_safety_poll() && user_list.flag.bits.gToolEn &&
                   machine_error_hold.userErr_hold.word == 0 &&
                   machine_error_hold.MC_error_hold.word == 0 &&
                   user_list.userErr.word == 0 && mc_core_list.MC_error.word == 0)
				{
					mc_core_list.motor_en=1;
				
				}
				else 
				{
					mc_core_list.motor_en=0;
				}
//				GPIO_Toggle(GPIOA,GPIO_Pin_4);
//				GPIO_SetBits(GPIOA,GPIO_Pin_4);
                /* 原子确认定位入口；库未返回时，安全停机先关驱动，暂不通电刹车。 */
                if(mc_core_list.ipd_star_flag==1)
                {
                    ipd_primask=__get_PRIMASK();
                    __disable_irq();
                    if(mc_core_list.ipd_star_flag==1 && !motor_safety_poll())
                    {
                        motor_ipd_active=1;
                    }
                    __set_PRIMASK(ipd_primask);
                    if(motor_ipd_active)
                    {
                        ipd_detect();
                        ipd_primask=__get_PRIMASK();
                        __disable_irq();
                        mc_core_list.ipd_star_flag=0;
                        motor_ipd_active=0;
                        /* 库退出后仍保持运行锁定，下一 PWM 周期只允许执行安全硬刹车。 */
                        motor_safety_poll();
                        __set_PRIMASK(ipd_primask);
                    }
                }

				
				
//				GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			}
		
			 
			
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

