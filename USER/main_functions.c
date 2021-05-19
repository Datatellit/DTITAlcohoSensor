/**
  ******************************************************************************
  * @file    USER/main_functions.c
  * @author  SBS@DataTellIt Inc.
  * @version V1.0.1
  * @date    May 2, 2021
  * @brief   This is the implementation of main functions of Alcoho sensor Application
  ******************************************************************************
  *
  * <h2><center>&copy; COPYRIGHT 2016-2021 DataTellIt Inc.</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main_functions.h"
#include "delay.h"

/* Public variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ALCOHO_READ_INTERVAL_MS         50
#define ALCOHO_PRINT_COUNT              20      // period = ALCOHO_READ_INTERVAL_MS * ALCOHO_PRINT_COUNT

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t m_bPrint = 0;
float m_MaxAD = 0;

/* Private function prototypes -----------------------------------------------*/
void USART_Configuration(void);
void ADC_Configuration(void);
uint16_t ADC_ReadMAValue(const uint8_t _count);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* --------------------- Start implementation --------------------------------*/
/**
  * @brief  Multi-sensor runtime setup, Arduino compatible
  * @param  None
  * @retval None
  *
  */
void setup() {
    // System Init
	USART_Configuration();
	ADC_Configuration();
	delay_init();
    
    printf("**********************************************************\r\n");
    printf("*                                                        *\r\n");
    printf("*  Thank you for using DataTellIt Alcoho Sensor ! ^_^    *\r\n");
    printf("*                                                        *\r\n");
    printf("**********************************************************\r\n");
}

/**
  * @brief  Multi-sensor main loop, Arduino compatible
  * @param  None
  * @retval None
  *
  */
int loop() {
	
    uint16_t lv_value = ADC_ReadMAValue(10);
    /* Printf message with AD value to serial port every 1 second */
    //printf("The current AD value = %4.2fV \r\n", (lv_value/4096)*3.3);
    if(lv_value > m_MaxAD ) {
        m_MaxAD = lv_value;
    }        
    
    if( ++m_bPrint > ALCOHO_PRINT_COUNT ) {
        // [0..4095] -> [0..100]
        // Y = 0.0432X - 14.015, X = 23.121Y + 324.42
        printf("%4.1f ", (m_MaxAD < 325 ? 0 : (m_MaxAD * 43.2 - 14015) / 1000));
        //printf("%4.1f ", m_MaxAD);
        m_bPrint = 0;
        m_MaxAD = 0;
    }
    delay_ms(ALCOHO_READ_INTERVAL_MS);
    
    return 0;
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure; 

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    /*
    *  USART1_TX -> PA9 , USART1_RX -> PA10
    */				
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure); 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Configure the ADC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void ADC_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    // 时钟分频72M/8=9M 最大时钟不超过14M
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    /* Configure PA.00 (ADC Channel_0) as analog input -----------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ADC1 configuration ----------------------------------------------------*/    
    ADC_DeInit(ADC1);                                                       /* ADC复位 */
    ADC_DMACmd(ADC1, DISABLE);                                              /* 禁止DMA */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	                    /* 独立模式 */
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                /* 连续多通道模式 */
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	                    /* 连续转换 */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     /* 转换不受外界决定 */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		            /* 右对齐 */
    ADC_InitStructure.ADC_NbrOfChannel = 1;					                /* 扫描通道数 */
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel_0 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
    ADC_Cmd(ADC1, ENABLE);                                                  /* Enable ADC1 */
    ADC_ResetCalibration(ADC1);                                             /* ADC寄存器复位校准函数 */
    while(ADC_GetResetCalibrationStatus(ADC1));                             /* 等待上一步完成 */
    ADC_StartCalibration(ADC1);                                             /* 开始指定ADC的校准状态 */
    while(ADC_GetCalibrationStatus(ADC1));                                  /* 等待上一步完成 */
}

uint16_t ADC_ReadValue()
{	
	static uint16_t value = 0;
	
    // Start Conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);     // 开关_ADC软件触发-开关 状态寄存器为0
	// Wait convert finished
	uint8_t lv_timeout = 0;
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET && lv_timeout++ < 100) {
		value = ADC_GetConversionValue(ADC1);
		delay_us(5);
	}
	
	// Clear flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	return value;
}

uint16_t ADC_ReadMAValue(const uint8_t _count)
{
	uint32_t lv_adcSum = 0;
	uint16_t lv_newData;
	for( uint8_t i = 0; i < _count + 1; i++ ) {
		lv_newData = ADC_ReadValue();
		if(i != 0) { // Skip the first 1
		  lv_adcSum += lv_newData;
		}
	}
	return((uint16_t)(lv_adcSum / _count + 0.5));
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*******************(C)COPYRIGHT 2016 - 2021 DataTellIt Inc. *****END OF FILE******/
