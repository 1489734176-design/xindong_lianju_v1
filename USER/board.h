#ifndef __BOARD_H
#define __BOARD_H

/** Files includes */
#include <stdio.h>
#include "mm32_device.h"
#include "hal_conf.h"
#include "parameter.h"
/** ADC interface */
#define BEMF_U_CHANNEL              ADC_Channel_4 	
#define BEMF_U_RANK                 (0)
#define BEMF_V_CHANNEL              ADC_Channel_3
#define BEMF_V_RANK                 (1)
#define BEMF_W_CHANNEL              ADC_Channel_2
#define BEMF_W_RANK                 (2)

#define IPEAK_CHANNEL        			ADC_Channel_5  	
#define IPEAK_RANK             		(3)

#define TMOS_CHANNEL                ADC_Channel_6 	
#define TMOS_RANK                   (4)

#define SPEED_CHANNEL               ADC_Channel_7  	
#define SPEED_RANK                  (5)

#define ON_START_CHANNEL               ADC_Channel_1  	
#define ON_START_RANK                  (6)



#define VBUS_CHANNEL                ADC_Channel_0  	
/* 数字方式不采样 PB0；母线电压序号前移，ADC 方式保留原 8 通道。 */
#if ON_OFF_DIGITAL_EN
#define VBUS_RANK                   (6)
#define ADC_SCAN_LAST_RANK          (6)
#else
#define VBUS_RANK                   (7)
#define ADC_SCAN_LAST_RANK          (7)
#endif


//#define  DIR_CHANNEL                ADC_Channel_7 	
//#define  DIR_RANK                   (7)

#define ADC_GPIO_CLK               (RCC_AHBENR_GPIOA_Msk | RCC_AHBENR_GPIOB_Msk)



//#define DIR_PORT                    GPIOA
//#define DIR_PIN              			  GPIO_Pin_7

#define SPEED_PORT                  GPIOA
#define SPEED_PIN                   GPIO_Pin_7

#define VBUS_PORT                   GPIOB
#define VBUS_PIN                    GPIO_Pin_1

#define IPEAK_PORT             		GPIOA
#define IPEAK_PIN             		GPIO_Pin_2

#define TMOS_PORT                   GPIOA
#define TMOS_PIN                    GPIO_Pin_15

#define ON_OFF_AD_PORT                  GPIOA
#define ON_OFF_AD_PIN                   GPIO_Pin_7

#define ON_START_PORT                   GPIOB
#define ON_START_PIN                    GPIO_Pin_0
/* ON/OFF：PB0 按下为高、松开为低；安全开关：PA14 按下为低。 */
#define ON_OFF_PRESSED()            (GPIO_ReadInputDataBit(ON_START_PORT, ON_START_PIN) == Bit_SET)
#define SAFETY_PORT                 GPIOA
#define SAFETY_PIN                  GPIO_Pin_14
#define SAFETY_PIN_SOURCE           GPIO_PinSource14
#define SAFETY_PRESSED()            (GPIO_ReadInputDataBit(SAFETY_PORT, SAFETY_PIN) == Bit_RESET)


//dir

#define DIR_RCC_CLOCKGPIO          (RCC_AHBENR_GPIOA_Msk)
#define DIR_PORT                   GPIOA
#define DIR_PIN                    GPIO_Pin_13


//led-pin



//en-port

#define EN_PIN_RCC_CLOCKGPIO          (RCC_AHBENR_GPIOA_Msk)
#define EN_PORT                   GPIOA
#define EN_PIN                    GPIO_Pin_4


/** BEMF_ADC */
#define BEMF_U_PORT                 GPIOA
#define BEMF_U_PIN                  GPIO_Pin_11

#define BEMF_V_PORT                 GPIOA
#define BEMF_V_PIN                  GPIO_Pin_12

#define BEMF_W_PORT                 GPIOA
#define BEMF_W_PIN                  GPIO_Pin_3


#if 0

/** PWM interface */
#define BLDC1_GPIO_CLK               (RCC_AHBENR_GPIOA_Msk )

