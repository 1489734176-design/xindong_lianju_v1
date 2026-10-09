#ifndef __MOTOR_CONFIG_H
#define __MOTOR_CONFIG_H

#define MOTOR_TIMR_FRQ_HZ 12000000
#define MOTOR_TIMR_FRQ_MHZ 12



/****************************************************************************
*
*扇区时间限制
*
*****************************************************************************/
#define MaskTime_Max_US                                         (1500)/*最大屏蔽时间uint:us*/
#define MaskTime_Max_TIMER_COUNT                                (u32)(MaskTime_Max_US*MOTOR_TIMR_FRQ_MHZ) 

#define MaskTime_Min_US                                         (20)/*最小屏蔽时间uint:us*/
#define MaskTime_Min_TIMER_COUNT 																(u32)(MaskTime_Min_US*MOTOR_TIMR_FRQ_MHZ) 

#define ShiftTime_Max_US                                        (3000)/*最大移相时间uint:us*/
#define ShiftTime_Max_TIMER_COUNT                               (u32)(ShiftTime_Max_US*MOTOR_TIMR_FRQ_MHZ) 


#define ShiftTime_Min_US                                        (10)/*最小移相时间uint:us*/
#define ShiftTime_Min_TIMER_COUNT                           		(u32)(ShiftTime_Min_US*MOTOR_TIMR_FRQ_MHZ) 


#define SectorTIme_Max_US                                       (20000)/*最大计算扇区时间uint:us*/
#define SectorTIme_Max_TIMER_COUNT                           		(u32)(SectorTIme_Max_US*MOTOR_TIMR_FRQ_MHZ) 



#define SectorTIme_Min_US                                       (50)/*最小计算扇区时间uint:us*/
#define SectorTIme_Min_TIMER_COUNT                              (u32)(SectorTIme_Min_US*MOTOR_TIMR_FRQ_MHZ) 
/****************************************************************************/

/****************************************************************************

*屏蔽移相设置
*
*****************************************************************************/
//#define Sector_CloseLoop_CntSet                                 (6)/*换相时间闭环计算滤波*/

#define MaskTime_OpenLoop_US                                    (950)/*开环屏蔽时间uint:us*/
#define MaskTime_OpenLoop_TIMER_COUNT														(u32)(MaskTime_OpenLoop_US*MOTOR_TIMR_FRQ_MHZ) 

#define ShiftTime_OpenLoop_US                                   (20)/*开环移相时间uint:us*/
#define ShiftTime_OpenLoop_TIMER_COUNT	                         (u32)(ShiftTime_OpenLoop_US*MOTOR_TIMR_FRQ_MHZ) 

#define MaskTime_ClosedLoop_Angle                               (30)/*闭环屏蔽角度:1-50*/
#define ShiftTime_ClosedLoop_Angle                              (15)/*闭环移相角度:1-50*/

/****************************************************************************
*失步保护
*
*****************************************************************************/
#define LosStep_Protect_En                                      (1)/*失步保护*/
#define LosStep_Protect_CntSet                                  (30)/*失步保护滤波*/
/****************************************************************************/
/****************************************************************************

*静止启动
*
*****************************************************************************/
//#define MotorStart_IPD                                          (1)
//#define MotorStart_Drag                                         (2)
//#define MotorStart_LowVolt                                      (3)
//#define MotorStart_Mode                                         (MotorStart_IPD)

#define MotorStart_IPD_PulseTime                                (60)/*IPD脉冲时间uint:us*/
//#define MotorStart_IPD_ChargeTime                               (500)/*IPD自举时间uint:us*/
#define MotorStart_IPD_IdleTime                                 (1000)/*IPD空闲时间uint:us*/

//#define MotorStart_IPD_Drive_Phase2                             (0)
//#define MotorStart_IPD_Drive_Phase3                             (1)
//#define MotorStart_IPD_Drive_Set                     		    (MotorStart_IPD_Drive_Phase2)/*选择1组脉冲可选择2相或3相*/

//#define MotorStart_IPD_PulseRephaseEn                           (0)/*IPD反向脉冲使能*/
/****************************************************************************/

#define ForceChangePhase_En                               		(0)/*强制换相*/ 
#define ForceChangePhase_CntSet                            		(480)/*强制换相时间,单位1/pwm_fre*/

/****************************************************************************
*
*热启动
*
*****************************************************************************/
#define Motion_En                                               (1)/*热启动使能*/
#define Motion_Pass_CntSet                                      (6)/*滑行判断滤波*/          
//#define Motion_Pass_LoopRun_Set                                 (1)/*滑行切入后换相开环或是闭环*/

#define Motion_SectorTime_Lim_US                                (10000)/*滑行限制扇区条件uint:us*/

#define Motion_CheckTime_Count                                 (40*SLOWLOOP_1ms_CNT_LOAD)/*滑行开窗最低时间uint:ms*/
//#define Motion_CheckTime_OnceIRQ_Add_MS                         (2)/*滑行开窗递增时间uint:ms*/

#define Motion_Fail_BrakeTime_Count                                 (30*SLOWLOOP_1ms_CNT_LOAD)/*滑行刹车最低时间uint:ms*/

#define  Motion_ADC_THRESHOLD                                       (130) //滑行切入阈值
/****************************************************************************
*
*停机
*
*****************************************************************************/
#define SBrake_En                                               (0)/*软刹车使能*/
#define Sbrake_Duty_Add                                     (1)/*0-32768刹车占空比*/
#define Sbrake_Duty_Max                                     (u16)(PWM_PRIOD_LOAD*0.7f)/*0-32768刹车占空比*/
#define Sbrake_Duty_Min                                     (u16)(PWM_PRIOD_LOAD*0.1f)/*0-32768刹车占空比*/
#define Sbrake_Time_MS                                          (200*SLOWLOOP_1ms_CNT_LOAD)/*软刹车时间 uint:ms*/   
 
#define HBrake_En                                               (0)/*硬刹车使能*/
#define HBrake_MIN_Time_MS_CNT                                      (155*SLOWLOOP_1ms_CNT_LOAD)/*硬刹车时间 uint:ms*/
#define HBrake_Time_MS_CNT                                          (400*SLOWLOOP_1ms_CNT_LOAD)/*硬刹车时间 uint:ms*/
    
//#define NoBrake_Time_MS                                         (100)/*自然停机时间 uint:ms*/   

/****************************************************************************

*PWM_DUTY加减参数 
*
*****************************************************************************/


#define PWM_ADD_GAP                6  //2
#define PWM_ADD_VALUE              1  //1


#define PWM_SUB_GAP                1
#define PWM_SUB_VALUE              1


#define PWM_ADD_FAST_GAP                3  //2
#define PWM_ADD_FAST_VALUE              1  //1


#define PWM_SUB_FAST_GAP                3
#define PWM_SUB_FAST_VALUE              1
#endif
