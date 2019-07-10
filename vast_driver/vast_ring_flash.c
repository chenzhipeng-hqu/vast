/**
 ******************************************************************************
 * @file    xxx.c
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/** @addtogroup Project
  * @{
  */

/***********************************************
                    include
***********************************************/
#include "vast_ring_flash.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
static uint32_t nextAddr = 0, lastAddr = 0;

/***********************************************
                   function
***********************************************/
/**
  * @brief  vast_ring_flash_initialize.
  * @param
  * @retval
  */
int16_t vast_ring_flash_initialize(void)
{
	int16_t ret = VAST_OK;
	uint32_t nowAddr = 0;

	nowAddr = (*(volatile uint32_t *)PAGE_BASE_ADDRESS);

	if(nowAddr == 0xffffffff)
	{
		lastAddr = nextAddr;
		nextAddr = PAGE_BASE_ADDRESS;
	}
	else
	{
		while (nowAddr != 0xffffffff)
		{
			lastAddr = nextAddr;
			nextAddr = nowAddr;

			if(nowAddr > PAGE_END_ADDRESS)
			{
				FLASH_EraseInitTypeDef pEraseInit;
				uint32_t PageError = 0;

				pEraseInit.TypeErase = TYPEERASE_SECTORS;
				pEraseInit.Sector = PAGE_ID;
				pEraseInit.NbSectors = 1;
				pEraseInit.VoltageRange = VOLTAGE_RANGE;

				ret = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
				/* If erase operation was failed, a Flash error code is returned */
				if (ret != HAL_OK)
				{
					return ret;
				}

				lastAddr = nextAddr;
				nextAddr = PAGE_BASE_ADDRESS;
				ret = VAST_ERROR;
				break;
			}

			nowAddr = (*(volatile uint32_t *)nextAddr);
		}

		printf("nextAddr=%#lx\r\n", nextAddr);
	}

    return ret;
}

/**
  * @brief  vast_ring_flash_store.
  * @param
  * @retval
  */
int16_t vast_ring_flash_store(uint8_t *dat, uint16_t len)
{
	int16_t ret = VAST_OK;
	uint32_t nowAddr = nextAddr;

	lastAddr = nextAddr;

	if (nowAddr + len >= PAGE_END_ADDRESS)
	{
		FLASH_EraseInitTypeDef pEraseInit;
		uint32_t PageError = 0;

		pEraseInit.TypeErase = TYPEERASE_SECTORS;
		pEraseInit.Sector = PAGE_ID;
		pEraseInit.NbSectors = 1;
		pEraseInit.VoltageRange = VOLTAGE_RANGE;

		ret = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
		/* If erase operation was failed, a Flash error code is returned */
		if (ret != HAL_OK)
		{
			return ret;
		}

		nextAddr = PAGE_BASE_ADDRESS;
	}

	ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, nowAddr, (uint32_t)(nowAddr+len+4));
	if (ret != HAL_OK)
	{
		return ret;
	}

	nowAddr = nowAddr + 4;

	for(uint8_t i=0; i<len; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, nowAddr, dat[i]);
		nowAddr++;
	}


	//printf("nowAddr=%lx, nextAddr+len=%lx \r\n", nowAddr, (nextAddr+len+4));

	nextAddr = nowAddr;

    return ret;
}

/**
  * @brief  vast_ring_flash_store.
  * @param
  * @retval
  */
int16_t vast_ring_flash_read(uint8_t *dat, uint8_t size)
{
	int16_t len = 0, read_len = 0;
	uint8_t i = 0;

	len = nextAddr - lastAddr;

	//printf("len=%d, size=%d\r\n", len, size);
	//printf("lastAddr=%#lx, nextAddr=%#lx \r\n", lastAddr, nextAddr);

	read_len = ((len-4) > size) ? size : (len-4);

	for(i=0; i<read_len; i++)
	{
		dat[i] = (*(volatile uint8_t *)(lastAddr+i+4));
	}

	return i;
}
/*
uint32_t save_script(uint32_t *NextAddr)
{
	uint32_t size, RecAddr;
	uint16_t next_addr[2];

	FLASH_EraseInitTypeDef f;
	uint32_t PageError = 0;

	have_start = 0;
	printf("flash write start...\r\n");

	RecAddr = nextsricpt_addr;

//	RecAddr = STM32_FLASH_END - 100;		// xxx need to delete

	printf("RecAddr is %#x\r\n", RecAddr);
	// һ��ָ��Ĵ�С
	size = ((u32)script_sum * sizeof(struct scriptInfo)) + 4;

	if (RecAddr + size >= STM32_FLASH_END)
	{
		// д���ˣ�����ȫ��
		printf("flash full,ready to erase...\r\n");
		HAL_FLASH_Unlock();
		f.TypeErase = FLASH_TYPEERASE_PAGES;
		f.PageAddress = FLASH_SAVE_ADDR;
		f.NbPages = FLASH_SAVE_SECTOR;
		HAL_FLASHEx_Erase(&f, &PageError);
		HAL_FLASH_Lock();

		RecAddr = FLASH_SAVE_ADDR;
		printf("RecAddr return to %#x\r\n", FLASH_SAVE_ADDR);
	}

	*NextAddr = RecAddr + size;

	printf("NextAddr is %#x\r\n", *NextAddr);

	next_addr[0] = *NextAddr & 0x0000ffff;
	next_addr[1] = (*NextAddr >> 16) & 0x0000ffff;

	STMFLASH_Write_NoCheck(RecAddr, (u16*)next_addr, 2);
	STMFLASH_Write_NoCheck(RecAddr + 4, (u16*)script_buffer, (size - 4) / 2);

	printf("flash write ok\r\n");

	return RecAddr;
}
*/
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



