#ifndef __PARAMETER_H_
#define __PARAMETER_H_

#include "mm32_device.h"


//----------------------------------------------------------------------//
/* System setting */
//----------------------------------------------------------------------//
#define SYSCLK_HSI			       	    ((u32)48000000)   									// 系统时钟
#define PWMFREQ                	   	((u16)16000)    										// PWM频率
#define PWM_PRIOD_LOAD 	       			(u16)(SYSCLK_HSI/PWMFREQ/2-1)  			// PWM周期装载值

#define DEADTIME 					       		(1)      														// us, 定义死区时间
#define DEADTIME_LOAD 		       		((u16)(DEADTIME*SYSCLK_HSI/1000000))	 	// 死区装载值

#define SLOWLOOP_500us_CNT_LOAD     ((u16)(PWMFREQ/1000/2))							// TIM1 计时 500us
#define SLOWLOOP_1ms_CNT_LOAD      	((u16)(PWMFREQ/1000))								// TIM1 计时 1ms
#define SLOWLOOP_400ms_CNT_LOAD    	((u16)400*SLOWLOOP_1ms_CNT_LOAD)		// TIM1 计时 200ms

#define TIM3_TO_PWM_PERIOD 	       	(float)(1000000/PWMFREQ)  		

#define SLOWLOOP_FRE                (1000) //循环计数1次为1ms
//---------------------------------------------------------------------//
// PWM 
//---------------------------------------------------------------------//

#define  SLEEP_TIME                 				15000ul//10000   
#define LED_SCAN_EN       (1)
/* PA14 专用于安全输入，关闭涉及 PA14 的 LED 驱动及电量、档位显示。 */
#define LED_PA14_EN       (0)
#if LED_PA14_EN
#error PA14 LED drive conflicts with the safety switch
#endif

/* ON/OFF 检测：1 为 PB0 数字输入（高按下、低松开），0 为原 ADC 方式。 */
#ifndef ON_OFF_DIGITAL_EN
#define ON_OFF_DIGITAL_EN (0)
#endif
#define ON_OFF_DEBOUNCE_MS (10) /* 每毫秒采样，连续稳定 10 次才确认。 */
#define TRG_OFF_ADC       (TRG_ON_ADC - 50) /* ADC 保留大于 600 开、小于 550 关。 */

#define PWM_DUTY_GEARS1    (u32)(32768*0.6f)    
#define PWM_DUTY_GEARS2    (u32)(32768*0.8f) 
#define PWM_DUTY_GEARS3    (u32)(32768*1.0f) 
//---------------------------------------------------------------------------------------------------------------------------------------------------//
//************************************************************************************************************************************************/


//vsp 占空比控制参数
#define MAX_TOOL_DUTY 		                	(PWM_PRIOD_LOAD)//高档位2最大占空比   
#define MIN_TOOL_DUTY 		   		 						(PWM_PRIOD_LOAD*0.15f) //电位器最小占空比

#define TRG_ON_ADC               						600//电位器启动判断ADC	
#define MAX_TRG_ADC 		   		   						4000//电位器计算最大值
#define MIN_TRG_ADC              						700//电位器计算最小值	  
#define K_ADC_DUTY			   									(float)(MAX_TOOL_DUTY-MIN_TOOL_DUTY) / (float)(MAX_TRG_ADC - MIN_TRG_ADC)
	
//---PMW_duty---限制参数，此为限制VSP调节的PWM_DUTY范围，故PWM范围 由VSP和LIMIT_MAX_DUTY和VSP和LIMIT_MIN_DUTY同时决定

#define LIMIT_MIN_PWN_DUTY   (PWM_PRIOD_LOAD*0.15f) 

#define LIMIT_MAX_PWN_DUTY   (PWM_PRIOD_LOAD) 


//---------------------------------------------------------------------//
// ADC 
//---------------------------------------------------------------------//
#define ADC_REFV                 		5													// unit:V, ADC参考电压  
#define ADC_REF_VALUE          	   	4095											// 12-bit ADC

