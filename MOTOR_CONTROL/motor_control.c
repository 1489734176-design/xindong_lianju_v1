#include "motor_control.h"
#include "mc_core.h"
#include "drv_pwm.h"
#include "parameter.h"
#include "board.h"
#include "drv_adc.h"
#include "user_control.h"
#include "systick.h"
#include "motor_config.h"

extern u32 PI_sum_i;
extern u32 PI_p;
extern u32 pwm_duty_aim_PI_OUT;
Motor_Control_list motor_control_list;

void mc_init(void)
{
		McCore_Param_t p;
	motor_control_list.bldc_state=MOTOR_STOP;

	mc_core_list.MC_error.word=0;
	mc_core_list.time_count_60_degree=0;
	mc_core_list.bldc_bemf_detected=0;
	mc_core_list.pwm_actual_runing_data=0;
	mc_core_list.mc_moving=0;
	mc_core_list.mc_dir=Tool_Dir_Init;
	mc_core_list.ipd_star_flag=0;
	mc_core_list.motor_en=0;
	mc_core_list.block_count=0;
	mc_core_list.CurrentOffset_ad_data=0;
	mc_core_list.motor_bemf_runing=0;
	mc_core_list.Run_SectorCnt=0;
	mc_core_list.motion_en=Motion_En;
	mc_core_list.motion_processed_result=0;

	motor_control_list.hbrake_en=HBrake_En;
	motor_control_list.sbrake_en=SBrake_En;


	p.shift_closedloop_angle=ShiftTime_ClosedLoop_Angle;
	p.mask_closedloop_angle=MaskTime_ClosedLoop_Angle;
	p.mask_openloop_timer_count=MaskTime_OpenLoop_TIMER_COUNT;
	p.shift_openloop_timer_count=ShiftTime_OpenLoop_TIMER_COUNT;
	p.mask_min_timer_count=MaskTime_Min_TIMER_COUNT;
	p.mask_max_timer_count=MaskTime_Max_TIMER_COUNT;
	p.shift_min_timer_count=ShiftTime_Min_TIMER_COUNT;
	p.shift_max_timer_count=ShiftTime_Max_TIMER_COUNT;
	p.ipd_idle_time=MotorStart_IPD_IdleTime;
	p.ipd_pulse_time=MotorStart_IPD_PulseTime;
	p.force_shihf_phase_en=ForceChangePhase_En;
	p.force_shihf_phase_timer_count_set=ForceChangePhase_CntSet;
	p.lose_step_protect_en=LosStep_Protect_En;
	p.lose_step_protect_count_set=LosStep_Protect_CntSet;
	p.i_peak_ad_channel=IPEAK_CHANNEL;
	p.motion_check_time_count=Motion_CheckTime_Count;
	p.motion_pass_count_set=Motion_Pass_CntSet;
	p.lack_phase_delta=(u16)(CURRENT_ADC_PER_A);
	p.ibus_peak_ocp_once_en=IBus_Peak_OCP_ONCE_EN;
	p.ibus_peak_ocp_once_adc_value=IBus_Peak_OCP_ONCE_AdcValue;

	mc_core_list_init(&p);
}
void motor_block_detect(void)
{
	u32 block_ipeak_data=0;
	
	//	u32 block_ipeak_data=mc_core_list.Board_ADC_DATA.u16I_Peak;
	
			if(mc_core_list.Board_ADC_DATA.u16I_Peak>mc_core_list.CurrentOffset_ad_data)
		{
			block_ipeak_data=mc_core_list.Board_ADC_DATA.u16I_Peak-mc_core_list.CurrentOffset_ad_data;
			
		}
		else 
		{
			block_ipeak_data=0;
		}
	#if Block_Protect_Current_En
	
		if(motor_control_list.bldc_state==MOTOR_RUN||motor_control_list.bldc_state==MotorStart_Drag_OPENLOOP)
		{
			
					mc_core_list.block_count++;
				  if(mc_core_list.block_count>Block_Protect_MAX_TIME)
					{
							mc_core_list.MC_error.bits.gBlock=1;
							mc_core_list.block_recover_count=0;
					
					}
					
						if(block_ipeak_data>Block_Protect_Current1_AdcValue)
					{
						mc_core_list.block_count1++;
						if(mc_core_list.block_count1>Block_Protect_Current1_TIME)
						{
						
							mc_core_list.MC_error.bits.gBlock=1;
							mc_core_list.block_recover_count=0;
						}
					
					}		


					 if(block_ipeak_data>Block_Protect_Current2_AdcValue)
					{
						mc_core_list.block_count2++;
						if(mc_core_list.block_count2>Block_Protect_Current2_TIME)
						{
						
							mc_core_list.MC_error.bits.gBlock=1;
							mc_core_list.block_recover_count=0;
						}
					
					}

					 if(block_ipeak_data>Block_Protect_Current3_AdcValue)
					{
						mc_core_list.block_count3++;
						if(mc_core_list.block_count3>Block_Protect_Current3_TIME)
						{
						
							mc_core_list.MC_error.bits.gBlock=1;
							mc_core_list.block_recover_count=0;
						}
					
					}	

					 if(block_ipeak_data>Block_Protect_Current4_AdcValue)
					{
						mc_core_list.block_count4++;
						if(mc_core_list.block_count4>Block_Protect_Current4_TIME)
						{
						
							mc_core_list.MC_error.bits.gBlock=1;
							mc_core_list.block_recover_count=0;
						}
					
					}						
			
		}
		else
		{
//				if(mc_core_list.block_count)
//				{
//					mc_core_list.block_count--;
//				}
//				if(mc_core_list.block_count1)
//				{
//					mc_core_list.block_count1--;
//				}
//				if(mc_core_list.block_count2)
//				{
//					mc_core_list.block_count2--;
//				}
//				if(mc_core_list.block_count3)
//				{
//					mc_core_list.block_count3--;
//				}
//				if(mc_core_list.block_count4)
//				{
//					mc_core_list.block_count4--;
//				}
	
//				if((mc_core_list.block_count==0)&&(mc_core_list.block_count1==0)&&(mc_core_list.block_count2)==0 \
//					&&(mc_core_list.block_count3==0)&&(mc_core_list.block_count4==0))
//				{
//					mc_core_list.MC_error.bits.gBlock=0;
//				
//				}	
					mc_core_list.block_count=0;
					mc_core_list.block_count1=0;
					mc_core_list.block_count2=0;
					mc_core_list.block_count3=0;
					mc_core_list.block_count4=0;
					mc_core_list.block_recover_count++;
					if(mc_core_list.block_recover_count>Block_Protect_Recover_TIME)
					{
						mc_core_list.MC_error.bits.gBlock=0;
						mc_core_list.block_recover_count=0;
					}


		}
	
	
	#else
	if(motor_control_list.bldc_state==MOTOR_RUN||motor_control_list.bldc_state==MotorStart_Drag_OPENLOOP)
	{
		mc_core_list.block_count++;
		
		if(mc_core_list.pwm_actual_runing_data<Block_Protect_LowDuty_DATA)
		{
			if(mc_core_list.block_count>Block_Protect_LowDuty_COUNT)
			{
				
					//mos过载处理
				if(mc_core_list.MC_error.bits.gBlock==0)
				{
					user_list.Mos_overload_count++;
				
				}
				mc_core_list.block_recover_count=0;
				mc_core_list.MC_error.bits.gBlock=1;
			}
		}
		else
		{			
			if(mc_core_list.block_count>Block_Protect_HighDuty_COUNT)
			{
					//mos过载处理
				if(mc_core_list.MC_error.bits.gBlock==0)
				{
					user_list.Mos_overload_count++;
				
				}
				mc_core_list.block_recover_count=0;
				mc_core_list.MC_error.bits.gBlock=1;
			}
		
		}
	}
	else 
	{
		mc_core_list.block_count=0;
					mc_core_list.block_recover_count++;
			if(mc_core_list.block_recover_count>Block_Protect_Recover_TIME)
			{
				mc_core_list.MC_error.bits.gBlock=0;
				mc_core_list.block_recover_count=0;
			}

	
	}
	#endif
	
}
#if 0
void pwm_set(void)
{
		static u8 pwm_add_count=0;
  	static u8 pwm_sub_count=0;
		u8 limit_current_on=0;
		

		#if(LIMIT_PEAK_CURRENT_EN)
		if(mc_core_list.Board_ADC_DATA.u16I_Peak>LIMIT_PEAK_CURRENT_AdcValue)
		{
		
			limit_current_on=1;
		}
		#endif
	
		#if(LIMIT_AVG_CURRENT_EN)
	
			if(user_list.IBusAvg_Adc>LIMIT_AVG_CURRENT_AdcValue)
		{
		
			limit_current_on=1;
		}
		#endif
		if((mc_core_list.pwm_actual_runing_data<motor_control_list.pwm_duty_aim)&&(limit_current_on==0))
		{
				pwm_add_count++;
				if(pwm_add_count>=PWM_ADD_GAP)
				{
					mc_core_list.pwm_actual_runing_data+=PWM_ADD_VALUE;
					pwm_add_count=0;
				}
				
		}
		else if(mc_core_list.pwm_actual_runing_data>motor_control_list.pwm_duty_aim||limit_current_on)
		{
			pwm_sub_count++;
			if(pwm_sub_count>=PWM_SUB_GAP)
			{
				mc_core_list.pwm_actual_runing_data-=PWM_SUB_VALUE;
				pwm_sub_count=0;
			}
		}
		
//		if(limit_current_on)
//		{
//		GPIO_Toggle(GPIOA,GPIO_Pin_4);
//		}
		if(mc_core_list.pwm_actual_runing_data<LIMIT_MIN_PWN_DUTY)
		{
			mc_core_list.pwm_actual_runing_data=LIMIT_MIN_PWN_DUTY;
		}
		if(mc_core_list.pwm_actual_runing_data>LIMIT_MAX_PWN_DUTY)
		{
			mc_core_list.pwm_actual_runing_data=LIMIT_MAX_PWN_DUTY;
		}
		 PwmDutyUpdate(mc_core_list.pwm_actual_runing_data);


}
#else 
void pwm_set(void)
{
		static u8 pwm_add_count=0;
  	static u8 pwm_sub_count=0;
		static u8 pwm_add_fast_count=0;
  	static u8 pwm_sub_fast_count=0;
		u8 limit_current_on=0;
	static u16 limit_current_on_hold_count=0;
	#if(LIMIT_PEAK_CURRENT_EN)
    uint16_t IBusPeak_Adc_temp=0;
	#endif
	#if(LIMIT_AVG_CURRENT_EN)
		uint16_t IBusAvg_Adc_temp=0;
	#endif
	//u32 block_ipeak_data=mc_core_list.Board_ADC_DATA.u16I_Peak;
		#if(LIMIT_PEAK_CURRENT_EN)
			if(mc_core_list.Board_ADC_DATA.u16I_Peak>mc_core_list.CurrentOffset_ad_data)
		{
			IBusPeak_Adc_temp=mc_core_list.Board_ADC_DATA.u16I_Peak-mc_core_list.CurrentOffset_ad_data;
			
		}
		else 
		{
			IBusPeak_Adc_temp=0;
		}
		#endif
		
//		#if(LIMIT_AVG_CURRENT_EN)
//		if( mc_core_list.Board_ADC_DATA.u16I_SUM_Avg> mc_core_list.CurrentOffset_ad_data)
//		{
//			IBusAvg_Adc_temp=mc_core_list.Board_ADC_DATA.u16I_SUM_Avg-mc_core_list.CurrentOffset_ad_data;
//		}
//		else 
//		{
//			IBusAvg_Adc_temp=0;
//		}
//		#endif


		if(limit_current_on_hold_count)
		{
			limit_current_on_hold_count--;
		}
		else 
		{
			user_list.limit_current_on_flag=0;
		}

		#if(LIMIT_PEAK_CURRENT_EN)
		if(IBusPeak_Adc_temp>LIMIT_PEAK_CURRENT_AdcValue)
		{
		
			user_list.limit_current_on_flag=1;
			limit_current_on_hold_count=160;
			limit_current_on=1;
		}
		#endif
	
		#if(LIMIT_AVG_CURRENT_EN)
	
			if(user_list.IBusAvg_Adc>LIMIT_AVG_CURRENT_AdcValue)
		{
		
					user_list.limit_current_on_flag=1;
	  	limit_current_on_hold_count=160;
			limit_current_on=1;
			
		}
		#endif
		if((motor_control_list.pwm_duty_virtual<motor_control_list.pwm_duty_aim))
		{
					pwm_add_count++;
					if(pwm_add_count>=PWM_ADD_GAP)
					{
						
						pwm_add_count=0;
						motor_control_list.pwm_duty_virtual+=PWM_ADD_VALUE;
						if(motor_control_list.pwm_duty_virtual>motor_control_list.pwm_duty_aim)
						{
							motor_control_list.pwm_duty_virtual=motor_control_list.pwm_duty_aim;
						
						}
						
					}
								
		}
		else if(motor_control_list.pwm_duty_virtual>motor_control_list.pwm_duty_aim)
		{
			pwm_sub_count++;
			if(pwm_sub_count>=PWM_SUB_GAP)
			{
				pwm_sub_count=0;
				if(motor_control_list.pwm_duty_virtual>PWM_SUB_VALUE)
				{
					motor_control_list.pwm_duty_virtual-=PWM_SUB_VALUE;
				}
				if(motor_control_list.pwm_duty_virtual<motor_control_list.pwm_duty_aim)
				{
					motor_control_list.pwm_duty_virtual=motor_control_list.pwm_duty_aim;
				
				}
			}
		}
		
		if((mc_core_list.pwm_actual_runing_data<motor_control_list.pwm_duty_virtual)&&(limit_current_on==0))
		{
		
					pwm_add_fast_count++;
				if(pwm_add_fast_count>=PWM_ADD_FAST_GAP)
				{
					pwm_add_fast_count=0;
					mc_core_list.pwm_actual_runing_data+=PWM_ADD_FAST_VALUE;
					if(mc_core_list.pwm_actual_runing_data>motor_control_list.pwm_duty_virtual)
					{
						mc_core_list.pwm_actual_runing_data=motor_control_list.pwm_duty_virtual;
					
					}
				}
			
		}
		if(mc_core_list.pwm_actual_runing_data>motor_control_list.pwm_duty_virtual||limit_current_on)
		{
			
							pwm_sub_fast_count++;
				if(pwm_sub_fast_count>=PWM_SUB_FAST_GAP)
				{
					pwm_sub_fast_count=0;
					if(mc_core_list.pwm_actual_runing_data>PWM_SUB_FAST_VALUE)
					{
						mc_core_list.pwm_actual_runing_data-=PWM_SUB_FAST_VALUE;
					}
					if((mc_core_list.pwm_actual_runing_data<motor_control_list.pwm_duty_virtual)&&(limit_current_on==0))
					{
						mc_core_list.pwm_actual_runing_data=motor_control_list.pwm_duty_virtual;
					
					}
				}
			

		}		
		
//		if(limit_current_on)
//		{
//		GPIO_Toggle(GPIOA,GPIO_Pin_4);
//		}
		if(mc_core_list.pwm_actual_runing_data<LIMIT_MIN_PWN_DUTY)
		{
			mc_core_list.pwm_actual_runing_data=LIMIT_MIN_PWN_DUTY;
		}
		if(mc_core_list.pwm_actual_runing_data>LIMIT_MAX_PWN_DUTY)
		{
			mc_core_list.pwm_actual_runing_data=LIMIT_MAX_PWN_DUTY;
		}
		 PwmDutyUpdate(mc_core_list.pwm_actual_runing_data);


}

