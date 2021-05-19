#include "sys.h"

/* Public variables ----------------------------------------------------------*/
//u32 gSysTickCount = 0;

#if !defined (__ICCARM__)  //IAR����
/**
  *****************************************************************************
  * @Name   : THUMBָ�֧�ֻ������
  *
  * @Brief  : �������·���ʵ��ִ�л��ָ��WFI
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void WFI_SET(void)
{
	WFI;    
}

/**
  *****************************************************************************
  * @Name   : �ر������ж�(���ǲ�����fault��NMI�ж�)
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}

/**
  *****************************************************************************
  * @Name   : ���������ж�
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}

#endif

/**
  *****************************************************************************
  * @Name   : ϵͳ��λ
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void SYS_SoftReset(void)
{
	SCB->AIRCR = 0x05fa0000 | 0x00000004;  //������ο���Cortex-M3Ȩ��ָ��(����).pdf����285ҳ����
}

/**
  *****************************************************************************
  * @Name   : ����GPIOx�ܽ�����ߵ�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO��
  *           Pin:   IO��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_SetLevel(GPIO_TypeDef* GPIOx, uint8_t Pin)
{
	GPIOx->ODR &= ~(1<<Pin);
	GPIOx->ODR |= 1<<Pin;
}

/**
  *****************************************************************************
  * @Name   : ����GPIOx�ܽ�����͵�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO��
  *           Pin:   IO��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_ResetLevel(GPIO_TypeDef* GPIOx, uint8_t Pin)
{
	GPIOx->ODR &= ~(1<<Pin);
}

/**
  *****************************************************************************
  * @Name   : ��ȡGPIOx�ܽŵ�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx:  IO��
  *           Pin:    IO��
  *           In_Out: ����Ĵ�����������Ĵ���
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetLevel(GPIO_TypeDef* GPIOx, uint8_t Pin, uint8_t In_Out)
{
	if(In_Out)  //��ȡ��������Ĵ���
	{
		if(GPIOx->ODR & (1<<Pin))  printf("\r\nGPIOx->Pin Out: 1\r\n");
		else                       printf("\r\nGPIOx->Pin Out: 0\r\n");
	}
	else  //��ȡ��������Ĵ���
	{
		if(GPIOx->IDR & (1<<Pin))  printf("\r\nGPIOx->Pin In: 1\r\n");
		else                       printf("\r\nGPIOx->Pin In: 0\r\n");
	}
}

/**
  *****************************************************************************
  * @Name   : ��ȡGPIO�����ַ
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetAddress(void)
{
	printf("\r\nGPIOA Address: 0x%x\r\n", (uint32_t)GPIOA);
	printf("GPIOB Address: 0x%x\r\n", (uint32_t)GPIOB);
	printf("GPIOC Address: 0x%x\r\n", (uint32_t)GPIOC);
	printf("GPIOD Address: 0x%x\r\n", (uint32_t)GPIOD);
	printf("GPIOE Address: 0x%x\r\n", (uint32_t)GPIOE);
	
	printf("GPIOA_Base Address: 0x%x\r\n", (uint32_t)GPIOA_BASE);
	printf("GPIOB_Base Address: 0x%x\r\n", (uint32_t)GPIOB_BASE);
	printf("GPIOC_Base Address: 0x%x\r\n", (uint32_t)GPIOC_BASE);
	printf("GPIOD_Base Address: 0x%x\r\n", (uint32_t)GPIOD_BASE);
	printf("GPIOE_Base Address: 0x%x\r\n", (uint32_t)GPIOE_BASE);
}

uint32_t BSP_CPU_ClkFreq(void) {
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    return ((uint32_t)rcc_clocks.HCLK_Frequency);
}

/*
void SystemTickTack() {
    gSysTickCount++;
}
*/