/* ------------------------------ ADC校准相关参数设置 ---------------------------- */
#define EN_OFFSET_CALIB       			(1)      									/* 是否使能判断偏置异常 */
#define CALIB_SAMPLES_K            	(10)   										// 2^10 = 1024, 校准次数设定
#define CALIB_SAMPLES              	(1024)										// 校准次数设定
#define CALIB_SAMPLES_DOUBLE       	(CALIB_SAMPLES << 1)  
#define CURRENT_OFFSET_VOLTAGE     	(0.0)    									/* 电流偏置电压, 根据实际电路设定, unit:V */
/* 将用户设定值转换为ADC采样值 */
#define CURRENT_OFFSET_VALUE				(u16)(ADC_REF_VALUE * CURRENT_OFFSET_VOLTAGE / ADC_REFV)
#define CURRENT_OFFSET_H_THD				(u16)(CURRENT_OFFSET_VALUE + 500)
#define CURRENT_OFFSET_L_THD				(u16)(CURRENT_OFFSET_VALUE - 500)

/* ---------------------------- Current Protect  ---------------------------------- */

/* ------------------------------ 电流采样硬件参数 ---------------------------- */
#define RSHUNT 												 1							/* 采样电阻 单位：mR */		
#define AMPLIFICATION_GAIN 						 (float)(10.89)		/* 运放倍数 */		
	
//#define MAX_BUS_CURRENT_SETTINT        (float)(18.0)                                   						/* 限电流 单位：A */
#define CURRENT_ADC_PER_A              (float)(RSHUNT * AMPLIFICATION_GAIN * ADC_REF_VALUE / ADC_REFV / 1000.0f)  			/* 每安电流ADC值 */ 
//#define CURRENT_LIM_VALUE              (u16)(MAX_BUS_CURRENT_SETTINT * CURRENT_ADC_PER_A) 				/* 电流ADC值 */


#define PWM_IO_BKIN_EN                  (0)
#define PWM_IO_IT_BK_EN                  (1)

#define MOS_SHORT_CURRENT               (100.0f)
#define MOS_SHORT_CURRENT_AdcValue    (u16)(MOS_SHORT_CURRENT*CURRENT_ADC_PER_A)

#define IBus_Avg_REC_MS               (200) //ms
#define IBus_Avg_REC_MS_CONUT         (u16)((float)IBus_Avg_REC_MS*SLOWLOOP_FRE/1000.0f) 

#define IBus_Avg_OCP1                (50.0f)
#define IBus_Avg_OCP1_AdcValue         (u16)(IBus_Avg_OCP1*CURRENT_ADC_PER_A)

#define IBus_Avg_OCP1_MS               (2000) //ms
#define IBus_Avg_OCP1_MS_CONUT         (u16)((float)IBus_Avg_OCP1_MS*SLOWLOOP_FRE/1000.0f) 
	
#define IBus_Avg_OCP2                 (55.0f)
#define IBus_Avg_OCP2_AdcValue         (u16)(IBus_Avg_OCP2*CURRENT_ADC_PER_A)

#define IBus_Avg_OCP2_MS               (300) //ms
#define IBus_Avg_OCP2_MS_CONUT        (u16)((float)IBus_Avg_OCP2_MS*SLOWLOOP_FRE/1000.0f) 

#define IBus_Avg_OCP3                  (75.0f)
#define IBus_Avg_OCP3_AdcValue         (u16)(IBus_Avg_OCP3*CURRENT_ADC_PER_A)

#define IBus_Avg_OCP3_MS               (20) //ms
#define IBus_Avg_OCP3_MS_CONUT        (u16)((float)IBus_Avg_OCP3_MS*SLOWLOOP_FRE/1000.0f)
#define Mos_overload_EN (0)  //mos过载保护使能


#if Mos_overload_EN
#define  Mos_overload_count_Set   							  (8) //mos过载保护次数，连续峰值保护或堵转次数保护
#define  Recover_Mos_overload_time_count     (5000) //单位ms
#endif

/*****************************************************************************/
#define Block_Protect_En                                        (1)/*堵转保护*/


#define Block_Protect_Current_En                                 (1)/*根据电流设置堵转保护时间*/

#define Block_Protect_Recover_TIME                     (100*SLOWLOOP_1ms_CNT_LOAD)/*堵转恢复时间unit:ms*/

#if Block_Protect_Current_En

