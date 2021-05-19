#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

// 1ms interval
/// HSI clock: 8M, should call RCC_HSICmd() during system initializing
//#define TIMER_PERIOD    	999
//#define TIMER_PRESCALER 	7
/// SYSTICK: 48M
#define TIMER_PERIOD    	1999
#define TIMER_PRESCALER 	23

typedef void (*Tim_CallBack_t)();
extern Tim_CallBack_t Timer_1ms_handler;
extern Tim_CallBack_t Timer_10ms_handler;
extern Tim_CallBack_t Timer_5ms_handler;

void Timer_Init(void);

#ifdef __cplusplus
}
#endif

#endif  /* __TIMER_H */
