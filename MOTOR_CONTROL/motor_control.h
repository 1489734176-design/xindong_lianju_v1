#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H


#include "stdint.h"
#include "mm32g0001.h"



typedef enum 
{		
    MOTOR_STOP = 0,           //停止状态
    MOTOR_MOTION = 1,		      // 查询状态
    MOTOR_POSITION = 2,	      // 测电感
    MOTOR_RUN = 3, 		       	// 闭环
    MOTOR_SBRAKE = 4,         //软刹车
    MOTOR_HBRAKE = 5,         //硬刹车
    MOTOR_BRAKE2STOP = 6,     //刹车到停止过度
    MotorStart_Drag_OPENLOOP = 7,    
		Motor_Align=8,
    MOTOR_READY = 9,    
    MOTOR_STOPWAIT = 10,    
    MOTOR_MOTION_FAIL = 11,		      // 查询状态    
}MC_StateMachine_e;

typedef struct 
{

	MC_StateMachine_e bldc_state;



	uint8_t u8Calibration_OK_flag;
	uint8_t u8CurrentOffset_error_flag;
	uint16_t CurrentOffset_ad_data;
	uint16_t pwm_duty_aim_per;//PWM目标预备值
	uint16_t pwm_duty_aim;  //PWM目标实际值
	uint16_t pwm_duty_virtual;
	uint8_t  hbrake_en;
	uint8_t  sbrake_en;
	uint16_t sbreak_pwm_duty;



}Motor_Control_list;
extern  Motor_Control_list motor_control_list;

void MC_Machine_State(void);
void CurrentOffsetCalibration(void);
void mose_check(void);

void start_mc_init(void);
void align_control(void);
u8 drag_control(void);
void drag_control_init(void);
#endif
