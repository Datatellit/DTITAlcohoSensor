#include "usart1.h"
#include <string.h>

#if Printf_Support_En == 0
#include <stdarg.h>
#endif

uint8_t USART1_RX_BUF[USART1_REC_LEN];	        //uart1 receive cache
volatile uint16_t USART1_RX_LEN=0;				//uart1 receive data len
volatile uint8_t bIsWritingRBuf = 0;

volatile uint16_t timeoutTick = 0;

void USART1_Conf(USART_InitTypeDef* USART_InitStruct)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	
#if EN_USART1_RX == 1  //使能接收
	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
#endif
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_StructInit(&GPIO_InitStructure);
#if EN_USART1_RX == 1  //使能接收
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
#endif
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART Rx as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	//GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
	
	/* USART configuration */
	USART_Init(USART1, USART_InitStruct);
	
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	/* Enable the usart1 Interrupt */
#if EN_USART1_RX == 1  //使能接收
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
* @brief  Initialize the USART1: Configure USART.
* @param  None
* @retval None
*/
void Usart1_Init(int speed)
{
	USART_InitTypeDef USART_InitStructure;
	/* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
	/* USART configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART1_Conf(&USART_InitStructure);
#if EN_USART1_RX == 1  //使能接收
	// interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

void Usart1_EnableRxTx(const uint8_t xRxEnable, const uint8_t xTxEnable)
{
	/* If xRXEnable enable serial receive interrupts. If xTxENable enable
	 * transmitter empty interrupts.
	 */
	if(xRxEnable) {
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	} else {
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	}
	if(xTxEnable) {
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	} else {
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
}

/******************************************************************************
           加入以下代码，支持printf函数，而不需要选择use MicroLIB
******************************************************************************/
#if Printf_Support_En == 1
/**
  *****************************************************************************
  * @Name   : 重定义out_char函数
  *
  * @Brief  : none
  *
  * @Input  : ch：发送字符
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
int putchar(int c)
{
	uint16_t timeout = 0;
	
	
	USART_SendData(USART1, (uint16_t)c);  //发送数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)  //循环发送，直到发送完毕
	{
		timeout++;
		if (timeout > 800)
		{
			timeout = 0;
			break;
		}
	}
	
	return c;
}

#else

void Usart1PutChar(const uint8_t c)
{
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Usart1_PutString(const uint8_t *s)
{
	while (*s != '\0') {
		Usart1PutChar(*s);
		s++;
	}
}

void Usart1_PutData(const uint8_t *TxBuffer, uint16_t len)
{
	while( len-- ) {
		while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
		USART_SendData(USART1, *TxBuffer);
		TxBuffer++;
	}
}

void Usart1_printf(char *format, ...)
{
	va_list ap;
	char string[512];
	
	va_start(ap, format);
	vsprintf(string, format, ap);
	va_end(ap);
	
	Usart1_PutString((uint8_t *)string);
}

#endif

void Usart1_Clear()
{
	memset(USART1_RX_BUF, 0x00, sizeof(USART1_RX_BUF));
	USART1_RX_LEN = 0;
}
