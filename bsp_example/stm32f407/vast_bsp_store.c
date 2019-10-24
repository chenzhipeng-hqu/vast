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
#include "vast_bsp.h"
#include "vast_core/device.h"
#include "vast_driver/vast_store.h"

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

/***********************************************
                   function
***********************************************/

#ifdef configUSING_CLI

/**
  * @brief  cliCmdStoreWrite
  * @param
  * @retval
  */
void cliCmdStoreWrite(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint8_t ret = VAST_OK, type = 0;
	uint32_t len = 0;
	uint8_t *p = NULL;

	printf("t1:%ld\r\n", HAL_GetTick());

	type = str2u32(argv[1]);

	if(argc > 3)
	{
		len = str2u32(argv[3]);
//		p = (uint8_t *)vast_malloc(len);
		p = (uint8_t *)malloc(len);

		if(p != NULL)
		{
			memset((void *)p, str2u32(argv[2]), len);

			//ret = vast_ring_flash_store((uint8_t *)p, len);
			ret = vast_store_write(type, (uint8_t *)p, len);

			if(ret != VAST_OK)
			{
				pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
			}

//			vast_free(p);
			free(p);
		}
		else
		{
			printf("p == NULL\r\n");
		}

	}
	else if(argc > 2)
	{
		//ret = vast_ring_flash_store((uint8_t *)argv[2], strlen(argv[2]));
		ret = vast_store_write(type, (uint8_t *)argv[2], strlen(argv[2]));

		if(ret != VAST_OK)
		{
			pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
		}
	}
	printf("t2:%ld\r\n", HAL_GetTick());
}

/**
  * @brief  cliCmdStoreRead
  * @param
  * @retval
  */

void cliCmdStoreRead(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint8_t ret = VAST_OK, type = 0, isDetail = 0, isPrintHex = 0;
	static uint8_t buff_array2[10240] = {0};
	uint16_t read_len;

	type = str2u32(argv[1]);
	isDetail = str2u32(argv[2]);
	isPrintHex = str2u32(argv[3]);
	//read_len = vast_ring_flash_read(buff_array2, sizeof(buff_array2));
	read_len = vast_store_read(type, buff_array2, sizeof(buff_array2));
	buff_array2[read_len] = 0;

	pCli->Init.Write("read from flash [%d][%d]", type, read_len, (char *)buff_array2);

	if(isDetail == 1)
	{
		DBG_INFO(DBG_DEBUG, "[%s]", (char *)buff_array2);
	}

	if(isPrintHex == 1)
	{
		DBG_ARRAY(DBG_DEBUG, "->(", "\b)", (char *)buff_array2, read_len);
	}

	DBG_INFO(DBG_INFO, "\r\n");

	if(ret != VAST_OK)
	{
		pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
	}
}

/**
  * @brief  cliCmdStoreInfo
  * @param
  * @retval
  */

void cliCmdStoreInfo(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	/*uint32_t lastAddr = vast_ring_flash_get_last_addr();
	uint32_t nextAddr = vast_ring_flash_get_next_addr();*/
	uint32_t lastAddr = vast_store_get_last_addr();
	uint32_t nextAddr = vast_store_get_next_addr();
	pCli->Init.Write("lastAddr=%#lx, nextAddr=%#lx, len=%d\r\n", lastAddr, nextAddr, nextAddr-lastAddr);

	int16_t ret = VAST_OK;
	uint8_t *p = NULL;
	uint32_t len = 5000;

//	p = (uint8_t *)vast_malloc(len);
	p = (uint8_t *)malloc(len+2);

	if(p != NULL)
	{
		for (uint8_t i=0; i<1; i++)
		{
			memset((void *)p, 0x30, len);
			ret = vast_store_write(0, (uint8_t *)p, len);

			memset((void *)p, 0x31, len);
			ret = vast_store_write(1, (uint8_t *)p, len+1);

			memset((void *)p, 0x32, len);
			ret = vast_store_write(2, (uint8_t *)p, len+2);

			if(ret != VAST_OK)
			{
				pCli->Init.Write("%s, ret=%d\r\n", __FUNCTION__, ret);
			}
		}
//		vast_free(p);
		free(p);
	}
	else
	{
		printf("p == NULL\r\n");
	}
}

/**
  * @brief  cliCmdStorePrint
  * @param
  * @retval
  */

void cliCmdStorePrint(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint8_t type = 0, isDetail = 0, isPrintHex = 0;

	type = str2u32(argv[1]);
	isDetail = str2u32(argv[2]);
	isPrintHex = str2u32(argv[3]);
	vast_store_print(type, isDetail, isPrintHex);
}

/**
  * @brief  cliCmdStorePrint
  * @param
  * @retval
  */

void cliCmdStoreReset(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	vast_store_reset();
}

/**
  * @brief  CLICmd_StoreCtrl
  * @param
  * @retval
  */
const CLICmdTypedef CLICmd_StoreCtrl[] =
{
	{"i", "i:info", cliCmdStoreInfo, 0},
	{"p", "p:print type isDetail isPrintHex", cliCmdStorePrint, 0},
	{"R", "R:reset", cliCmdStoreReset, 0},
	{"w", "w:write type data [size]", cliCmdStoreWrite, 0},
	{"r", "r:read type isDetail isPrintHex", cliCmdStoreRead, 0},
	// last command must be all 0s.
	{0, 0, 0, 0}
};
CLI_CMD_EXPORT(store, "store dir", 0, CLICmd_StoreCtrl);

#endif
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



