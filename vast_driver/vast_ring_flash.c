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
static uint32_t nextAddr = PAGE_BASE_ADDRESS+4, lastAddr = PAGE_BASE_ADDRESS+4;

/***********************************************
                   function
***********************************************/
/**
  * @brief  vast_ring_flash_initialize.
  * @param
  * @retval
  */
static int16_t vast_ring_flash_clear(void)
{
	int16_t ret = VAST_OK;
	uint32_t erase_times = 0;

	erase_times = (*(volatile uint32_t *)PAGE_BASE_ADDRESS) + 1;

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError = 0;

	pEraseInit.TypeErase = TYPEERASE_SECTORS;
	pEraseInit.Sector = PAGE_ID;
	pEraseInit.NbSectors = 1;
	pEraseInit.VoltageRange = VOLTAGE_RANGE;

	ret = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

	ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, PAGE_BASE_ADDRESS, (uint32_t)(erase_times));

	HAL_FLASH_Lock();

	lastAddr = PAGE_BASE_ADDRESS+4;
	nextAddr = PAGE_BASE_ADDRESS+4;

    return ret;
}

/**
  * @brief  vast_ring_flash_initialize.
  * @param
  * @retval
  */
int16_t vast_ring_flash_initialize(void)
{
	int16_t ret = VAST_OK;
	uint32_t nowAddr = 0;

	//HAL_FLASH_Unlock();
	nowAddr = (*(volatile uint32_t *)(PAGE_BASE_ADDRESS));
	//HAL_FLASH_Lock();

	if(nowAddr == 0xffffffff)
	{
		//lastAddr = PAGE_BASE_ADDRESS;
		//nextAddr = PAGE_BASE_ADDRESS;
		HAL_FLASH_Unlock();
		ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, PAGE_BASE_ADDRESS, (uint32_t)(1));
		HAL_FLASH_Lock();
	}
	else
	{
		//HAL_FLASH_Unlock();
		nowAddr = (*(volatile uint32_t *)(PAGE_BASE_ADDRESS+4));

		while ((nowAddr != 0xffffffff))
		{
			if((nowAddr < PAGE_END_ADDRESS) && (nowAddr >= PAGE_BASE_ADDRESS))
			{
				lastAddr = nextAddr;
				nextAddr = nowAddr;
			}
			else
			{
				ret = vast_ring_flash_clear();
				/* If erase operation was failed, a Flash error code is returned */
				if (ret != HAL_OK)
				{
					return ret;
				}

				break;
			}

			nowAddr = (*(volatile uint32_t *)nextAddr);
		}
		//HAL_FLASH_Lock();
	}

	if((nextAddr&0x03) != 0)
	{
		nextAddr = (nextAddr&0xfffffffc) + 4;
	}

	printf("nextAddr=%#lx\r\n", nextAddr);

    return ret;
}

/**
  * @brief  vast_ring_flash_store.
  * @param
  * @retval
  */
uint32_t vast_ring_flash_store(uint8_t *dat, uint32_t len)
{
	int16_t ret = VAST_OK;
	uint32_t nowAddr = 0;
	uint32_t i = 0;

	ASSERT(len < PAGE_SIZE,
			return VAST_ERROR);

	lastAddr = nextAddr;

	//DBG_LOG(DBG_DEBUG, "1. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);

	if (nextAddr + len >= PAGE_END_ADDRESS)
	{
		printf("%#lX + %ld >= %#lX! \r\n", nextAddr, len, PAGE_END_ADDRESS);

		//nextAddr = PAGE_BASE_ADDRESS;

		ret = vast_ring_flash_clear();
		/* If erase operation was failed, a Flash error code is returned */
		if (ret != HAL_OK)
		{
			return ret;
		}
	}

	nowAddr = nextAddr;

	nextAddr = nowAddr + len + 0x4;

	if((nextAddr&0x03) != 0)
	{
		nextAddr = (nextAddr&0xfffffffc) + 0x4;
	}

	//DBG_LOG(DBG_DEBUG, "2. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);

	HAL_FLASH_Unlock();

	ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, nowAddr, (uint32_t)(nextAddr));
	if (ret != HAL_OK)
	{
		printf("nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);
		return ret;
	}

	nowAddr = nowAddr + 4;

	//DBG_LOG(DBG_DEBUG, "3. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);
#if 1
	for(i=0; i<(len/4); i++,nowAddr+=4)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, nowAddr, (uint32_t)(*((uint32_t *)&dat[i*4])));
	}

	//DBG_LOG(DBG_DEBUG, "4. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);

	for(i=i*4; i<len; i++,nowAddr++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, nowAddr, dat[i]);
	}
