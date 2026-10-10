#include "drv_adc.h"
#include "board.h"

void drv_adc_init(void)
{

		ADC_InitTypeDef  ADC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_3;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Scan;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_3_5);

    /* 参数是最后一个序号：数字按键 7 通道，ADC 按键 8 通道。 */
    ADC_AnyChannelNumCfg(ADC1, ADC_SCAN_LAST_RANK);
    ADC_AnyChannelSelect(ADC1, BEMF_U_RANK, BEMF_U_CHANNEL);
    ADC_AnyChannelSelect(ADC1, BEMF_V_RANK, BEMF_V_CHANNEL);
    ADC_AnyChannelSelect(ADC1, BEMF_W_RANK, BEMF_W_CHANNEL);
    ADC_AnyChannelSelect(ADC1, IPEAK_RANK, IPEAK_CHANNEL);
	
		ADC_AnyChannelSelect(ADC1, TMOS_RANK, TMOS_CHANNEL);
		
    ADC_AnyChannelSelect(ADC1, SPEED_RANK, SPEED_CHANNEL);
/* 数字方式 PB0 保持下拉输入，不加入 ADC 扫描。 */
#if !ON_OFF_DIGITAL_EN
    ADC_AnyChannelSelect(ADC1, ON_START_RANK, ON_START_CHANNEL);
#endif
		
    ADC_AnyChannelSelect(ADC1, VBUS_RANK, VBUS_CHANNEL);	
		
		
		//----------------dir---------------
//		ADC_AnyChannelSelect(ADC1, DIR_RANK, DIR_CHANNEL);
		
		
    ADC_AnyChannelCmd(ADC1, ENABLE);
		
	 ADC_ExternalTrigSourceConfig(ADC1,ADC_ExtTrig_T1_CC4,ADC_ExtTrig_Edge_Dual,ADC_ExtTrig_Shift_0);

    /** External triggering was enabled */
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

		ADC_Cmd(ADC1, ENABLE);
}

