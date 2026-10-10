#include "user_control.h"
#include "parameter.h"
#include "board.h"
#include "motor_control.h"
#include "drv_adc.h"
#include "flash_data_save.h"
#include "motor_config.h"
#include "systick.h"
#include "drv_pwm.h"
u32 PI_sum_i=0;
u32 PI_p=0;
u32	pwm_duty_aim_PI_OUT=0;
User_list_Struct user_list;

Machine_ERROR_HOLD machine_error_hold;
/* 安全锁定独立于原故障位；中断置位，只能经松开后重新按键解除。 */
volatile uint8_t safety_stop_latched = 0;
volatile uint8_t safety_brake_active = 0;
volatile uint8_t motor_ipd_active = 0;
static volatile uint8_t safety_release_count = 0;
static volatile uint8_t safety_restart_ready = 0;

void user_para_init(void)
{
	user_list.userErr.word=0;
	user_list.user_state=TOOL_STOP;
	user_list.flag.bits.gToolDir= Tool_Dir_Init;
	user_list.Mos_overload_count=0;
	user_list.Recover_Mos_overload_count=0;
	machine_error_hold.MC_error_hold.word=0;
	machine_error_hold.userErr_hold.word=0;
	user_list.limit_current_on_flag=0;
	user_list.gears_level_changed=0;
}

//void fly_machine_judge(void)
//{
//		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
//		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
//		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
//		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
//		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
//		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位

//}

void fly_machine_init(void)
{
//		    GPIO_InitTypeDef GPIO_InitStruct;
		u16 i=0;
		u16 ad_sum=0;
		ADC_InitTypeDef  ADC_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    RCC_AHBPeriphClockCmd(ADC_GPIO_CLK, ENABLE);			// OK

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;

    GPIO_InitStructure.GPIO_Pin     = SPEED_PIN;
    GPIO_Init(SPEED_PORT, &GPIO_InitStructure);
	


    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_3;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Scan;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, 0, SPEED_CHANNEL);
		
		//----------------dir---------------