#define Block_Protect_MAX_TIME                     (80*SLOWLOOP_1ms_CNT_LOAD)/*堵转时间unit:ms*/
#define Block_Protect_Current1                      (35.00)/*unit:a*/ 
#define Block_Protect_Current1_AdcValue             (u16)(Block_Protect_Current1*CURRENT_ADC_PER_A)
#define Block_Protect_Current1_TIME                 (80*SLOWLOOP_1ms_CNT_LOAD)/*堵转时间unit:ms*/


#define Block_Protect_Current2                      (55.00)/*unit:a*/ 
#define Block_Protect_Current2_AdcValue             (u16)(Block_Protect_Current2*CURRENT_ADC_PER_A)
#define Block_Protect_Current2_TIME                 (50*SLOWLOOP_1ms_CNT_LOAD)/*堵转时间unit:ms*/


#define Block_Protect_Current3                      (75.00)/*unit:a*/ 
#define Block_Protect_Current3_AdcValue             (u16)(Block_Protect_Current3*CURRENT_ADC_PER_A)
#define Block_Protect_Current3_TIME                 (50*SLOWLOOP_1ms_CNT_LOAD)/*堵转时间unit:ms*/

#define Block_Protect_Current4                      (90.00)/*unit:a*/ 
#define Block_Protect_Current4_AdcValue             (u16)(Block_Protect_Current4*CURRENT_ADC_PER_A)
#define Block_Protect_Current4_TIME                 (10*SLOWLOOP_1ms_CNT_LOAD)/*堵转时间unit:ms*/


#else
#define Block_Protect_LowDuty_DATA									            (u16)(0.70f*PWM_PRIOD_LOAD)/*堵转保护低占空比*/
#define Block_Protect_LowDuty_COUNT                            (80*SLOWLOOP_1ms_CNT_LOAD)/*低占空比堵转时间unit:ms*/
#define Block_Protect_HighDuty_COUNT                           (30*SLOWLOOP_1ms_CNT_LOAD)/*高占空比堵转时间unit:ms*/

#endif


#define Multistage_Peak_OCP_EN              (1) //多级保护，共用一个保护标志位，兼容中微65xx错误标识


#if Multistage_Peak_OCP_EN

#define IBus_Smart_Peak_OCP1                      (90.00)/*unit:a*/ 
	#define IBus_Smart_Peak_OCP1_AdcValue             (u16)(IBus_Smart_Peak_OCP1*CURRENT_ADC_PER_A)
	#define IBus_Smart_Peak_OCP1_MS                   (550) //ms
	#define IBus_Smart_OCP1_MS_Operation_Value        (u16)(32768.0f/((float)IBus_Smart_Peak_OCP1_MS*SLOWLOOP_FRE/1000.0f))

	#define IBus_Smart_Peak_OCP2                      (100.00)/*unit:a*/ 
	#define IBus_Smart_Peak_OCP2_AdcValue             (u16)(IBus_Smart_Peak_OCP2*CURRENT_ADC_PER_A)
	#define IBus_Smart_Peak_OCP2_MS                   (100) //ms
	#define IBus_Smart_OCP2_MS_Operation_Value        (u16)(32768.0f/((float)IBus_Smart_Peak_OCP2_MS*SLOWLOOP_FRE/1000.0f))
		
	#define IBus_Smart_Peak_OCP3                      (110.00)/*unit:a*/ 
	#define IBus_Smart_Peak_OCP3_AdcValue             (u16)(IBus_Smart_Peak_OCP3*CURRENT_ADC_PER_A)
	#define IBus_Smart_Peak_OCP3_MS                   (10) //ms
	#define IBus_Smart_OCP3_MS_Operation_Value        (u16)(32768.0f/((float)IBus_Smart_Peak_OCP3_MS*SLOWLOOP_FRE/1000.0f))
	
	
	#define IBus_Smart_REC_MS                         120
	#define IBus_Smart_REC_MS_Operation_Value        (u16)(32768.0f/((float)IBus_Smart_REC_MS*SLOWLOOP_FRE/1000.0f))