#endif

void CurrentOffsetCalibration(void)
{
		u8 i=0;
		u32 ibus_adc_sum=0;
		Stop_Motor();
		ADC_Cmd(ADC1, DISABLE);
		ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);		// 关闭ADC转换中断
		TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);	// 关闭TIM1更新中断
		
    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, 0, IPEAK_CHANNEL);	
    ADC_AnyChannelCmd(ADC1, ENABLE);
	
	
	
		ADC_ExternalTrigConvCmd(ADC1, DISABLE);			// 关闭ADC外部触发转换
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);					// 清除ADC转换标志位
	  ADC_SampleTimeConfig(ADC1, ADC_SampleTime_3_5);
		ADC_Cmd(ADC1, ENABLE);
		systick_delay_us(500);
	
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
		ibus_adc_sum=0;
		for(i=0;i<16;i++)
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
			ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
			ibus_adc_sum += (uint16_t)GET_ADC_VALUE(IPEAK_CHANNEL);
		}
		mc_core_list.CurrentOffset_ad_data=ibus_adc_sum>>4;
		ADC_Cmd(ADC1, DISABLE);
		systick_delay_us(100);
		drv_adc_init();

}

void mos_check_current(u8 mose_check_inedx)
{

			u8 i=0;
    u16 mos_check_ad=0;
    /* 自检尚未开放周期中断，每次桥臂通电前主动轮询安全开关。 */
    if(motor_safety_poll())
    {
        return;
    }
			switch(mose_check_inedx)
			{
				case 0:
						UL_HOffLOn();
					break;
				case 1:
						VL_HOffLOn();
					break;
				case 2:
						WL_HOffLOn();
					break;
				case 3:
					UH_HOnLOff();
					break;
				case 4:
					VH_HOnLOff();
					break;
				case 5:
					WH_HOnLOff();
					break;
				default :
					break;				
			
			}
			for(i=0;i<20;i++)
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
			ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
            /* 自检采样期间触发安全开关，立即停止本次桥臂通电。 */
            if(motor_safety_poll())
            {
                break;
            }
            mos_check_ad= (uint16_t)GET_ADC_VALUE(IPEAK_CHANNEL);
			if(mos_check_ad>mc_core_list.CurrentOffset_ad_data)
			{
			
			mos_check_ad=mos_check_ad-mc_core_list.CurrentOffset_ad_data;
			}
			else 
			{
			
			mos_check_ad=0;
			}
			if(mos_check_ad>MOS_SHORT_CURRENT_AdcValue||	machine_error_hold.userErr_hold.bits.gIBusAcmpOCP==1)
			{
				Stop_Motor();
				machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=0;
				
				if(mose_check_inedx<3)
				{
					user_list.userErr.bits.gMosBridgeH=1;
				}
				else 
				{
					user_list.userErr.bits.gMosBridgeL=1;
				}
				
				break;
			}
		}
		
		Stop_Motor();
}

