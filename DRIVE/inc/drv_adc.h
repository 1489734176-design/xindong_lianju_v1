#ifndef __DRV_ADC_H
#define __DRV_ADC_H

/** Files includes */
#include <stdio.h>
#include "hal_adc.h"

#define GET_ADC_VALUE(Channel)      (READ_REG(*(&(ADC1->ADDR0) + Channel)) & 0xFFF)
void drv_adc_init(void);
#endif