#else

	#define IBus_Peak_OCP1                  (70.00)/*unit:a*/ 
	#define IBus_Peak_OCP1_AdcValue         (u16)(IBus_Peak_OCP1*CURRENT_ADC_PER_A)
	#define IBus_Peak_OCP1_MS               (20) //ms
	#define IBus_Peak_OCP1_MS_CONUT        (u16)((float)IBus_Peak_OCP1_MS*SLOWLOOP_FRE/1000.0f)

#endif
#define IBus_Peak_OCP_ONCE_EN              (1)
#define IBus_Peak_OCP_ONCE               (150.0f)  //峰值保护，单次检测触发
#define IBus_Peak_OCP_ONCE_AdcValue      (u16)(IBus_Peak_OCP_ONCE*CURRENT_ADC_PER_A)




#define LIMIT_PEAK_CURRENT_EN               (0)
#define LIMIT_PEAK_CURRENT                  (100.0f)
#define LIMIT_PEAK_CURRENT_AdcValue         (u16)(LIMIT_PEAK_CURRENT*CURRENT_ADC_PER_A)


#define LIMIT_AVG_CURRENT_EN               (0)
#define LIMIT_AVG_CURRENT                  (62.0f)
#define LIMIT_AVG_CURRENT_AdcValue         (u16)(LIMIT_AVG_CURRENT*CURRENT_ADC_PER_A)


#define LIMIT_PROTECT_EN               (0)
#define LIMIT_AVG_CURRENT_PROTECT_MS       (2500)
#define LIMIT_AVG_CURRENT_PROTECT_MS_CONUT        (u16)((float)LIMIT_AVG_CURRENT_PROTECT_MS*SLOWLOOP_FRE/1000.0f)


/* ---------------------------- Voltage Protect Parameter -------------------------- */
#define EN_VOLATAGE_PROTECT    	       	(1)    														/* 母线电压保护检测使能 */ 
#define VBUS_SHUNT_RATIO           			(float)((10.0 + 100.0)/10.0)			// 母线电压分压比 (下拉电阻/(上拉电阻+下拉电阻))

#define VBUS_BASE_X100       ((u32)(VBUS_SHUNT_RATIO*ADC_REFV*100))

#define VBus_Stb_UVP_V                                          (14.80f)/*unit:v*/
#define VBus_Stb_UVP_ADC     		       	                        (u16)(VBus_Stb_UVP_V * ADC_REF_VALUE / VBUS_SHUNT_RATIO / ADC_REFV)  		/* 计算欠压保护门槛值*/
#define VBus_Stb_UVP_FiltMS                                     (100)/*unit:ms*/
#define VBus_Stb_UVP_FiltMS_COUNT                               (u16)((float)VBus_Stb_UVP_FiltMS*SLOWLOOP_FRE/1000.0f)
                    
#define VBus_Run_UVP1_V                                         (13.50f)/*unit:v*/
#define VBus_Run_UVP1_ADC     		       	                      (u16)(VBus_Run_UVP1_V * ADC_REF_VALUE / VBUS_SHUNT_RATIO / ADC_REFV)
#define VBus_Run_UVP1_FiltMS                                    (200)/*unit:ms*/
#define VBus_Run_UVP1_FiltMS_COUNT                              (u16)((float)VBus_Run_UVP1_FiltMS*SLOWLOOP_FRE/1000.0f)
	

#define VBus_Run_UVP2_V                                         (10.00f)/*unit:v*/
#define VBus_Run_UVP2_ADC     		       	                      (u16)(VBus_Run_UVP2_V * ADC_REF_VALUE / VBUS_SHUNT_RATIO / ADC_REFV)
#define VBus_Run_UVP2_FiltMS                                    (100)/*unit:ms*/
#define VBus_Run_UVP2_FiltMS_COUNT                              (u16)((float)VBus_Run_UVP2_FiltMS*SLOWLOOP_FRE/1000.0f)

    
#define VBus_OVP_V                                              (24.50f)/*unit:v*/
#define VBus_OVP_ADC     		       	                             (u16)(VBus_OVP_V * ADC_REF_VALUE / VBUS_SHUNT_RATIO / ADC_REFV)
#define VBus_OVP_FiltMS                                         (100)/*unit:ms*/
#define VBus_OVP_FiltMS_COUNT                                   (u16)((float)VBus_OVP_FiltMS*SLOWLOOP_FRE/1000.0f)