void mose_check(void)
{
    u8 i=0;
    /* 上电已按下安全开关时，跳过会给桥臂通电的 MOS 自检。 */
    if(motor_safety_poll())
    {
        return;
    }
		Stop_Motor();
		ADC_Cmd(ADC1, DISABLE);
		ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);		// 关闭ADC转换中断
		TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);	// 关闭TIM1更新中断
		ADC_ExternalTrigConvCmd(ADC1, DISABLE);			// 关闭ADC外部触发转换
    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, 0, IPEAK_CHANNEL);	
	
		
    ADC_AnyChannelCmd(ADC1, ENABLE);
	  ADC_Cmd(ADC1, ENABLE);
	  systick_delay_us(500);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
   for(i=0;i<6;i++)
	{
        /* 锁定后跳过其余桥臂，仍执行函数末尾的 ADC 配置恢复。 */
        if(motor_safety_poll())
        {
            break;
        }
        mos_check_current(i);
		
		systick_delay_us(800);
	}
	
//		Systick_delay_us(2);

	
	
//		Systick_delay_us(2);



		ADC_Cmd(ADC1, DISABLE);
			systick_delay_us(500);
			drv_adc_init();
		 ADC_ClearFlag(ADC1, ADC_FLAG_EOC);					// 清除ADC转换标志位
