#ifndef __LED_H
#define __LED_H
#include <stdio.h>
#include "mm32_device.h"
#include "hal_conf.h"
#include "parameter.h"
#define LED_RCC_CLOCKGPIO          (RCC_AHBENR_GPIOA_Msk)
#define LED_PIN1_PORT                   GPIOA
#define LED_PIN1                    GPIO_Pin_1

#define LED_PIN2_PORT                   GPIOA
#define LED_PIN2                    GPIO_Pin_13

/* PA14 专作安全输入，禁止 LED 改写其方向或输出电平。 */
#if LED_PA14_EN
#define LED_PIN3_PORT                   GPIOA
#define LED_PIN3                   GPIO_Pin_14
#endif

#if(1)
#define LED_PIN1_OUT()  MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x01U << GPIO_CRL_MODE1_Pos)
#define LED_PIN2_OUT()  MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE13, 0x01U << GPIO_CRH_MODE13_Pos)
/* PA14 专作安全输入，禁止 LED 改写其方向或输出电平。 */
#if LED_PA14_EN
#define LED_PIN3_OUT()  MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE14, 0x01U << GPIO_CRH_MODE14_Pos)
#endif

#define LED_PIN1_IN()  MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x00U << GPIO_CRL_MODE1_Pos)
#define LED_PIN2_IN()  MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE13, 0x00U << GPIO_CRH_MODE13_Pos)
/* PA14 专作安全输入，禁止 LED 改写其方向或输出电平。 */
#if LED_PA14_EN
#define LED_PIN3_IN()  MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE14, 0x00U << GPIO_CRH_MODE14_Pos)
#endif

#define LED_PIN1_HIGHT() WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS1)
#define LED_PIN1_LOW()   WRITE_REG(GPIOA->BRR, GPIO_BRR_BR1)

#define LED_PIN2_HIGHT() WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS13)
#define LED_PIN2_LOW()   WRITE_REG(GPIOA->BRR, GPIO_BRR_BR13)

/* PA14 专作安全输入，禁止 LED 改写其方向或输出电平。 */
#if LED_PA14_EN
#define LED_PIN3_HIGHT() WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS14)
#endif
/* PA14 专作安全输入，禁止 LED 改写其方向或输出电平。 */
#if LED_PA14_EN
#define LED_PIN3_LOW()   WRITE_REG(GPIOA->BRR, GPIO_BRR_BR14)
#endif
#endif
typedef union
{
    unsigned char  Word;
    struct
    {
        unsigned char gB0:1;
        unsigned char gB1:1;
        unsigned char gB2:1;
        unsigned char gB3:1;
        unsigned char gB4:1;
        unsigned char gB5:1;
        unsigned char gB6:1;
        unsigned char gB7:1;
    }bits;
}BITS_UNION;


/*output */
typedef struct 
{
    uint8_t     LedDeal;
	uint8_t		LedRunMode;/**/
    uint16_t	LedRunTimes;	/*led循环次数*/
    uint16_t	LedRunCnt_Once;/*led一次几闪*/



    uint8_t     gSocUpdate;
    uint8_t     gSocDislay;
}User_LED_Control_t;

extern BITS_UNION ledData;



#define LED1 											ledData.bits.gB1
#define LED2 											ledData.bits.gB2
#define LED3 											ledData.bits.gB3
#define LED4 											ledData.bits.gB4
#define LED5 											ledData.bits.gB5
#define LED6 											ledData.bits.gB6 

#define led_on		1
#define led_off		0

#define LED1_ON                     	  			LED1 = led_on
#define LED1_OFF                    	        	LED1 = led_off
#define LED1_FLASH                  	      		LED1 = ~LED1
#define LED2_ON                     	       	    LED2 = led_on
#define LED2_OFF                    	      	    LED2 = led_off
#define LED2_FLASH                  	     		LED2 = ~LED2
#define LED3_ON                     	   			LED3 = led_on
#define LED3_OFF                    	            LED3 = led_off
#define LED3_FLASH                  	       	    LED3 = ~LED3

#define LED4_ON                     	  			LED4 = led_on
#define LED4_OFF                    	        	LED4 = led_off
#define LED4_FLASH                  	      		LED4 = ~LED4
#define LED5_ON                     	       	    LED5 = led_on
#define LED5_OFF                    	      	    LED5 = led_off
#define LED5_FLASH                  	     		LED5 = ~LED5
#define LED6_ON                     	   			LED6 = led_on
#define LED6_OFF                    	            LED6 = led_off
#define LED6_FLASH                  	       	    LED6 = ~LED6

void led_init(void);
void LED_Scan(void);
void LedError_Run(void);
uint8_t LedError_Control(void);
void LedSoc_Run(uint32_t volt_vx100,uint8_t display_en,uint8_t fast_mode);
void UO_Led_Handler(void);
#endif

