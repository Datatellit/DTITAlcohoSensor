
#ifndef __USART1_H
#define __USART1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

#define Printf_Support_En   0  				//是否支持C标准库printf()函数，0: 不支持；1: 支持
#define EN_USART1_RX        1  				//串口接收使能。0：禁止接收, 1：允许接收
#define USART1_TIMEOUT 		50    			// 500ms
#define USART1_REC_LEN 		16 	        	// 定义最大接收字节数

#if EN_USART1_RX == 1  //使能接收
extern volatile uint16_t timeoutTick;
extern uint8_t USART1_RX_BUF[USART1_REC_LEN];				
extern volatile uint16_t USART1_RX_LEN;	
#endif

void Usart1_Init(int speed);
void Usart1_Clear(void);
void Usart1_EnableRxTx(const uint8_t xRxEnable, const uint8_t xTxEnable);

#if Printf_Support_En == 0
void Usart1PutChar(const uint8_t c);
void Usart1_PutString(const uint8_t *s);
void Usart1_PutData(const uint8_t *TxBuffer, uint16_t len);
void Usart1_printf(char *format, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __USART1_H */