#else
	for(i=0; i<len; i++,nowAddr++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, nowAddr, dat[i]);
	}
#endif


	HAL_FLASH_Lock();

    return ret;
}

/**
  * @brief  vast_ring_flash_read.
  * @param
  * @retval
  */
uint32_t vast_ring_flash_read(uint8_t *dat, uint32_t size)
{
	uint32_t len = 0, read_len = 0;
	uint32_t i = 0;

	len = nextAddr - lastAddr;

	ASSERT(((len>0) | (size>0)),
		DBG_LOG(DBG_ERROR, "nextAddr=%#lx, lastAddr=%#lx, size=%d\r\n", nextAddr, lastAddr, size);
		return 0
			);

	//printf("len=%d, size=%d\r\n", len, size);
	//printf("lastAddr=%#lx, nextAddr=%#lx \r\n", lastAddr, nextAddr);

	read_len = ((len-4) > size) ? size : (len-4);

	//HAL_FLASH_Unlock();

	for(i=0; i<read_len; i++)
	{
		dat[i] = (*(volatile uint8_t *)(lastAddr+i+4));
	}

	//HAL_FLASH_Lock();
	return i;
}

/**
  * @brief  vast_ring_flash_get_next_addr.
  * @param
  * @retval
  */
int16_t vast_ring_flash_print(void)
{
	int16_t ret = VAST_OK;
	uint32_t u32_nowAddr = 0, u32_lastAddr;
	uint32_t len = 0;
	uint32_t i = 0;

	u32_nowAddr = u32_lastAddr = PAGE_BASE_ADDRESS + 4;
	u32_nowAddr = (*(volatile uint32_t *)u32_lastAddr);

	DBG_INFO(DBG_DEBUG, "erase times: %d\r\n", (*(volatile uint8_t *)(PAGE_BASE_ADDRESS)));

	while ((u32_nowAddr != 0xffffffff))
	{
		len = u32_nowAddr - u32_lastAddr;

		DBG_INFO(DBG_DEBUG, "%#lx: ", u32_lastAddr);
		for(i=0; i<len-4; i++)
		{
			DBG_INFO(DBG_DEBUG, "%c", (*(volatile uint8_t *)(u32_lastAddr+i+4)));
		}
		DBG_INFO(DBG_DEBUG, "->(");
		for(i=0; i<len-4; i++)
		{
			DBG_INFO(DBG_DEBUG, "%02x ", (*(volatile uint8_t *)(u32_lastAddr+i+4)));
		}
		DBG_INFO(DBG_DEBUG, ")\r\n");

		u32_lastAddr = u32_nowAddr;
		u32_nowAddr = (*(volatile uint32_t *)u32_lastAddr);
	}

	return ret;
}

/**
  * @brief  vast_ring_flash_get_next_addr.
  * @param
  * @retval
  */
uint32_t vast_ring_flash_get_next_addr(void)
{
	return nextAddr;
}

/**
  * @brief  vast_ring_flash_get_last_addr.
  * @param
  * @retval
  */
uint32_t vast_ring_flash_get_last_addr(void)
{
	return lastAddr;
}

/*
int main(int argc, char *argv[])
{
	if( vast_ring_flash_initialize() != HAL_OK)
	{
		DBG_INFO(DBG_ERROR, "vast_ring_flash_initialize error.\r\n");
	}

	uint8_t buff_array[32] = {0}, read_len = 0;
	read_len = vast_ring_flash_read(buff_array, sizeof(buff_array));

	DBG_ARRAY(DBG_DEBUG, "buff_array: ", "\r\n", (char *)buff_array, read_len);

	uint8_t test_array2[] = {0x1D, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C};
	vast_ring_flash_store(test_array2, sizeof(test_array2));

	while(1)
	{
	}
	return 0;
}
*/
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