//		 ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);		// 


}

#if 0
void start_mc_init(void)
{
//	mc_core_list.last_step_index=255;
	mc_core_list.aglin_count=0;

}
void align_control(void)
{
		mc_core_list.bldc_step_current=0;
		step_control(mc_core_list.bldc_step_current,mc_core_list.mc_dir);

}

void drag_control_init(void)
{
	
		mc_core_list.drag_step_count=600;
		mc_core_list.drag_step_count_compare=500;

}
u8 drag_control(void)
{
//	u16 pwm_ad_data;
	u8 res=0;
		mc_core_list.drag_step_count++;
		if(mc_core_list.drag_step_count>(mc_core_list.drag_step_count_compare))
		{
				if(mc_core_list.drag_step_count_compare>80)
				{
					res=0;
				mc_core_list.drag_step_count_compare=mc_core_list.drag_step_count_compare-10;
				}
				else 
				{
					res=1;
				}
				 change_phase();
			mc_core_list.crossed_zero_timer_count=read_timer_count();
//			mc_core_list.crossed_zero_timer_count=TIM13->CNT;
//			mc_core_list.swith_timer_count=mc_core_list.swith_timer_count+(((u32)mc_core_list.tim13_h_count)<<16);
		mc_core_list.drag_step_count=0;
		}

	

		return res;


}
#endif
void MC_Machine_State(void)
{
	static u16 hbreak_count=0;
	static u16 sbreak_count=0;
	static u16 break2stop_count=0;
    static u8 motionless_count=0;
    static u8 safety_brake_started=0;

    /* 每个 PWM 周期轮询安全开关，锁定时只允许进入硬刹车及其结束过渡。 */
    if(motor_safety_poll())
    {
        if(!safety_brake_active || motor_ipd_active)
        {
            hbreak_count = 0;
            sbreak_count = 0;
            break2stop_count = 0;
            motionless_count = 0;
            safety_brake_started = 0;
            return;
        }
        if(!safety_brake_started)
        {
            /* 刹车只初始化一次，不因反复轮询或长按安全开关重置刹车时间。 */
            hbreak_count = 0;
            break2stop_count = 0;
            safety_brake_started = 1;
            Safety_Brake_Motor(1);
            if(!safety_brake_active)
            {
                safety_brake_started = 0;
                return;
            }
        }
    }
    else
    {
        safety_brake_started = 0;
    }

	switch (motor_control_list.bldc_state)
	{
	
		case MOTOR_STOP :
			
			Stop_Motor();
	  	mc_core_list.pwm_actual_runing_data=0;
			motor_control_list.pwm_duty_virtual=0;
		  PI_sum_i=0;
			PI_p=0;
	    pwm_duty_aim_PI_OUT=0;
			if(mc_core_list.motor_en==1&&machine_error_hold.userErr_hold.word==0&&machine_error_hold.MC_error_hold.word==0)
			{
//				start_mc_init();
//				mc_core_list.ipd_star_flag=1;
				motor_control_list.bldc_state=MOTOR_MOTION;
				mc_core_list.mc_moving=0;
				mc_core_list.motion_processed_result=0;
				motionless_count=0;
//				start_mc_init();
//				mc_core_list.ipd_star_flag=1;
//				mc_core_list.bldc_state=Motor_Align;
			}
			break;
		case MOTOR_MOTION :
			
				if(mc_core_list.motor_en==0||machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
				{
						motor_control_list.bldc_state=MOTOR_STOP;
						mc_core_list.mc_moving=0;	
				}
				if(mc_core_list.Board_ADC_DATA.u16BemfU>Motion_ADC_THRESHOLD||mc_core_list.Board_ADC_DATA.u16BemfV>Motion_ADC_THRESHOLD||mc_core_list.Board_ADC_DATA.u16BemfW>Motion_ADC_THRESHOLD)
				{
					motion_handle();
					if(mc_core_list.motion_processed_result==1)
					{
						motor_control_list.bldc_state=MOTOR_READY;//滑行检测失败
					
					}
					else if(mc_core_list.motion_processed_result==2)
					{
						
						if(mc_core_list.motion_en==0)
						{
							

							
							motor_control_list.bldc_state=MOTOR_READY;
						}
						else 
						{
							mc_core_list.mc_moving=0;	
							mc_core_list.motor_bemf_runing=1;
							motor_control_list.bldc_state=MOTOR_RUN;
						}
					
					}
						
						motionless_count=0;
				}
				else
				{
					motionless_count++;
					if(motionless_count>10)
					{
						if(mc_core_list.mc_moving)//滑行检测失败
						{
						  motor_control_list.bldc_state=MOTOR_READY;
							hbreak_count=0;
						}
						else 
						{
							
					  	mc_core_list.bldc_bemf_detected=0;
				   	 motor_control_list.bldc_state=MOTOR_POSITION;
					   mc_core_list.ipd_star_flag=1;
//						motor_control_list.bldc_state=MOTOR_READY;
//							hbreak_count=0;
						}
					}
				
				}
			break;
		case MOTOR_READY:
				if(mc_core_list.mc_moving)
				{
					hbreak_count=0;
					mc_core_list.mc_moving=0;
				
				}
				else 
				{
					
		
				
				}
				if(mc_core_list.motor_en==0||machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
				Brake_Motor();
				hbreak_count++;
				if(hbreak_count>Motion_Fail_BrakeTime_Count)
				{
					mc_core_list.bldc_bemf_detected=0;
					motor_control_list.bldc_state=MOTOR_POSITION;
						mc_core_list.ipd_star_flag=1;
				}
				
				break;			
				
		case Motor_Align :
			#if 0
			if(mc_core_list.motor_en==0||machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
			{
				if(machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
				else if(motor_control_list.sbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_SBRAKE;
					sbreak_count=0;
				}
				else if(motor_control_list.hbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_HBRAKE;
					hbreak_count=0;
				}
				else 
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
			}
			
			if(mc_core_list.aglin_count<200)
			{
				mc_core_list.aglin_count++;
			}
			else 
			{
				drag_control_init();
				mc_core_list.bldc_step_current++;
				if(mc_core_list.bldc_step_current>=6)
				{
					mc_core_list.bldc_step_current=0;
					
				}
//				mc_core_list.bldc_step_current++;
//				if(mc_core_list.bldc_step_current>=6)
//				{
//					mc_core_list.bldc_step_current=0;
//					
//				}
				TIM_Cmd(TIM14, DISABLE);	
				TIM_ClearFlag(TIM14, TIM_FLAG_Update);
				mc_core_list.bldc_bemf_detected=0;
				mc_core_list.motor_bemf_runing=1;
				motor_control_list.bldc_state=MotorStart_Drag_OPENLOOP;
				mc_core_list.lock_timer_time=12000;
			
			}
			 align_control();
			 #endif
			break;
			
			case MOTOR_POSITION:
//					if(mc_core_list.motor_en==0)
//					{
//							motor_control_list.bldc_state=MOTOR_STOP;
//							mc_core_list.mc_moving=0;	
//					}
				

				if(mc_core_list.ipd_star_flag==0) //等待IPD结束
				{
//					drag_control_init();

					motor_to_bemf_runing_pre();	
					motor_control_list.bldc_state=MOTOR_RUN;
			 }
			break;
		case MotorStart_Drag_OPENLOOP :
			#if 0 
				pwm_set();
			if(mc_core_list.motor_en==0||machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
			{
				if(machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
				else if(motor_control_list.sbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_SBRAKE;
					sbreak_count=0;
				}
				else if(motor_control_list.hbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_HBRAKE;
					hbreak_count=0;
				}
				else 
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
			}
			if(drag_control())
			{
				mc_core_list.motor_bemf_runing=1;
				motor_control_list.bldc_state=MOTOR_RUN;
			
			}
//			if(mc_core_list.bldc_bemf_detected==1)
//			{
//			motor_control_list.bldc_state=MOTOR_RUN;
//			}
			#endif
		break;
			
			

		case MOTOR_RUN :
				pwm_set();
			if(mc_core_list.motor_en==0||machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
			{
				mc_core_list.motor_bemf_runing=0;
				if(machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
				else if(motor_control_list.sbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_SBRAKE;
					sbreak_count=0;
				}
				else if(motor_control_list.hbrake_en)
				{
					motor_control_list.bldc_state=MOTOR_HBRAKE;
					hbreak_count=0;
				}
				else 
				{
					motor_control_list.bldc_state=MOTOR_STOP;
				}
			}
			break;
		case MOTOR_BRAKE2STOP:
			Stop_Motor();
			break2stop_count++;
            if(break2stop_count>80)//10ms
            {
                motor_control_list.bldc_state=MOTOR_STOP;
                if(safety_stop_latched)
                {
                    /* 硬刹车及结束过渡完成，关闭全部输出，但保留重新按键锁定。 */
                    safety_brake_active=0;
                    safety_brake_started=0;
                    motor_safety_poll();
                }
            }
			
			break;
				
		case MOTOR_HBRAKE:
			mc_core_list.pwm_actual_runing_data=0;
			motor_control_list.pwm_duty_virtual=0;
            hbreak_count++;
            if(safety_stop_latched)
            {
                /* 与 ON/OFF 松开共用 HBrake_Time_MS_CNT 和同一个硬刹车状态。 */
                Safety_Brake_Motor(0);
                if(!safety_brake_active)
                {
                    break;
                }
            }
            else
            {
                Brake_Motor();
            }
			if(machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
			{
				motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			}
			else if(mc_core_list.motion_en&&mc_core_list.motor_en)
			{
			
		  	motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			}
			else if(hbreak_count>HBrake_MIN_Time_MS_CNT&&mc_core_list.motor_en)
			{
				motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			
			}
			else if(hbreak_count>HBrake_Time_MS_CNT||machine_error_hold.userErr_hold.bits.gIBusAcmpOCP)
			{
//				if(mc_core_list.motion_en==0)
//				{
//					motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
//				}
//				else 
//				{
					motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
//				}
				break2stop_count=0;
				Stop_Motor();
			
			}
			

//				Stop_Motor();
			
            if(safety_stop_latched && motor_control_list.bldc_state == MOTOR_BRAKE2STOP)
            {
                /* 刹车计时到期立即撤销刹车输出，保持 GPIO 全低直到重新按键。 */
                motor_safety_poll();
            }

		break;
		case MOTOR_SBRAKE:
			#if 1
			if(sbreak_count==0)
			{
				motor_control_list.sbreak_pwm_duty=Sbrake_Duty_Min;
			}
			PwmDutyUpdate(motor_control_list.sbreak_pwm_duty);
			
			motor_control_list.sbreak_pwm_duty+=Sbrake_Duty_Add;
			if(motor_control_list.sbreak_pwm_duty>Sbrake_Duty_Max)
			{
				motor_control_list.sbreak_pwm_duty=Sbrake_Duty_Max;
			}
			UVWL_HOffLPwm();	
			if(machine_error_hold.userErr_hold.word!=0||machine_error_hold.MC_error_hold.word!=0)
			{
				motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			}
			else if(mc_core_list.motion_en&&mc_core_list.motor_en)
			{
			
		  	motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			}
			else if(++sbreak_count>Sbrake_Time_MS||machine_error_hold.userErr_hold.bits.gIBusAcmpOCP)
			{
				motor_control_list.bldc_state=MOTOR_BRAKE2STOP;
				break2stop_count=0;
				Stop_Motor();
			
			}
			
			#endif
//			motor_control_list.bldc_state=MOTOR_STOP;
//			Stop_Motor();
			break;
		default:
			
		 break;
	}

}