//		ADC_AnyChannelSelect(ADC1, DIR_RANK, DIR_CHANNEL);
		
		
    ADC_AnyChannelCmd(ADC1, ENABLE);
		ADC_Cmd(ADC1, ENABLE);
		systick_delay_us(300);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
	
	
		for(i=0;i<4;i++)
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);								// 软件触发ADC转换, 获取峰值电流采样值
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);		// 等待ADC转换完成
			ADC_ClearFlag(ADC1, ADC_FLAG_EOC);										// 清除ADC转换完成标志位
	
					ad_sum += (uint16_t)GET_ADC_VALUE(SPEED_CHANNEL);
		}
		ad_sum=(ad_sum>>2);
		if(ad_sum<983)
		{
			machine_error_hold.userErr_hold.bits.fly_machine=1;
		
		}
		
		#if 0


    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_7);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   =GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
	#endif		
		
	
}
void user_oc_handle(void)
{
		static u16 IBusAvgOCP1_count=0;
	static u16 IBusAvgOCP2_count=0;
	static u16 IBusAvgOCP3_count=0;
	static u16 IBusPeakOCP1_count=0;
	#if LIMIT_PROTECT_EN
	static u16 IBus_Limit_max_count=0;
	#endif
	static u16 IBusAvg_rec_count=0;
	u32 IBus_Avg_OCP1_AdcValue_temp=0;
	u32 IBus_Avg_OCP2_AdcValue_temp=0;
	u32 IBus_Avg_OCP3_AdcValue_temp=0;
	u16  ibus_temp=0;
	   
			if(user_list.gears_level==0)
			{
				IBus_Avg_OCP1_AdcValue_temp=(IBus_Avg_OCP1_AdcValue*PWM_DUTY_GEARS1)>>15;
				IBus_Avg_OCP2_AdcValue_temp=(IBus_Avg_OCP2_AdcValue*PWM_DUTY_GEARS1)>>15;
				IBus_Avg_OCP3_AdcValue_temp=(IBus_Avg_OCP3_AdcValue*PWM_DUTY_GEARS1)>>15;
			}
			else if(user_list.gears_level==1)
			{
				IBus_Avg_OCP1_AdcValue_temp=(IBus_Avg_OCP1_AdcValue*PWM_DUTY_GEARS2)>>15;
				IBus_Avg_OCP2_AdcValue_temp=(IBus_Avg_OCP2_AdcValue*PWM_DUTY_GEARS2)>>15;
				IBus_Avg_OCP3_AdcValue_temp=(IBus_Avg_OCP3_AdcValue*PWM_DUTY_GEARS2)>>15;
			}
			else 
			{
			
				IBus_Avg_OCP1_AdcValue_temp=(IBus_Avg_OCP1_AdcValue*PWM_DUTY_GEARS3)>>15;
				IBus_Avg_OCP2_AdcValue_temp=(IBus_Avg_OCP2_AdcValue*PWM_DUTY_GEARS3)>>15;
				IBus_Avg_OCP3_AdcValue_temp=(IBus_Avg_OCP3_AdcValue*PWM_DUTY_GEARS3)>>15;
			}
			#if LIMIT_PROTECT_EN
		if((mc_core_list.pwm_actual_runing_data+5)<motor_control_list.pwm_duty_virtual&&motor_control_list.bldc_state==MOTOR_RUN)
		{
				if(user_list.userErr.bits.gIBusAvgLimit_Protect==0)
				{
					IBus_Limit_max_count++;
					if(IBus_Limit_max_count>LIMIT_AVG_CURRENT_PROTECT_MS_CONUT&&user_list.limit_current_on_flag)
					{
						IBus_Limit_max_count=200;
						user_list.userErr.bits.gIBusAvgLimit_Protect=1;
						
					}
				}
			
		}
		else 
		{
			
			if(motor_control_list.bldc_state!=MOTOR_RUN)
			{
				IBus_Limit_max_count=0;
				}
			if(IBus_Limit_max_count)
			{
				IBus_Limit_max_count--;
			}
			else 
			{
				user_list.userErr.bits.gIBusAvgLimit_Protect=0;
			}
		
		}
		#endif
		if(mc_core_list.Board_ADC_DATA.u16I_Peak>mc_core_list.CurrentOffset_ad_data)
		{
			ibus_temp=mc_core_list.Board_ADC_DATA.u16I_Peak-mc_core_list.CurrentOffset_ad_data;
		}
		else 
		{
			ibus_temp=0;
		
		}
		user_list.IBusAvg1_Adc=(u16)((float)mc_core_list.pwm_actual_runing_data *ibus_temp/PWM_PRIOD_LOAD);	
		user_list.IBusAvg_Adc=(user_list.IBusAvg1_Adc>>3)+user_list.IBusAvg_Adc-(user_list.IBusAvg_Adc>>3);
		//------------------ocp1
		if(user_list.IBusAvg_Adc>IBus_Avg_OCP1_AdcValue_temp)
		{
				
			IBusAvgOCP1_count++;
			if(IBusAvgOCP1_count>IBus_Avg_OCP1_MS_CONUT)
			{
				IBusAvgOCP1_count=IBus_Avg_OCP1_MS_CONUT;
				user_list.userErr.bits.gIBusAvgOCP1=1;
				IBusAvg_rec_count=0;
			}
		}
		else 
		{
			if(IBusAvgOCP1_count>0)
			{
				IBusAvgOCP1_count--;
			}
			else
			{
				user_list.userErr.bits.gIBusAvgOCP1=0;
			
			}
		}
			////------------------ocp2
			if(user_list.IBusAvg_Adc>IBus_Avg_OCP2_AdcValue_temp)
		{
				
			IBusAvgOCP2_count++;
			if(IBusAvgOCP2_count>IBus_Avg_OCP2_MS_CONUT)
			{
				IBusAvgOCP2_count=IBus_Avg_OCP2_MS_CONUT;
				user_list.userErr.bits.gIBusAvgOCP2=1;
				IBusAvg_rec_count=0;
			}
		}
		else 
		{
			if(IBusAvgOCP2_count>0)
			{
				IBusAvgOCP2_count--;
			}
			else
			{
				user_list.userErr.bits.gIBusAvgOCP2=0;
			
			}
			
		}
		
			//------------------ocp3
			if(user_list.IBusAvg_Adc>IBus_Avg_OCP3_AdcValue_temp)
		{
				
			IBusAvgOCP3_count++;
			if(IBusAvgOCP3_count>IBus_Avg_OCP3_MS_CONUT)
			{
				IBusAvgOCP3_count=IBus_Avg_OCP3_MS_CONUT;
				user_list.userErr.bits.gIBusAvgOCP3=1;
				IBusAvg_rec_count=0;
			}
		}
		else 
		{
			if(IBusAvgOCP3_count>0)
			{
				IBusAvgOCP3_count--;
			}
			else
			{
				user_list.userErr.bits.gIBusAvgOCP3=0;
			
			}
			
		}	
		if(user_list.IBusAvg_Adc<IBus_Avg_OCP1_AdcValue&&motor_control_list.bldc_state==MOTOR_STOP)
		{
			IBusAvg_rec_count++;
			if(IBusAvg_rec_count>IBus_Avg_REC_MS_CONUT)
			{
				IBusAvg_rec_count=0;
				user_list.userErr.bits.gIBusAvgOCP3=0;
				user_list.userErr.bits.gIBusAvgOCP2=0;
				user_list.userErr.bits.gIBusAvgOCP1=0;
				IBusAvgOCP3_count=0;
				IBusAvgOCP2_count=0;
				IBusAvgOCP1_count=0;
			}
			
		}

#if Multistage_Peak_OCP_EN
		
			  if(ibus_temp > IBus_Smart_Peak_OCP3_AdcValue)
        {

						IBusPeakOCP1_count+=IBus_Smart_OCP3_MS_Operation_Value;
            if(IBusPeakOCP1_count >= 32768 )
            {
               IBusPeakOCP1_count= 32768;
							//mos过载处理
							if(user_list.userErr.bits.gIBusPeakOCP1==0)
							{
								user_list.Mos_overload_count++;
							
							}
							user_list.userErr.bits.gIBusPeakOCP1 = 1; 
            }
        }
		    else  if(ibus_temp > IBus_Smart_Peak_OCP2_AdcValue)
        {

						IBusPeakOCP1_count+=IBus_Smart_OCP2_MS_Operation_Value;
            if(IBusPeakOCP1_count >= 32768 )
            {
               IBusPeakOCP1_count= 32768;
							//mos过载处理
							if(user_list.userErr.bits.gIBusPeakOCP1==0)
							{
								user_list.Mos_overload_count++;
							
							}
							user_list.userErr.bits.gIBusPeakOCP1 = 1; 
            }
        }
				else if(ibus_temp > IBus_Smart_Peak_OCP1_AdcValue)
				{
				
						IBusPeakOCP1_count+=IBus_Smart_OCP1_MS_Operation_Value;
            if(IBusPeakOCP1_count >= 32768 )
            {
               IBusPeakOCP1_count= 32768;
							//mos过载处理
							if(user_list.userErr.bits.gIBusPeakOCP1==0)
							{
								user_list.Mos_overload_count++;
							
							}
							user_list.userErr.bits.gIBusPeakOCP1 = 1; 
            }
				
				
				}
        else  
        {
					
							if(motor_control_list.bldc_state==MOTOR_STOP)
						{
								if(IBusPeakOCP1_count>IBus_Smart_REC_MS_Operation_Value)
							{
									IBusPeakOCP1_count-=IBus_Smart_REC_MS_Operation_Value;
							}
							else
							{
								IBusPeakOCP1_count=0;
									user_list.userErr.bits.gIBusPeakOCP1 = 0;
							}
						}
						else
						{
					
							if(IBusPeakOCP1_count>IBus_Smart_Peak_OCP1_AdcValue)
							{
									IBusPeakOCP1_count-=IBus_Smart_Peak_OCP1_AdcValue;
							}
							else
							{
								IBusPeakOCP1_count=0;
									user_list.userErr.bits.gIBusPeakOCP1 = 0;
							}
						}
//            if(IBusPeakOCP1_count>IBus_Smart_OCP1_MS_Operation_Value)
//            {
//                IBusPeakOCP1_count-=IBus_Smart_OCP1_MS_Operation_Value;
//            }
//            else
//            {
//								user_list.userErr.bits.gIBusPeakOCP1 = 0;
//            }
        }
		
#else
        if(ibus_temp > IBus_Peak_OCP1_AdcValue)
        {
            if(IBusPeakOCP1_count++ >= IBus_Peak_OCP1_MS_CONUT )
            {
               IBusPeakOCP1_count= IBus_Peak_OCP1_MS_CONUT;

							user_list.userErr.bits.gIBusPeakOCP1 = 1; 
            }
        }
        else
        {
            if(IBusPeakOCP1_count)
            {
                IBusPeakOCP1_count--;
            }
            else
            {
								user_list.userErr.bits.gIBusPeakOCP1 = 0;
            }
        }
#endif				
}