#define BLDC1_UH_PORT                GPIOA
#define BLDC1_UH_PIN                 GPIO_Pin_6
#define BLDC1_VH_PORT                GPIOA
#define BLDC1_VH_PIN                 GPIO_Pin_8
#define BLDC1_WH_PORT                GPIOA
#define BLDC1_WH_PIN                 GPIO_Pin_10

#define BLDC1_UL_PORT                GPIOA
#define BLDC1_UL_PIN                 GPIO_Pin_7
#define BLDC1_VL_PORT                GPIOA
#define BLDC1_VL_PIN                 GPIO_Pin_9
#define BLDC1_WL_PORT                GPIOA
#define BLDC1_WL_PIN                 GPIO_Pin_0

//#define BLDC1_BKP_PORT               GPIOA
//#define BLDC1_BKP_PIN                GPIO_Pin_6

#define BLDC1_UH_PIN_SRC             GPIO_PinSource6
#define BLDC1_VH_PIN_SRC             GPIO_PinSource8
#define BLDC1_WH_PIN_SRC             GPIO_PinSource10
#define BLDC1_UL_PIN_SRC             GPIO_PinSource7
#define BLDC1_VL_PIN_SRC             GPIO_PinSource9
#define BLDC1_WL_PIN_SRC             GPIO_PinSource0
//#define BLDC1_BKP_PIN_SRC            GPIO_PinSource6

#define BLDC1_UH_PIN_AF              GPIO_AF_1
#define BLDC1_VH_PIN_AF              GPIO_AF_1
#define BLDC1_WH_PIN_AF              GPIO_AF_1
#define BLDC1_UL_PIN_AF              GPIO_AF_1
#define BLDC1_VL_PIN_AF              GPIO_AF_1
#define BLDC1_WL_PIN_AF              GPIO_AF_2


#endif

#if 1

/** PWM interface */
#define BLDC1_GPIO_CLK               (RCC_AHBENR_GPIOA_Msk )

#define BLDC1_UH_PORT                GPIOA
#define BLDC1_UH_PIN                 GPIO_Pin_6
#define BLDC1_VH_PORT                GPIOA
#define BLDC1_VH_PIN                 GPIO_Pin_8
#define BLDC1_WH_PORT                GPIOA
#define BLDC1_WH_PIN                 GPIO_Pin_10

#define BLDC1_UL_PORT                GPIOA
#define BLDC1_UL_PIN                 GPIO_Pin_5
#define BLDC1_VL_PORT                GPIOA
#define BLDC1_VL_PIN                 GPIO_Pin_9
#define BLDC1_WL_PORT                GPIOA
#define BLDC1_WL_PIN                 GPIO_Pin_0

//#define BLDC1_BKP_PORT               GPIOA
//#define BLDC1_BKP_PIN                GPIO_Pin_6

#define BLDC1_UH_PIN_SRC             GPIO_PinSource6
#define BLDC1_VH_PIN_SRC             GPIO_PinSource8
#define BLDC1_WH_PIN_SRC             GPIO_PinSource10
#define BLDC1_UL_PIN_SRC             GPIO_PinSource5
#define BLDC1_VL_PIN_SRC             GPIO_PinSource9
#define BLDC1_WL_PIN_SRC             GPIO_PinSource0
//#define BLDC1_BKP_PIN_SRC            GPIO_PinSource6

#define BLDC1_UH_PIN_AF              GPIO_AF_1
#define BLDC1_VH_PIN_AF              GPIO_AF_1
#define BLDC1_WH_PIN_AF              GPIO_AF_1
#define BLDC1_UL_PIN_AF              GPIO_AF_1
#define BLDC1_VL_PIN_AF              GPIO_AF_1
#define BLDC1_WL_PIN_AF              GPIO_AF_2


#endif

void Bsp_Gpio_Init(void);
void Bsp_Pwm_Init(void);
void Bsp_Pwm_Low(void); /* 六路桥臂引脚切换为普通低电平输出。 */
void Peripheral_Init(void);
void Interrupt_Init(void);
void GPIO_Toggle(GPIO_TypeDef *GPIOn, uint16_t PINn);
void EN_Port_in(void);
void EN_Port_Out_L(void);
#endif



