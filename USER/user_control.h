#ifndef __USER_CONTROL_H
#define __USER_CONTROL_H
#include "stdint.h"
#include "mm32g0001.h"
#include "mc_core.h"
typedef enum 
{
    TOOL_STOP = 0,	
    TOOL_RUN = 1,
    TOOL_ERROR_STOP = 2,	
    TOOL_POWER_DOWN = 3,
    TOOL_STOPPING = 4,
    TOOL_PRESTOP = 5,
    TOOL_READY = 6,
}UC_StateMachine_e;
//typedef union 
//{
//		struct
//		{
//			unsigned OverVBUSFlag 		:1;			//Over voltage			
//			unsigned UnderVBUSFlag 		:1;	 		//Under voltage			
//			unsigned OverTempFlag 		:1;	 		//Over temperature	
//			unsigned OverIBUSFlag 		:1;			//Over current			
//			unsigned LackPhaseFlag 		:1;			
//			unsigned LockedFlag 			:1;			//Rotor Locked
//			unsigned BrakeFlag 				:1;			//Hardware fault
//			unsigned OffsetFlag 			:1;			//Hardware fault
//		}bit;
//		uint8_t Byte;
//} DiagFlag;

typedef union
{
    uint32_t word;
    struct
    {
        uint32_t    gToolEn:1;
        uint32_t    gToolDir:1;
		uint32_t    gToolTrigger:1;
		uint32_t    gAutoStopEn;
		
		uint32_t    gAutoStop;
    }bits;
}UC_Flag_u;

typedef union
{
    uint32_t word;
    struct
    {
        uint32_t 		gIBusAcmpOCP:1;
        uint32_t 		gIBusPeakOCP1:1;
        uint32_t 		gIBusAvgOCP1:1;	    
        uint32_t 		gIBusAvgOCP2:1;	
        
        uint32_t 		gIBusAvgOCP3:1;
        uint32_t 		gVBusRunUVP1:1;	    
        uint32_t 		gVBusRunUVP2:1;		
        uint32_t 		gVBusStbUVP:1;
        
        uint32_t 		gVBusOVP:1;	
        uint32_t 		gLVD:1;
        uint32_t 		gNtcMos:1;
        uint32_t 		gNtcBat:1;
        
        uint32_t 		gIBusOffset:1;
        uint32_t 		gMosBridgeH:1;
        uint32_t 		gMosBridgeL:1;
        uint32_t 		gWDTOver:1;
        uint32_t 		gHardFault:1;
			
				uint32_t 		fly_machine:1;
				
				uint32_t 		gMos_overload:1;
				
				uint32_t 		gIBusAvgLimit_Protect:1;	

    }bits;
}UC_Error_u;    

//typedef struct 
//{
//    UC_Error_u          userErr;   
//}User_Error_t;






typedef struct
{
	UC_StateMachine_e user_state;
	UC_Error_u          userErr; 
	uint16_t  IBusAvg1_Adc;
	uint16_t  IBusAvg_Adc;
	uint16_t  IBusPeak_Adc;
	uint8_t   u8_1ms_flag;
	uint32_t 	VBus_Vx100;
	uint16_t   dir_adc;
	uint16_t   key_adc;
	uint8_t gears_level;
	uint32_t sleep_cnt;
	uint16_t CCW_Auto_Mask_count;
	UC_Flag_u  flag;   
	uint16_t Mos_overload_count;
	uint16_t Recover_Mos_overload_count;
	uint16_t Recover_Mos_overload_count1;
	uint8_t  En_pin_dir;
  uint8_t	limit_current_on_flag;
	uint8_t gears_level_changed;
	uint16_t on_start_adc;
} User_list_Struct;

typedef struct
{
  UC_Error_u  userErr_hold;
	MC_Error_u  MC_error_hold;

}Machine_ERROR_HOLD;


extern User_list_Struct user_list;
extern Machine_ERROR_HOLD machine_error_hold;
void user_direction_handle(void);
void motor_on_off_control(void);
void user_state_control(void);
void user_oc_handle(void);
void Mos_overload_handle(void);
void error_hold_updata(void);
void User_Temperature_Handler(void);
void Volt_Handler(void);
void gears_deal(void);
void pwm_duty_aim_deal(void);
void user_WrenchKey_Handler(void);
void user_para_init(void);
void fly_machine_init(void);
void user_gears_handle(void);
//extern DiagFlag DiagnoseFAULT;

#endif