void Mos_overload_handle(void)
{
	#if Mos_overload_EN
		static u16 Mos_overload_count_image=0;
		if(user_list.Mos_overload_count>=Mos_overload_count_Set)
		{
		
			user_list.userErr.bits.gMos_overload=1;
		}
		//-----3s没有出现过载现象，user_list.Mos_overload_count自动减一
		if(Mos_overload_count_image!=user_list.Mos_overload_count)
		{
			Mos_overload_count_image=user_list.Mos_overload_count;
			user_list.Recover_Mos_overload_count1=0;
		}
		user_list.Recover_Mos_overload_count1++;
//		if(motor_control_list.bldc_state==MOTOR_STOP)
//		{
//			user_list.Recover_Mos_overload_count1++;//加速恢复
//		}
		if(user_list.Recover_Mos_overload_count1>1500)
		{
			user_list.Recover_Mos_overload_count1=0;
			if(user_list.Mos_overload_count>1)
			{
			
				user_list.Mos_overload_count=user_list.Mos_overload_count-1;
//				user_list.Mos_overload_count--;
			}
			else
			{
			user_list.Mos_overload_count=0;
			}
		
		}
		//--------------------------------------------------
		if(motor_control_list.bldc_state==MOTOR_STOP)
		{
		
			user_list.Recover_Mos_overload_count++;
			if(user_list.Recover_Mos_overload_count>Recover_Mos_overload_time_count)
			{
				user_list.Mos_overload_count=0;
				user_list.userErr.bits.gMos_overload=0;
			}
		}
		else 
		{
				user_list.Recover_Mos_overload_count=0;
		}
	#endif

}
void Volt_Handler(void)
{


	static u16 VBusStbUVP_Cnt=VBus_Stb_UVP_FiltMS_COUNT-1;//上电只需检测一次
	static u16 VBusRunUVP1_Cnt=0;
	static u16 VBusRunUVP2_Cnt=0;
	static u16 	VBusOVP_Cnt=VBus_OVP_FiltMS_COUNT-1;
    #if EN_VOLATAGE_PROTECT    
				user_list.VBus_Vx100=(mc_core_list.Board_ADC_DATA.u16Vbus*VBUS_BASE_X100)>>12;
        if((mc_core_list.Board_ADC_DATA.u16Vbus < VBus_Stb_UVP_ADC)&&(user_list.user_state != TOOL_RUN)) // 0 可以改为电机停止
        {
            if(VBusStbUVP_Cnt++ >= VBus_Stb_UVP_FiltMS_COUNT)
            {
                VBusStbUVP_Cnt= VBus_Stb_UVP_FiltMS_COUNT;

								user_list.userErr.bits.gVBusStbUVP = 1; //静态欠压保护
            }
        }
        else
        {
            if(VBusStbUVP_Cnt)
            {
                VBusStbUVP_Cnt--;
            }
            else
            {
			      	user_list.userErr.bits.gVBusStbUVP = 0;
            }
        }


        if(mc_core_list.Board_ADC_DATA.u16Vbus < VBus_Run_UVP1_ADC)
        {
            if(VBusRunUVP1_Cnt++ >= VBus_Run_UVP1_FiltMS_COUNT)
            {
                VBusRunUVP1_Cnt= VBus_Run_UVP1_FiltMS_COUNT;
     
                user_list.userErr.bits.gVBusRunUVP1 = 1;
                
            }
        }
        else
        {
            if(VBusRunUVP1_Cnt)
            {
                VBusRunUVP1_Cnt--;
            }
			else
            {
							user_list.userErr.bits.gVBusRunUVP1 = 0;
            }
        }
        
       
        if(mc_core_list.Board_ADC_DATA.u16Vbus < VBus_Run_UVP2_ADC)
        {
            if(VBusRunUVP2_Cnt++ >=VBus_Run_UVP2_FiltMS_COUNT)
            {
                VBusRunUVP2_Cnt= VBus_Run_UVP2_FiltMS_COUNT;
  
                user_list.userErr.bits.gVBusRunUVP2 = 1;
            }
        }
        else
        {
            if(VBusRunUVP2_Cnt)
            {
                VBusRunUVP2_Cnt--;
            }
						else
            {
					  	user_list.userErr.bits.gVBusRunUVP2 = 0;
            }
        }



        if(mc_core_list.Board_ADC_DATA.u16Vbus > VBus_OVP_ADC)
        {
            if(VBusOVP_Cnt++ >= VBus_OVP_FiltMS_COUNT)
            {
                VBusOVP_Cnt= VBus_OVP_FiltMS_COUNT;

                user_list.userErr.bits.gVBusOVP = 1;
            }
        } 
        else
        {
            if(VBusOVP_Cnt)
            {
                VBusOVP_Cnt--;
            }
						else
            {
							user_list.userErr.bits.gVBusOVP = 0;
            }
        }


		#endif
}



