#include "flash_op.h"

// Flash����״̬����
volatile FLASH_Status FLASHStatus = FLASH_BUSY;

/*
 * Name:	    WriteFlashData
 * Function:	���ڲ�Flashp����ҳд������
 * Input:	    WriteAddress������Ҫд���Ŀ���ַ��ƫ�Ƶ�ַ��
 *             data[]��      д��������׵�ַ
 *             num��         д�����ݵĸ���
 */
void WriteFlashData(uint32_t WriteAddress, const uint32_t *data, uint16_t num)
{
    WriteAddress = (uint32_t)FLASH_CONFIG_ADDR + WriteAddress;
    num = (num + 3) / 4;
    
    // ����flash
	FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    
    FLASHStatus = FLASH_BUSY;
    // ������ҳ
    FLASHStatus = FLASH_ErasePage(FLASH_CONFIG_ADDR);
	if(FLASHStatus == FLASH_COMPLETE)   // flash�������
	{
        FLASHStatus = FLASH_BUSY;
        while( num-- )
        {
            // д������
            FLASHStatus = FLASH_ProgramWord(WriteAddress, *data++);
            WriteAddress += 4;
        }
	}
    
    FLASHStatus = FLASH_BUSY;
    // ��������flash
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
	
	// ����flash
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	uint8_t NbrOfPage = ((len - 1) >> 11) + 1;	// div 2048
	uint16_t num = len >> 2;
	FLASHStatus = FLASH_BUSY;
	
	/* Erase the FLASH Program memory pages */
	for(uint8_t j = 0; j < NbrOfPage; j++)
	{
		// ������ҳ
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
	
	// д������
	FLASHStatus = FLASH_BUSY;
	while( num-- )
	{
		// д������
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
	// ��������flash
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
 * Function:	���ڲ�Flash����ҳ��ȡN�ֽ�����
 * Input:	    ReadAddress�����ݵ�ַ��ƫ�Ƶ�ַ��
 *              ReadBuf����ȡ�������ݴ��λ��ָ��
 *              ReadNum����ȡ�ֽڸ���
 * Output:      
 */
void ReadFlashData(uint32_t ReadAddress, uint32_t *ReadBuf, uint16_t ReadNum)
{   
    ReadAddress = (uint32_t)FLASH_CONFIG_ADDR + ReadAddress;
	FLASH_ReadBuf(ReadAddress, ReadBuf, ReadNum);
}
