#include "sys.h"

/* Public variables ----------------------------------------------------------*/
//u32 gSysTickCount = 0;

#if !defined (__ICCARM__)  //IAR环境
/**
  *****************************************************************************
  * @Name   : THUMB指令不支持汇编内联
  *
  * @Brief  : 采用如下方法实现执行汇编指令WFI
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
  * @Name   : 关闭所有中断(但是不包括fault和NMI中断)
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
  * @Name   : 开启所有中断
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
  * @Name   : 系统软复位
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
	SCB->AIRCR = 0x05fa0000 | 0x00000004;  //具体请参考《Cortex-M3权威指南(中文).pdf》第285页内容
}

/**
  *****************************************************************************
  * @Name   : 设置GPIOx管脚输出高电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO组
  *           Pin:   IO号
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
  * @Name   : 设置GPIOx管脚输出低电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO组
  *           Pin:   IO号
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
  * @Name   : 读取GPIOx管脚电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx:  IO组
  *           Pin:    IO号
  *           In_Out: 输入寄存器还是输出寄存器
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetLevel(GPIO_TypeDef* GPIOx, uint8_t Pin, uint8_t In_Out)
{
	if(In_Out)  //读取的是输出寄存器
	{
		if(GPIOx->ODR & (1<<Pin))  printf("\r\nGPIOx->Pin Out: 1\r\n");
		else                       printf("\r\nGPIOx->Pin Out: 0\r\n");
	}
	else  //读取的是输入寄存器
	{
		if(GPIOx->IDR & (1<<Pin))  printf("\r\nGPIOx->Pin In: 1\r\n");
		else                       printf("\r\nGPIOx->Pin In: 0\r\n");
	}
}

/**
  *****************************************************************************
  * @Name   : 读取GPIO分组地址
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
