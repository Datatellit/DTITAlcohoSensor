
#ifndef __USART1_H
#define __USART1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

#define Printf_Support_En   0  				//�Ƿ�֧��C��׼��printf()������0: ��֧�֣�1: ֧��
#define EN_USART1_RX        1  				//���ڽ���ʹ�ܡ�0����ֹ����, 1���������
#define USART1_TIMEOUT 		50    			// 500ms
#define USART1_REC_LEN 		16 	        	// �����������ֽ���

#if EN_USART1_RX == 1  //ʹ�ܽ���
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
