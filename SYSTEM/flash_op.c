#include "flash_op.h"

// Flash操作状态变量
volatile FLASH_Status FLASHStatus = FLASH_BUSY;

/*
 * Name:	    WriteFlashData
 * Function:	向内部Flashp配置页写入数据
 * Input:	    WriteAddress：数据要写入的目标地址（偏移地址）
 *             data[]：      写入的数据首地址
 *             num：         写入数据的个数
 */
void WriteFlashData(uint32_t WriteAddress, const uint32_t *data, uint16_t num)
{
    WriteAddress = (uint32_t)FLASH_CONFIG_ADDR + WriteAddress;
    num = (num + 3) / 4;
    
    // 解锁flash
	FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    
    FLASHStatus = FLASH_BUSY;
    // 擦除整页
    FLASHStatus = FLASH_ErasePage(FLASH_CONFIG_ADDR);
	if(FLASHStatus == FLASH_COMPLETE)   // flash操作完成
	{
        FLASHStatus = FLASH_BUSY;
        while( num-- )
        {
            // 写入数据
            FLASHStatus = FLASH_ProgramWord(WriteAddress, *data++);
            WriteAddress += 4;
        }
	}
    
    FLASHStatus = FLASH_BUSY;
    // 重新锁定flash
	FLASH_Lock();
} 

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  startAddr: start address for writing data buffer
  * @param  buf: pointer on data buffer
  * @param  len: length of data buffer (unit is 8-bit byte)
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  *         3: erase error
  *         4: startAddr is not the start address of a page
  * note: startAddr should be the start address of a page (multiple of 2048 bytes).
  * note: len (unit is byte) must be multiple of 4   
  */
int FLASH_WriteBuf(const uint32_t startAddr, const uint32_t *buf, const uint16_t len)
{
	if((startAddr & 0x7FF) > 0) return 4;

	uint32_t WriteAddress = startAddr;
	int retval = 0;
	
	// 解锁flash
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	uint8_t NbrOfPage = ((len - 1) >> 11) + 1;	// div 2048
	uint16_t num = len >> 2;
	FLASHStatus = FLASH_BUSY;
	
	/* Erase the FLASH Program memory pages */
	for(uint8_t j = 0; j < NbrOfPage; j++)
	{
		// 擦除整页
		FLASHStatus = FLASH_ErasePage(WriteAddress + (FLASH_PAGE_SIZE * j));
		if (FLASHStatus != FLASH_COMPLETE)
		{
			/* Error occurred while erasing data in Flash memory */
			return 3;
		}
		else
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
		}     
	}
	
	// 写入数据
	FLASHStatus = FLASH_BUSY;
	while( num-- )
	{
		// 写入数据
		FLASHStatus = FLASH_ProgramWord(WriteAddress, *buf++);
		WriteAddress += 4;
		
		if (FLASHStatus != FLASH_COMPLETE)
		{
			/* Error occurred while writing data in Flash memory */
			retval = 1;
			break;
		}		
	}

	FLASHStatus = FLASH_BUSY;
	// 重新锁定flash
	FLASH_Lock();
	
	return (retval);
}

void FLASH_ReadBuf(uint32_t ReadAddress, uint32_t *ReadBuf, uint16_t ReadNum)
{   
	ReadNum = (ReadNum + 3) / 4;
	while(ReadNum--)
    {
        *ReadBuf = *(__IO uint32_t *)ReadAddress;
        ReadAddress += 4;
        ReadBuf++;
	}    
}

/*
 * Name:	    ReadFlashNBtye
 * Function:	从内部Flash配置页读取N字节数据
 * Input:	    ReadAddress：数据地址（偏移地址）
 *              ReadBuf：读取到的数据存放位置指针
 *              ReadNum：读取字节个数
 * Output:      
 */
void ReadFlashData(uint32_t ReadAddress, uint32_t *ReadBuf, uint16_t ReadNum)
{   
    ReadAddress = (uint32_t)FLASH_CONFIG_ADDR + ReadAddress;
	FLASH_ReadBuf(ReadAddress, ReadBuf, ReadNum);
}
