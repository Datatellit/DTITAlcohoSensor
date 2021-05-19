#ifndef _FLASH_OP_H_
#define _FLASH_OP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

// ST STM32F030
#define UNIQUE_ID_ADDRESS         	(0x1FFFF7AC)

#define FLASH_PAGE_SIZE           	0x400     /* 1024 Bytes */
#define FLASH_START_ADDRESS         (uint32_t)FLASH_BASE
// 16K Flash
#define FLASH_TOTAL_SIZE         	(uint32_t)0x4000
#define FLASH_END_ADDRESS	    	(FLASH_START_ADDRESS + FLASH_TOTAL_SIZE)
#define FLASH_CONFIG_ADDR	    	(FLASH_END_ADDRESS - FLASH_PAGE_SIZE)

int FLASH_WriteBuf(const uint32_t startAddr, const uint32_t *buf, const uint16_t len);
void FLASH_ReadBuf(uint32_t ReadAddress, uint32_t *ReadBuf, uint16_t ReadNum);
void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData);
void WriteFlashData(uint32_t WriteAddress, const uint32_t *data, uint16_t num);
void ReadFlashData(uint32_t ReadAddress, uint32_t *ReadBuf, uint16_t ReadNum);

#ifdef __cplusplus
}
#endif

#endif