void User_Temperature_Handler(void)
{

		static u16 NtcMosErrClr_Cnt=0;
		static u16 NtcMosErr_Cnt=0;
    #if EN_MOS_TEMP_DETECT
  
        if(user_list.userErr.bits.gNtcMos == 1)
        {
            if(mc_core_list.Board_ADC_DATA.u16NTC_MOS> RSM_MOS_TEMP_OVER_THD_ADC)//        
            {
                if(NtcMosErrClr_Cnt++ >= RSM_MOS_TEMP_OVER_TIME)
                {
                    NtcMosErrClr_Cnt= RSM_MOS_TEMP_OVER_TIME;
                    user_list.userErr.bits.gNtcMos=0;
                }
            }
            else
            {
                if(NtcMosErrClr_Cnt)
                {
                    NtcMosErrClr_Cnt--;
                }

            }
            NtcMosErr_Cnt= 0;
        }
        else
        {
           NtcMosErrClr_Cnt= 0;
            if(
            ((mc_core_list.Board_ADC_DATA.u16NTC_MOS < MOS_TEMP_OVER_THD_ADC)&&(mc_core_list.Board_ADC_DATA.u16NTC_MOS>100))//按键复用
						//(strInput.NtcMos_Adc < strInput.NtcMosOTP_Adc)
            //||(strInput.NtcMos_Adc > strInput.NtcMosUTP_Adc)
            )
            {
                if(NtcMosErr_Cnt++ >= MOS_TEMP_OVER_TIME)
                {
                  NtcMosErr_Cnt= MOS_TEMP_OVER_TIME;
                    
									user_list.userErr.bits.gNtcMos=1;
                }
            }
            else
            {
                if(NtcMosErr_Cnt)
                {
                   NtcMosErr_Cnt--;
                }
            }
                    
            
        }
#endif


}


