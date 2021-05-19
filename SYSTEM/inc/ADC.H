#ifndef _ADC_H
#define _ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
 
void ADC_Configuration();
uint16_t ADC_ReadValue(const uint8_t ch, const uint8_t _sampleTime);
uint16_t ADC_ReadMAValue(const uint8_t ch, const uint8_t _count, const uint8_t _sampleTime);

#ifdef __cplusplus
}
#endif

#endif