#define SOC3_MV                     	                        (u32)((18.3)*100)
#define SOC2_MV                     	                        (u32)((17.3)*100)
#define SOC1_MV                     	                        (u32)((16.3)*100)


#define MOS_NTC_10K_3535K 0
#define MOS_NTC_100K_3950K 1
#if 	MOS_NTC_10K_3535K

#define RT_60_F 2.965f   //3435K-10k
#define RT_65_F 2.537f
#define RT_70_F 2.180f
#define RT_75_F 1.880f
#define RT_80_F 1.628f
#define RT_85_F 1.414f
#define RT_90_F 1.233f
#define RT_95_F 1.079f
#define RT_100_F 0.947f
#define RT_105_F 0.833f
#define RT_110_F 0.736f
#define RT_115_F 0.652f

#elif MOS_NTC_100K_3950K

#define RT_60_F 24.701f  //3950-100k
#define RT_65_F 20.641f
#define RT_70_F 17.325f
#define RT_75_F 14.605f
#define RT_80_F 12.364f
#define RT_85_F 10.509f
#define RT_88_F 9.550f
#define RT_89_F 9.253f
#define RT_90_F 8.967f
#define RT_95_F 7.680f
#define RT_100_F 6.601f
#define RT_104_5_F 5.71f
#define RT_105_F 5.694f
#define RT_110_F 4.929f
#define RT_115_F 4.280f
#define RT_120_F 3.728f
#define RT_123_F 3.437f
#define RT_124_F 3.346f
#define RT_125_F 3.257f
#endif 


/* ---------------------------- MOS Temperature Protect Parameter -------------------------- */
#define EN_MOS_TEMP_DETECT     	       	(1)    		/* MOS温度保护检测使能 */ 
#define MOS_TEMP_UP_VOL                	5     		/* MOS温度检测上拉电压，单位：V */   
#define MOS_TEMP_UP_RES        	       	4.7f   		/* MOS温度检测上拉电阻，单位：KΩ */   
#define MOS_TEMP_OVER_TIME     					50  			/* 单位：ms */  
#define RSM_MOS_TEMP_OVER_TIME 					100  			/* 单位：ms */  
/* 将用户设定值转换为ADC采样值 */
#define MOS_TEMP_OVER_RES_RATIO					(float)(RT_100_F/(RT_100_F +MOS_TEMP_UP_RES)) 								// 分压比 (下拉电阻/(上拉电阻+下拉电阻))
#define RSM_MOS_TEMP_OVER_RES_RATIO			(float)(RT_95_F/(RT_95_F + MOS_TEMP_UP_RES)) 				// 分压比 (下拉电阻/(上拉电阻+下拉电阻))
#define MOS_TEMP_OVER_THD_ADC       		(u16)(MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV)			// 计算出过温保护门槛值
#define RSM_MOS_TEMP_OVER_THD_ADC   		(u16)(RSM_MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV)	// 计算出解除过温保护门槛值








#define Tool_Dir_CW_Value                                       (1)/*顺时针方向赋值*/
#define Tool_Dir_CCW_Value                                      (0)/*逆时针方向赋值*/
#define Tool_Dir_Init                                           (0)/*逆时针方向赋值*/





//#define MOS_SHORT_CURRENT               (8.0f)
//#define MOS_SHORT_CURRENT_AdcValue    (u16)(MOS_SHORT_CURRENT*CURRENT_ADC_PER_A)



/*反转自停参数*/
#define CCW_AUTO_STOP_CURRENT      	   	                        (12.0f)     /*unit A*/
#define CCW_AUTO_STOP_CURRENT_AdcValue    (u16)(CCW_AUTO_STOP_CURRENT*CURRENT_ADC_PER_A)
#define CCW_AUTO_STOP_START_MASK_COUNT                      (150)  //屏蔽时间，启动过程
//#define TIME_AUTO_STOP                 	                        (500ul)     /*unit 1ms*/
//#define TIME_AUTO_STOP_REDUCE          	                        (1ul)     /*unit 1ms*/
#define AUTO_STOP_Sector_CNT       	                        (180ul)     


#endif