void user_direction_handle(void)
{
	
	static u8 dir_cw_count=0;
	static u8 dir_ccw_count=0;
	#if(1)
	if(GPIO_ReadInputDataBit(DIR_PORT,DIR_PIN)) 
	{
			dir_ccw_count=0;
			if(++dir_cw_count>10)
			{
					dir_cw_count=10;
				if(motor_control_list.bldc_state==MOTOR_STOP||motor_control_list.bldc_state==MOTOR_HBRAKE||motor_control_list.bldc_state==MOTOR_SBRAKE||motor_control_list.bldc_state==MOTOR_BRAKE2STOP)
				{
					user_list.flag.bits.gToolDir=Tool_Dir_CCW_Value;
					mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
				}
		
			}
			
			

	}
	else 
	{
			dir_cw_count=0;
			if(++dir_ccw_count>10)
			{
			
//				user_list.flag.bits.gToolDir=Tool_Dir_CW_Value;
//				mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
//				dir_ccw_count=10;
					dir_ccw_count=10;
				if(motor_control_list.bldc_state==MOTOR_STOP||motor_control_list.bldc_state==MOTOR_HBRAKE||motor_control_list.bldc_state==MOTOR_SBRAKE||motor_control_list.bldc_state==MOTOR_BRAKE2STOP)
				{
					user_list.flag.bits.gToolDir=Tool_Dir_CW_Value;
					mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
				
				}
			}
	
	}
	#endif
	
	#if(0)
	user_list.dir_adc=(uint16_t)GET_ADC_VALUE(DIR_CHANNEL);
//	mc_core_list.mc_dir=1;
		if(user_list.dir_adc>3600) 
	{
			dir_ccw_count=0;
			if(++dir_cw_count>10)
			{
				dir_cw_count=10;
				if(motor_control_list.bldc_state==MOTOR_STOP||motor_control_list.bldc_state==MOTOR_HBRAKE||motor_control_list.bldc_state==MOTOR_SBRAKE||motor_control_list.bldc_state==MOTOR_BRAKE2STOP)
				{
					user_list.flag.bits.gToolDir=Tool_Dir_CCW_Value;
					mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
				}
			}
			
			

	}
	else if(user_list.dir_adc<3600&&user_list.dir_adc>2000)
	{
			dir_cw_count=0;
			if(++dir_ccw_count>10)
			{
				dir_ccw_count=10;
				if(motor_control_list.bldc_state==MOTOR_STOP||motor_control_list.bldc_state==MOTOR_HBRAKE||motor_control_list.bldc_state==MOTOR_SBRAKE||motor_control_list.bldc_state==MOTOR_BRAKE2STOP)
				{
					user_list.flag.bits.gToolDir=Tool_Dir_CW_Value;
					mc_core_list.mc_dir=user_list.flag.bits.gToolDir;
				
				}
				
			}
	
	}
	#endif
}
u8 key_scan(void)
{
	static u8 key_count=0;
	u8 res=0;
	user_list.key_adc=(uint16_t)GET_ADC_VALUE(TMOS_CHANNEL);
	if(user_list.key_adc<100)
	{ 
		if(key_count<200)
		{
			key_count++;
		}
		if(key_count==50)
		{
			res=1;
		}
		
	}
	else 
	{
		key_count=0;
	
	}
	return res;


}

//void gears_deal(void)
//{
//	if(key_scan())
//	{
//		user_list.gears_level++;
//		if(user_list.gears_level>1)
//		{
//		
//			user_list.gears_level=0;
//		}
//	}

//}
	
void pwm_duty_aim_deal(void)
{
	

//		if(mc_core_list.Board_ADC_DATA.u16VR<MIN_TRG_ADC)
//		{
//		
//			mc_core_list.Board_ADC_DATA.u16VR=MIN_TRG_ADC;
//		}
//	  motor_control_list.pwm_duty_aim_per=(uint16_t)((float)(mc_core_list.Board_ADC_DATA.u16VR - MIN_TRG_ADC)*K_ADC_DUTY+ MIN_TOOL_DUTY);
		motor_control_list.pwm_duty_aim_per=PWM_PRIOD_LOAD;
	#if(0)
		motor_control_list.pwm_duty_aim=motor_control_list.pwm_duty_aim_per;
		
	#endif


}



