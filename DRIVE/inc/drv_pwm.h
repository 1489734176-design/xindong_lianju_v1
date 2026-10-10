#ifndef __DRV_PWM_H
#define __DRV_PWM_H

/** Files includes */
#include "mm32_device.h"
#include "HAL_conf.h"

#define SET_CCR1_VAL(Value)         WRITE_REG(TIM1->CCR1, Value)
#define SET_CCR2_VAL(Value)         WRITE_REG(TIM1->CCR2, Value)
#define SET_CCR3_VAL(Value)         WRITE_REG(TIM1->CCR3, Value)
#define SET_CCR4_VAL(Value)         WRITE_REG(TIM1->CCR4, Value)

#define DISABLE_PWMOUT()            CLEAR_BIT(TIM1->BDTR, TIM_BDTR_MOE_Msk)
#define ENABLE_PWMOUT()             PwmOutputsEnable() /* 所有输出恢复先判断安全锁定。 */

#define READ_TIM1_UPDATE_FLAG()     READ_BIT(TIM1->SR, TIM_IT_Update)
#define CLEAN_TIM1_UPDATE_FLAG()    CLEAR_BIT(TIM1->SR, TIM_IT_Update)

#define TIM1_BREAK_ENABLE()         SET_BIT(TIM1->BDTR,TIM_BDTR_BKE)
#define TIM1_BREAK_DISABLE()        CLEAR_BIT(TIM1->BDTR,TIM_BDTR_BKE)
#define READ_TIM1_BREAK_FLAG()      READ_BIT(TIM1->SR, TIM_IT_Break)
#define CLEAN_TIM1_BREAK_FLAG()     CLEAR_BIT(TIM1->SR, TIM_IT_Break)


void Drv_pwm_init(TIM_TypeDef * pTim, uint16_t u16Period,uint16_t u16DeadTime);
extern void UH_VL_HPwmLOn(void);
extern void WH_UL_HPwmLOn(void);
extern void WH_VL_HPwmLOn(void);
extern void VH_WL_HPwmLOn(void);
extern void UH_WL_HPwmLOn(void);
extern void VH_UL_HPwmLOn(void);



//U+V-=A+B-
void UH_VL_HOnLOn(void );
//U+W-=A+C-
void UH_WL_HOnLOn(void);
//V+W-=B+C-
void VH_WL_HOnLOn(void);

//V+U-=B+A-
void VH_UL_HOnLOn(void);
//W+U-=C+A-
void WH_UL_HOnLOn(void);

//W+V-=C+B-
void WH_VL_HOnLOn(void);




void UL_HOffLOn(void);
void VL_HOffLOn(void);
void WL_HOffLOn(void);
void UH_HOnLOff(void);
void VH_HOnLOff(void);
void WH_HOnLOff(void);


void UVWL_HOffLPwm(void );

void Start_Motor(void);
void Stop_Motor(void);
void PwmOutputsEnable(void); /* 故障恢复及唤醒统一使用受安全锁定约束的入口。 */

void Brake_Motor(void);
/* 安全锁定中的固定硬刹车输出，不能作为运行 PWM 使能入口。 */
void Safety_Brake_Motor(uint8_t start);
void PwmDutyUpdate(uint16_t u16Duty);



#endif



