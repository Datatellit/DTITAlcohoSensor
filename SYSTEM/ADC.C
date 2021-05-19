#include "ADC.h"
#include "delay.h"

// ADC without DMA
void Peripheral_Clock_Init(void)
{
    // ADC频率：48M / 4 = 12M
	//ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4); 

    /* Enable peripheral clocks ------------------------------------------------*/
	/* Enable ADC1 and GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	/* Configure PA.00 (ADC Channel_IN0) as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;				//IO口为模拟输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;            //AD口设置为浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;             // 禁止连续转换，手工触发由定时器触发
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel 0 configuration */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_55_5Cycles);
	
	//ADC_VrefintCmd(ENABLE);
	//ADC_TempSensorCmd(ENABLE);
	
	/* Start ADC1 calibaration */
	ADC_GetCalibrationFactor(ADC1);
}

void ADC_Configuration()
{
    Peripheral_Clock_Init();
    ADC1_GPIO_Config();
    ADC1_Init();    
}

uint16_t ADC_ReadValue(const uint8_t ch, const uint8_t _sampleTime)
{	
	static uint16_t value = 0;
	
	ADC_ChannelConfig(ADC1, ch, _sampleTime);
	ADC_Cmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);
	ADC_StartOfConversion(ADC1);
	
	// Wait convert finished
	uint8_t lv_timeout = 0;
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET && lv_timeout++ < 100) {
		value = ADC_GetConversionValue(ADC1);
		delay_us(5);
	}
	
	// Clear flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_Cmd(ADC1, DISABLE);
	
	return value;
}

uint16_t ADC_ReadMAValue(const uint8_t ch, const uint8_t _count, const uint8_t _sampleTime)
{
	uint32_t lv_adcSum = 0;
	uint16_t lv_newData;
	for( uint8_t i = 0; i < _count + 1; i++ ) {
		lv_newData = ADC_ReadValue(ch, _sampleTime);
		if(i != 0) { // Skip the first 1
		  lv_adcSum += lv_newData;
		}
	}
	return((uint16_t)(lv_adcSum / _count + 0.5));
}