void user_gears_handle(void)
{
	#if 0
	u8 res=0;
	res=key_scan();
	if(res)
	{
		if(user_list.gears_level<2)
		{
			user_list.gears_level++;
			
		}
		else
		{
			user_list.gears_level=0;
		}
		user_list.gears_level_changed=1;
	}
	
	if(user_list.gears_level==0)
	{
		motor_control_list.pwm_duty_aim=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS1)>>15;
	}
	else if(user_list.gears_level==1)
	{
		motor_control_list.pwm_duty_aim=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS2)>>15;
	}
	else 
	{
	
		motor_control_list.pwm_duty_aim=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS3)>>15;
	}
	
		if(user_list.gears_level_changed&&user_list.user_state!=TOOL_RUN)
	{
		user_list.gears_level_changed=0;
		save_user_data();
	}
	#else 
	motor_control_list.pwm_duty_aim=motor_control_list.pwm_duty_aim_per;
	#endif


}
void user_WrenchKey_Handler(void)
{
//	gears_deal();//扫描档位
	u32	pwm_duty_aim_temp=0;

	static u16 motor_run_count=0;

	u8 res=0;
	
		if(user_list.user_state==TOOL_RUN)
		{
			if(motor_run_count<5000)
			{
				motor_run_count++;
			}
			else 
			{
				motor_run_count=5000;
			}
		}
		else 
		{
			motor_run_count=0;
		}
	res=key_scan();
	if(motor_control_list.bldc_state!=MOTOR_RUN&&motor_control_list.bldc_state!=MOTOR_MOTION&&motor_control_list.bldc_state!=MOTOR_POSITION)
	{
			if(user_list.flag.bits.gToolDir==Tool_Dir_CW_Value)
			{
				if(res)
				{
					user_list.gears_level++;
					if(user_list.gears_level>1)
					{
					
						user_list.gears_level=0;
					}
					save_user_data();
				}
			}
			else 
			{
				if(res)
				{
						user_list.flag.bits.gAutoStopEn=!user_list.flag.bits.gAutoStopEn;
					save_user_data();
				}
					
			
			}
	}
	
	if(user_list.flag.bits.gToolDir==Tool_Dir_CCW_Value)
	{
			if(user_list.flag.bits.gAutoStopEn)
			{
//				motor_control_list.pwm_duty_aim=PWM_PRIOD_LOAD;
				pwm_duty_aim_temp=PWM_PRIOD_LOAD;
			}
			else 
			{
			//	motor_control_list.pwm_duty_aim=motor_control_list.pwm_duty_aim_per;
				pwm_duty_aim_temp=motor_control_list.pwm_duty_aim_per;
			}
	}
	else 
	{
	
	
		if(user_list.gears_level==0)
		{
		//	motor_control_list.pwm_duty_aim=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS1)>>15;
			pwm_duty_aim_temp=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS1)>>15;
		}
		else if(user_list.gears_level==1)
		{
		//	motor_control_list.pwm_duty_aim=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS3)>>15;
			pwm_duty_aim_temp=(motor_control_list.pwm_duty_aim_per*PWM_DUTY_GEARS3)>>15;
		}
	}
	#if(1)
	if(motor_run_count<180)
	
	{
		pwm_duty_aim_PI_OUT=(0.65f*PWM_PRIOD_LOAD);
	
	}
	else 
	{
		if(mc_core_list.time_count_60_degree>((u32)(1000*MOTOR_TIMR_FRQ_MHZ)))
		{
			PI_p=mc_core_list.time_count_60_degree-((u32)(1000*MOTOR_TIMR_FRQ_MHZ));
			PI_p=(PI_p>>2);
			if(PI_sum_i<PWM_PRIOD_LOAD)
			{
				PI_sum_i++;
				
			}
			pwm_duty_aim_PI_OUT=PI_sum_i+PI_p;
		}
		
		else 
		{
			PI_p=((u32)(1000*MOTOR_TIMR_FRQ_MHZ))-mc_core_list.time_count_60_degree;
			PI_p=(PI_p>>2);
			if(PI_sum_i>LIMIT_MIN_PWN_DUTY)
			{
				PI_sum_i--;
			}
			if(PI_sum_i>PI_p)
			{
				pwm_duty_aim_PI_OUT=PI_sum_i-PI_p;
			}
			else 
			{
				pwm_duty_aim_PI_OUT=LIMIT_MIN_PWN_DUTY;
			
			}
		}
		

	}
	
		if(pwm_duty_aim_PI_OUT>LIMIT_MAX_PWN_DUTY)
		{
			pwm_duty_aim_PI_OUT=LIMIT_MAX_PWN_DUTY;
		
		}
			if(pwm_duty_aim_PI_OUT>pwm_duty_aim_temp)
		{
			motor_control_list.pwm_duty_aim=pwm_duty_aim_PI_OUT;
		}
		else 
		{
		
			motor_control_list.pwm_duty_aim=pwm_duty_aim_temp;
		}
	#else
	
	motor_control_list.pwm_duty_aim=pwm_duty_aim_temp;
	#endif
	
	if(user_list.flag.bits.gAutoStopEn&&user_list.flag.bits.gToolDir==Tool_Dir_CCW_Value&&user_list.user_state==TOOL_RUN)
	{
				if(user_list.CCW_Auto_Mask_count<63334)
				{
					user_list.CCW_Auto_Mask_count++;
				}
				if(user_list.IBusAvg_Adc < CCW_AUTO_STOP_CURRENT_AdcValue&&user_list.CCW_Auto_Mask_count>CCW_AUTO_STOP_START_MASK_COUNT)
				{						
						if(mc_core_list.Run_SectorCnt > AUTO_STOP_Sector_CNT) //
						{
								user_list.flag.bits.gAutoStop = 1;

								mc_core_list.Run_SectorCnt = 0;	
						}		
				}
				else
				{
								mc_core_list.Run_SectorCnt = 0;
				}
				
	}
	else 
	{
			mc_core_list.Run_SectorCnt = 0;	
			user_list.CCW_Auto_Mask_count=0;
	}
	

}
/* 安全开关触发后禁止驱动运行，但允许与 ON/OFF 松开相同的硬刹车停转。 */
uint8_t motor_safety_poll(void)
{
    uint32_t primask;
    uint8_t first_trip = 0;
    uint8_t pressed = SAFETY_PRESSED();
    if(!pressed && !safety_stop_latched)
    {
        return 0;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    if(pressed)
    {
        if(!safety_stop_latched)
        {
            first_trip = 1;
            /* 只对正在运行或停机过程中的电机发起一次刹车；上电锁定不通电。 */
            safety_brake_active = (motor_control_list.hbrake_en &&
                                   motor_control_list.bldc_state != MOTOR_STOP);
        }
        safety_stop_latched = 1;
        safety_restart_ready = 0;
        safety_release_count = 0;
    }
    if(safety_stop_latched)
    {
        /* 定位和换相仍全部禁止，motor_en 保持为零，不能借刹车重新启动。 */
        TIM_ITConfig(TIM14, TIM_IT_Update, DISABLE);
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
        mc_core_list.motor_en = 0;
        mc_core_list.ipd_star_flag = 0;
        mc_core_list.motor_bemf_runing = 0;
        mc_core_list.mc_moving = 0;
        mc_core_list.bldc_bemf_detected = 0;
        mc_core_list.pwm_actual_runing_data = 0;
        motor_control_list.pwm_duty_virtual = 0;

        /* 原故障优先关闭桥臂，禁止为了刹车重新开放过流等保护关闭的输出。 */
        if(user_list.userErr.word != 0 || mc_core_list.MC_error.word != 0 ||
           machine_error_hold.userErr_hold.word != 0 ||
           machine_error_hold.MC_error_hold.word != 0)
        {
            safety_brake_active = 0;
        }
        if(safety_brake_active)
        {
            /* 已进入刹车结束过渡时不重置状态，长按安全开关也不重复计时。 */
            if(motor_control_list.bldc_state != MOTOR_BRAKE2STOP)
            {
                motor_control_list.bldc_state = MOTOR_HBRAKE;
            }
        }
        else
        {
            motor_control_list.bldc_state = MOTOR_STOP;
        }

        /* 首次触发先撤销运行输出；定位未返回、刹车结束或故障时保持六路全低。 */
        if(first_trip || !safety_brake_active || motor_ipd_active ||
           motor_control_list.bldc_state == MOTOR_BRAKE2STOP)
        {
            TIM_CtrlPWMOutputs(TIM1, DISABLE);
            Stop_Motor();
            Bsp_Pwm_Low();
        }
    }
    __set_PRIMASK(primask);
    return safety_stop_latched;
}

void motor_on_off_control(void)
{
    static uint8_t trigger_off_count = 0;
    static uint8_t trigger_on_count = 0;
    uint8_t pressed;
    uint8_t released;
    uint32_t primask = __get_PRIMASK();

    /* 按键消抖和解锁与中断内安全锁定互斥，避免锁定状态被旧计数覆盖。 */
    __disable_irq();
    motor_safety_poll();
#if ON_OFF_DIGITAL_EN
    /* 最新接线：PB0 高电平表示按下，低电平表示松开。 */
    pressed = ON_OFF_PRESSED();
    released = !pressed;
#else
    /* ADC 方式仍为大于 600 开、小于 550 关，保持原滞回阈值。 */
    pressed = (user_list.on_start_adc > TRG_ON_ADC);
    released = (user_list.on_start_adc < TRG_OFF_ADC);
#endif
    /* 连续稳定约 10ms 才改变按键状态，计数饱和避免长按回卷。 */
    if(released)
    {
        trigger_on_count = 0;
        if(trigger_off_count < ON_OFF_DEBOUNCE_MS)
        {
            trigger_off_count++;
        }
        if(trigger_off_count >= ON_OFF_DEBOUNCE_MS)
        {
            user_list.flag.bits.gToolTrigger = 0;
        }
    }
    else if(pressed)
    {
        trigger_off_count = 0;
        if(trigger_on_count < ON_OFF_DEBOUNCE_MS)
        {
            trigger_on_count++;
        }
        if(trigger_on_count >= ON_OFF_DEBOUNCE_MS)
        {
            user_list.flag.bits.gToolTrigger = 1;
        }
    }
    else
    {
        /* ADC 滞回区保持按键状态，但连续消抖计数重新开始。 */
        trigger_off_count = 0;
        trigger_on_count = 0;
    }

    if(safety_stop_latched)
    {
        /* 安全已松开且 ON/OFF 已确认松开，才接受随后重新按下。 */
        if(!SAFETY_PRESSED() && released)
        {
            if(safety_release_count < ON_OFF_DEBOUNCE_MS)
            {
                safety_release_count++;
            }
            if(safety_release_count >= ON_OFF_DEBOUNCE_MS)
            {
                safety_restart_ready = 1;
            }
        }
        else
        {
            safety_release_count = 0;
        }
        if(safety_restart_ready && pressed &&
           trigger_on_count >= ON_OFF_DEBOUNCE_MS)
        {
            /* 刹车未结束或其他故障未解除时，本次按下无效，须再次松开后重新按下。 */
            safety_restart_ready = 0;
            if(!SAFETY_PRESSED() && !safety_brake_active && !motor_ipd_active &&
               motor_control_list.bldc_state == MOTOR_STOP &&
               user_list.userErr.word == 0 &&
               mc_core_list.MC_error.word == 0 &&
               machine_error_hold.userErr_hold.word == 0 &&
               machine_error_hold.MC_error_hold.word == 0)
            {
                /* 先确保桥臂关闭，再恢复引脚复用，随后由原状态机重新启动。 */
                TIM_CtrlPWMOutputs(TIM1, DISABLE);
                Stop_Motor();
                safety_stop_latched = 0;
                Bsp_Pwm_Init();
                PwmOutputsEnable(); /* 输出恢复也不能绕过安全停机锁定。 */
            }
        }
        motor_safety_poll();
    }
    __set_PRIMASK(primask);
}

void error_hold_updata(void)
{
	machine_error_hold.MC_error_hold.word|=mc_core_list.MC_error.word;
	machine_error_hold.userErr_hold.word|=user_list.userErr.word;

}

void user_state_control(void)
{
    /* 安全锁定优先撤销工具使能，保留原故障检测及松手恢复逻辑。 */
    if(safety_stop_latched)
    {
        user_list.flag.bits.gToolEn = 0;
        if(user_list.user_state == TOOL_RUN)
        {
            user_list.user_state = TOOL_STOP;
        }
    }
				switch(user_list.user_state)
				{
					case TOOL_STOP:
						user_list.flag.bits.gToolEn=0;
						if(++user_list.sleep_cnt>SLEEP_TIME)
						{
							user_list.user_state=TOOL_POWER_DOWN;
							user_list.sleep_cnt=0;
						}
						if(machine_error_hold.MC_error_hold.word!=0||machine_error_hold.userErr_hold.word!=0)
						{
							user_list.user_state=TOOL_ERROR_STOP;
						}
					  if(user_list.flag.bits.gAutoStop == 1)
						{
									if(user_list.flag.bits.gToolTrigger==0)
									{
										user_list.flag.bits.gAutoStop =0;
									}
						}
						/* 启动同时受安全锁定、实时故障及锁存故障约束。 */
                        else if(user_list.flag.bits.gToolTrigger && !safety_stop_latched &&
                                machine_error_hold.MC_error_hold.word == 0 &&
                                machine_error_hold.userErr_hold.word == 0 &&
                                mc_core_list.MC_error.word == 0 && user_list.userErr.word == 0)
						{
						
								user_list.flag.bits.gToolEn=1;
								user_list.user_state=TOOL_RUN;
								user_list.sleep_cnt=0;
						}
						
						
					break;
						case TOOL_ERROR_STOP:
							user_list.flag.bits.gToolEn=0;
						
						if(++user_list.sleep_cnt>SLEEP_TIME)
						{
							user_list.user_state=TOOL_POWER_DOWN;
							user_list.sleep_cnt=0;
						}
						if(user_list.flag.bits.gToolTrigger==0)
						{
								
								if(user_list.userErr.word==0)
								{
									
									if(	machine_error_hold.userErr_hold.bits.gIBusAcmpOCP==1&&motor_control_list.bldc_state==MOTOR_STOP&&(GPIO_ReadInputDataBit(EN_PORT,EN_PIN)==1))
									{
									
										machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=0;
										PwmOutputsEnable(); /* 输出恢复也不能绕过安全停机锁定。 */
									}
//									mc_core_list.MC_error.word=0;
//									machine_error_hold.MC_error_hold.word=0;
									mc_core_list.MC_error.word&=(0x00000004);//堵转保护不直接清
									machine_error_hold.MC_error_hold.word&=(0x00000004);
									
									if(mc_core_list.MC_error.bits.gBlock==0) //mc_core_list.MC_error.bits.gBlock为0后清除
									{
										machine_error_hold.MC_error_hold.bits.gBlock=0;
									}						
									
									
									machine_error_hold.userErr_hold.word&=0x00000001;
									
								
								}
								if(	machine_error_hold.MC_error_hold.word==0&&machine_error_hold.userErr_hold.word==0)
								{
									user_list.user_state=TOOL_STOP;
								}
//							user_list.userErr.word=0;
							
							
						}
						else
						{
							user_list.sleep_cnt=0;
						}
							break;
					case TOOL_RUN:
						if(machine_error_hold.MC_error_hold.word!=0||machine_error_hold.userErr_hold.word!=0)
						{
							user_list.user_state=TOOL_ERROR_STOP;
						
						}
						if(user_list.flag.bits.gToolTrigger==0)
						{
						
							user_list.user_state=TOOL_STOP;
						}
						
						   if(user_list.flag.bits.gAutoStop == 1)
            {
                user_list.flag.bits.gToolEn = 0;   /**/
//                user_list.ToolState_Cnt = 0;
                
                user_list.user_state = TOOL_STOP;    
            }
            
					
						break;
						case TOOL_POWER_DOWN:
							user_list.sleep_cnt=0;
							//GPIO_ResetBits(EN_PORT, EN_PIN);
							EN_Port_Out_L();
						
							if(	machine_error_hold.userErr_hold.bits.gIBusAcmpOCP==1&&motor_control_list.bldc_state==MOTOR_STOP)
							{
									
									machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=0;
									PwmOutputsEnable(); /* 输出恢复也不能绕过安全停机锁定。 */
							}
						
							/* 安全锁定时不得通过 ON/OFF 唤醒路径重新启动。 */
                            if(user_list.flag.bits.gToolTrigger && !safety_stop_latched)
							{
									if(machine_error_hold.MC_error_hold.word!=0||machine_error_hold.userErr_hold.word!=0)
									{
									
										user_list.user_state = TOOL_ERROR_STOP; 
									}
									else 
									{
										user_list.user_state = TOOL_STOP;  
									}
							//		GPIO_SetBits(EN_PORT, EN_PIN);
									EN_Port_in();
//								  TIM_ClearFlag(TIM1, TIM_FLAG_Break);
										if((TIM1->BDTR & (TIM_BDTR_MOE))==0)
									{
										machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=1;
									}
								if(	machine_error_hold.userErr_hold.bits.gIBusAcmpOCP==1&&motor_control_list.bldc_state==MOTOR_STOP)
							{
									
									machine_error_hold.userErr_hold.bits.gIBusAcmpOCP=0;
									PwmOutputsEnable(); /* 输出恢复也不能绕过安全停机锁定。 */
							}
						
							}
							break;
					default:
						break;
				
				
				
				}


}
