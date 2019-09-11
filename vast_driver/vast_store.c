/**
 ******************************************************************************
 * @file    vast_store.c
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    28-Jul-2019
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
#include <stdarg.h>
#include "vast_core/utils.h"
#include "vast_store.h"
#include "vast_driver/vast_log.h"

/***********************************************
                    define
***********************************************/
#define 	VALID_BLOCK     	(0x8U)
#define 	RECEIVE_BLOCK    	(0xCU)
#define 	BACKUP_BLOCK    	(0xEU)
#define 	EMPTY_BLOCK     	(0xFU)

#define 	VAST_STORE_DBG_LOG(level, fmt, ...) 	do{											\
														if(level <= hstore.dbgLevel){			\
															DBG_PRINT(level, fmt, __VA_ARGS__);	\
														}										\
													}while(0)

/***********************************************
                    typedef
***********************************************/
typedef struct{
	uint8_t	 dbgLevel;
	uint32_t nextAddrVaild;
	uint32_t lastAddrVaild;
	uint32_t nextAddrBackUp;
	uint32_t lastAddrBackUp;
	uint32_t id[STORE_TYPE_MAX];
}Store_HandleTypedef;

/***********************************************
               function prototypes
***********************************************/
/*static int16_t vast_store_clear					(void);
static int16_t vast_store_erase					(uint8_t blockNum);
static int16_t vast_store_writeByte				(uint32_t addr, uint8_t data);
static void vast_store_readByte					(uint32_t addr, uint8_t *pData);
static int16_t vast_store_writeWord				(uint32_t addr, uint32_t data);
static void vast_store_readWord					(uint32_t addr, uint32_t *pData);
static int16_t vast_store_findValidBlock		(void);
static int32_t vast_store_findNextAddr			(void);
static int16_t vast_store_writeToBackupBlock	(void);*/

/***********************************************
                   variable
***********************************************/
static Store_HandleTypedef hstore = {0};

/***********************************************
                   function
***********************************************/
/**
  * @brief  vast_store_readByte.
  * @param
  * @retval
  */
static inline uint8_t vast_store_readByte (uint32_t addr)
{
	return (uint8_t)(*(volatile uint32_t *)addr);
}

/**
  * @brief  vast_store_readWord.
  * @param
  * @retval
  */
static inline uint32_t vast_store_readWord (uint32_t addr)
{
	return (uint32_t)(*(volatile uint32_t *)addr);
}

/**
  * @brief  vast_store_readBuff.
  * @param
  * @retval
  */
static inline void vast_store_readBuff (uint32_t addr, uint8_t *pData, uint32_t len)
{
	uint32_t i = 0;

	for (i=0; i<len; i++)
	{
		pData[i] = (uint8_t)(*(volatile uint32_t *)(addr+i));
	}
}

/**
  * @brief  vast_store_writeByte.
  * @param
  * @retval
  */
static inline int16_t vast_store_writeByte (uint32_t addr, uint8_t pData)
{
	HAL_StatusTypeDef ret = HAL_OK;

	HAL_FLASH_Unlock();
	ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, (uint8_t)(pData));
	HAL_FLASH_Lock();

	return (int16_t)ret;
}

/**
  * @brief  vast_store_writeWord.
  * @param
  * @retval
  */
static inline int16_t vast_store_writeWord (uint32_t addr, uint32_t data)
{
	HAL_StatusTypeDef ret = HAL_OK;

	HAL_FLASH_Unlock();
	ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, addr, (uint32_t)(data));
	HAL_FLASH_Lock();

	return (int16_t)ret;
}

/**
  * @brief  vast_store_writeBuff.
  * @param
  * @retval
  */
static int32_t vast_store_writeBuff (uint32_t addr, uint8_t *pData, uint32_t len)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t writeCnt = 0;

	HAL_FLASH_Unlock();

	for(writeCnt=0; writeCnt<(len/4); writeCnt++,addr+=4)
	{
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, (uint32_t)(*((uint32_t *)&pData[writeCnt*4])));

		if(ret != HAL_OK)
		{
			break;
		}
	}

	//DBG_LOG(DBG_DEBUG, "4. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);

	for(writeCnt=writeCnt*4; writeCnt<len; writeCnt++,addr++)
	{
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, pData[writeCnt]);

		if(ret != HAL_OK)
		{
			break;
		}
	}

	HAL_FLASH_Lock();

	if(ret != HAL_OK)
	{
		writeCnt = 0;
	}

	return (int32_t)writeCnt;
}

/**
  * @brief  vast_store_erase.
  * @param
  * @retval
  */
static int16_t vast_store_erase (uint32_t blockAddr)
{
	int16_t ret = VAST_OK;
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError = 0;
	uint8_t blockId = 0;

	HAL_FLASH_Unlock();

	if(blockAddr == BLOCK0_START_ADDRESS)
	{
		blockId = BLOCK0_START_ID;
	}
	else if(blockAddr == BLOCK1_START_ADDRESS)
	{
		blockId = BLOCK1_START_ID;
	}
	else
	{
		HAL_FLASH_Lock();
		return VAST_ERROR;
	}

	pEraseInit.TypeErase = TYPEERASE_SECTORS;
	pEraseInit.Sector = blockId;
	pEraseInit.NbSectors = PAGE_NUM;
	pEraseInit.VoltageRange = VOLTAGE_RANGE;

	ret = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

	HAL_FLASH_Lock();

	return ret;
}

/**
  * @brief  vast_store_clear.
  * @param blockAddr: BLOCK0_START_ADDRESS/BLOCK1_START_ADDRESS
  * @param blockStatus: VALID_BLOCK/BACKUP_BLOCK/EMPTY_BLOCK
  * @retval
  */
static int16_t vast_store_setBlockStatus (uint32_t blockAddr, uint8_t blockStatus)
{
	//uint8_t status = 0;
	uint32_t erase_times = 0, value = 0;
	int16_t ret = VAST_OK;

	value = vast_store_readWord(blockAddr);

	//status = __STORE_GET_STATUS(value);
	erase_times = __STORE_GET_ERASE_TIMES(value);

	ret = vast_store_erase(blockAddr);

	erase_times++;
	ret = vast_store_writeWord (blockAddr, __STORE_SET_STATUS(blockStatus) | __STORE_SET_ERASE_TIMES(erase_times));

	return ret;
}

/**
  * @brief  vast_store_findValidBlock.
  * @param
  * @retval
  */
static int16_t vast_store_findValidBlock (uint32_t *pValidAddr, uint32_t *pBackupAddr)
{
	int16_t ret = VAST_OK;
	uint8_t block0Status = 0, block1Status = 0;

	block0Status = __STORE_GET_STATUS(vast_store_readWord(BLOCK0_START_ADDRESS));
	block1Status = __STORE_GET_STATUS(vast_store_readWord(BLOCK1_START_ADDRESS));

	if (block0Status == VALID_BLOCK)
	{
		if(block1Status != BACKUP_BLOCK)
		{
			ret = vast_store_setBlockStatus (BLOCK1_START_ADDRESS, BACKUP_BLOCK);
		}

		*pValidAddr = BLOCK0_START_ADDRESS;
		*pBackupAddr = BLOCK1_START_ADDRESS;
	}
	else if ((block1Status == VALID_BLOCK))
	{
		if(block0Status != BACKUP_BLOCK)
		{
			ret = vast_store_setBlockStatus (BLOCK0_START_ADDRESS, BACKUP_BLOCK);
		}

		*pValidAddr = BLOCK1_START_ADDRESS;
		*pBackupAddr = BLOCK0_START_ADDRESS;
	}
	else
	{
		VAST_STORE_DBG_LOG(DBG_INFO, "block0Status=%#X, block1Status=%#X \r\n", block0Status, block1Status);
		ret = vast_store_setBlockStatus (BLOCK0_START_ADDRESS, VALID_BLOCK);
		ret = vast_store_setBlockStatus (BLOCK1_START_ADDRESS, BACKUP_BLOCK);

		*pValidAddr = BLOCK0_START_ADDRESS;
		*pBackupAddr = BLOCK1_START_ADDRESS;
	}

	return ret;
}

/**
  * @brief  vast_store_findNextAddr.
  * @param
  * @retval
  */
static int16_t vast_store_findAddress (uint32_t blockAddr, uint32_t *pNextAddr, uint32_t *pLastAddr)
{
	uint32_t nextAddrOffset = 0, lastAddrOffset = 0, id = 0;
	int16_t ret = VAST_OK;
	uint8_t storeType = STORE_TYPE_LOG;

	*pLastAddr = blockAddr;
	*pNextAddr = blockAddr+4;

	nextAddrOffset = __STORE_GET_NEXT_ADDR(vast_store_readWord(blockAddr+4));

	while ((nextAddrOffset != EMPTY_ADDR))
	{
		if((nextAddrOffset < (BLOCK_SIZE)) && (nextAddrOffset > (lastAddrOffset)))
		{
			id = __STORE_GET_IDL(vast_store_readWord(*pNextAddr)) | (__STORE_GET_IDH(vast_store_readWord((*pNextAddr)+4)) << 8);
			storeType = __STORE_GET_TYPE(vast_store_readWord(*pNextAddr));

			if((storeType < STORE_TYPE_MAX) /*& (hstore.id[storeType] < id)*/)
			{
				VAST_STORE_DBG_LOG(DBG_DEBUG, "addr:%#lx, type=%d, id=%#lx, len=%d\r\n", *pNextAddr, storeType, id, *pNextAddr-(*pLastAddr)-12);
				hstore.id[storeType] = id;
			}

			lastAddrOffset = nextAddrOffset;
			*pLastAddr = *pNextAddr;
			*pNextAddr = nextAddrOffset + blockAddr;
		}
		else
		{
			VAST_STORE_DBG_LOG(DBG_WARNING, "nextAddr=%#lx\r\n", *pNextAddr);
			nextAddrOffset = nextAddrOffset + 4;
			*pNextAddr = nextAddrOffset + blockAddr;
		}

		nextAddrOffset = __STORE_GET_NEXT_ADDR(vast_store_readWord(*pNextAddr));
	}

	if((*pNextAddr&0x03) != 0)
	{
		*pNextAddr = (*pNextAddr&0xfffffffc) + 4;
	}

	VAST_STORE_DBG_LOG(DBG_DEBUG, "blockAddr=%#lx, nextAddr=%#lx, lastAddr=%#lx, len=%ld\r\n", blockAddr, *pNextAddr, *pLastAddr, (*pNextAddr)-(*pLastAddr)-12);

	return ret;
}


/**
  * @brief  vast_store_writeToBackupBlock.
  * @param
  * @retval
  */
static int16_t vast_store_writeToBlock(uint32_t blockAddr, uint32_t *pNextAddr, uint32_t *pLastAddr, uint8_t type, uint8_t *dat, uint32_t len)
{
	int16_t ret = VAST_OK;
	uint32_t nowAddr = 0, lastAddr = 0;

	lastAddr = (*pLastAddr);
	nowAddr = (*pLastAddr) = (*pNextAddr);
	(*pNextAddr) = (*pNextAddr) + len + 12;

	if(((*pNextAddr)&0x03) != 0)
	{
		(*pNextAddr) = ((*pNextAddr)&0xfffffffc) + 4;
	}

	hstore.id[type]++;
	//1. store type & idL & nextAddrOffset
	VAST_STORE_DBG_LOG(DBG_DEBUG, "nowAddr:%#x, type:%#x, idL:%#x, nextAddr:%#x\r\n", nowAddr, __STORE_SET_TYPE(type), __STORE_SET_IDL(hstore.id[type]), __STORE_SET_NEXT_ADDR((*pNextAddr)));
	vast_store_writeWord(nowAddr, __STORE_SET_TYPE(type)
											| __STORE_SET_IDL(hstore.id[type])
											|  __STORE_SET_NEXT_ADDR((((*pNextAddr)-blockAddr))));
	nowAddr = nowAddr + 4;

	//2. store idH & lastAddrOffset
	VAST_STORE_DBG_LOG(DBG_DEBUG, "idH:%#lx, lastAddr:%#x\r\n", __STORE_SET_IDH(hstore.id[type]), __STORE_SET_LAST_ADDR((lastAddr-blockAddr)));

	vast_store_writeWord(nowAddr, __STORE_SET_IDH(hstore.id[type])
											|  __STORE_SET_LAST_ADDR((lastAddr-blockAddr)));

	nowAddr = nowAddr + 4;

	//3. store crc & len
	//TODO: calculate crc16
	VAST_STORE_DBG_LOG(DBG_DEBUG, "crc16:%#x, len:%ld\r\n", __STORE_SET_CRC(0xABCD), __STORE_SET_LEN(len));

	vast_store_writeWord(nowAddr, __STORE_SET_CRC(0xABCD)
											|  __STORE_SET_LEN(len));

	nowAddr = nowAddr + 4;

	//4. write buff
	vast_store_writeBuff(nowAddr, dat, len);

	// TODO: 5. read from flash and calculate crc16

	return ret;
}
#if 0
/**
  * @brief  vast_store_writeToBackupBlock.
  * @param
  * @retval
  */
static int16_t vast_store_writeToBackupBlock(Store_TypeTypeDef type, uint8_t *dat, uint32_t len)
{
	int16_t ret = VAST_OK;
	uint8_t storeTypeCnt = STORE_TYPE_LOG, tmpType, isNeedChangeBlockStatus = 0;
	uint32_t i = 0, lenAllType = 0, nowAddr, tmpNowAddrValid, lastAddrBackup = 0, tmpLen;
	uint16_t crc16[STORE_TYPE_MAX] = {0};
	uint32_t lastAddrValid[STORE_TYPE_MAX] = {0}, aLen[STORE_TYPE_MAX] = {0};
	uint32_t validAddr = 0xFFFFF, backupAddr = 0xFFFFF;

	vast_store_findValidBlock(&validAddr, &backupAddr);

	tmpNowAddrValid = hstore.lastAddrVaild;

	do {
		tmpType = __STORE_GET_TYPE(vast_store_readWord(tmpNowAddrValid));
		tmpLen = __STORE_GET_LEN(vast_store_readWord(tmpNowAddrValid + 8));

		if ( (tmpLen < (BLOCK_SIZE/STORE_TYPE_MAX)) && (type != tmpType) && (lastAddrValid[tmpType] == 0))
		{
			lastAddrValid[tmpType] = tmpNowAddrValid;

			// 2. read crc len
			crc16[tmpType] = __STORE_GET_CRC(vast_store_readWord(tmpNowAddrValid + 8));
			aLen[tmpType] = tmpLen;

			lenAllType += aLen[tmpType];

			storeTypeCnt++;
		}

		tmpNowAddrValid = __STORE_GET_LAST_ADDR(vast_store_readWord(tmpNowAddrValid + 4));

		if( tmpNowAddrValid < 4)
		{
			break;
		}
		else
		{
			tmpNowAddrValid = tmpNowAddrValid + validAddr;
		}

		//tmpNowAddrValid = tmpLastAddrValid;

	}while ((storeTypeCnt != (STORE_TYPE_MAX-1)) & ((tmpNowAddrValid-validAddr) >= 4) );

	if ((hstore.nextAddrBackUp-backupAddr) + lenAllType >= BLOCK_SIZE)
	{
		VAST_STORE_DBG_LOG(DBG_INFO, "backupBlock:%#lX + %ld >= %#lX! exchange store block\r\n", hstore.nextAddrBackUp, lenAllType, BLOCK_SIZE);
		ret = vast_store_setBlockStatus (backupAddr, VALID_BLOCK);
		vast_store_findAddress(backupAddr, &hstore.nextAddrBackUp, &hstore.lastAddrBackUp);

		// need to change valid block and backup block
		isNeedChangeBlockStatus = 1;
	}

	for (storeTypeCnt=STORE_TYPE_LOG; storeTypeCnt<STORE_TYPE_MAX; storeTypeCnt++)
	{
		if (lastAddrValid[storeTypeCnt] != 0)
		{
			// 3. store type id crc len data to backup block
			lastAddrBackup = hstore.lastAddrBackUp;
			nowAddr = hstore.lastAddrBackUp = hstore.nextAddrBackUp;
			hstore.nextAddrBackUp = hstore.nextAddrBackUp + aLen[storeTypeCnt] + 12;

			if((hstore.nextAddrBackUp&0x03) != 0)
			{
				hstore.nextAddrBackUp = (hstore.nextAddrBackUp&0xfffffffc) + 4;
			}

			//store type & idL & nextAddrOffset
			vast_store_writeWord(nowAddr, __STORE_SET_TYPE(storeTypeCnt)
													| __STORE_SET_IDL(hstore.id[storeTypeCnt])
													|  __STORE_SET_NEXT_ADDR((hstore.nextAddrBackUp-backupAddr)));
			nowAddr = nowAddr + 4;

			//store idH & lastAddrOffset
			vast_store_writeWord(nowAddr, __STORE_SET_IDH(hstore.id[storeTypeCnt])
													|  __STORE_SET_LAST_ADDR((lastAddrBackup-backupAddr)));
			nowAddr = nowAddr + 4;

			//store crc & len
			vast_store_writeWord(nowAddr, __STORE_SET_CRC(crc16[storeTypeCnt])
													|  __STORE_SET_LEN(aLen[storeTypeCnt]));

			if( (aLen[storeTypeCnt] <= 0) | (__STORE_SET_LEN(aLen[storeTypeCnt]) > (BLOCK_SIZE/STORE_TYPE_MAX)))
			{
				VAST_STORE_DBG_LOG(DBG_ERROR, "backupBlockAddr:%#lx, len:%ld\r\n", backupAddr, __STORE_SET_LEN(aLen[storeTypeCnt]));
			}

			nowAddr += 4;

			// 4. store data to backup block
			for (i=0; i<aLen[storeTypeCnt]; i+=4)
			{
				vast_store_writeWord(nowAddr+i, vast_store_readWord(lastAddrValid[storeTypeCnt]+i+12));
			}
		}
		else if(type == storeTypeCnt)
		{
			vast_store_writeToBlock(backupAddr, &(hstore.nextAddrBackUp), &(hstore.lastAddrBackUp), type, dat, len);
		}
	}

	// 4. erase valid block
	if(isNeedChangeBlockStatus == 1)
	{
		ret = vast_store_setBlockStatus (validAddr, BACKUP_BLOCK);
		vast_store_findAddress(validAddr, &hstore.nextAddrVaild, &hstore.lastAddrVaild);

		__VAST_SWAP(hstore.lastAddrBackUp, hstore.lastAddrVaild);
		__VAST_SWAP(hstore.nextAddrBackUp, hstore.nextAddrVaild);
	}
	else
	{
		ret = vast_store_setBlockStatus (validAddr, VALID_BLOCK);
		vast_store_findAddress(validAddr, &hstore.nextAddrVaild, &hstore.lastAddrVaild);
	}

	return ret;
}
#endif
/**
  * @brief  vast_ring_flash_initialize.
  * @param
  * @retval
  */
int16_t vast_store_initialize(void)
{
	int16_t ret = VAST_OK;
	uint32_t validAddr = 0xFFFFF, backupAddr = 0xFFFFF;

	hstore.dbgLevel = DBG_DEBUG;

#if 1
	vast_store_findValidBlock(&validAddr, &backupAddr);

	VAST_STORE_DBG_LOG(DBG_DEBUG, "validBlockAddr:%#lx, erase:%#lx\r\n", validAddr, __STORE_GET_ERASE_TIMES(vast_store_readWord(validAddr)));
	VAST_STORE_DBG_LOG(DBG_DEBUG, "backupBlockAddr:%#lx, erase:%#lx\r\n", backupAddr, __STORE_GET_ERASE_TIMES(vast_store_readWord(backupAddr)));

	vast_store_findAddress(validAddr, &hstore.nextAddrVaild, &hstore.lastAddrVaild);

	vast_store_findAddress(backupAddr, &hstore.nextAddrBackUp, &hstore.lastAddrBackUp);

	VAST_STORE_DBG_LOG(DBG_INFO, "max size for store per data is %d bytes\r\n", BLOCK_SIZE/STORE_TYPE_MAX);
#else
	uint8_t block0Status = 0, block1Status = 0;
	uint32_t erase_times = 0, value = 0;

	block0Status = __STORE_GET_STATUS(vast_store_readWord(BLOCK0_START_ADDRESS));
	block1Status = __STORE_GET_STATUS(vast_store_readWord(BLOCK1_START_ADDRESS));

	if( (block0Status == VALID_BLOCK) & (block1Status == BACKUP_BLOCK)){
		ret = vast_store_findAddress(BLOCK0_START_ADDRESS, &hstore.nextAddrVaild, &hstore.lastAddrVaild);

	}else if( (block0Status == BACKUP_BLOCK) & (block1Status == VALID_BLOCK)){

	}else {
		ret = vast_store_setBlockStatus(BLOCK0_START_ADDRESS, VALID_BLOCK);
		ret = vast_store_setBlockStatus(BLOCK1_START_ADDRESS, BACKUP_BLOCK);
		ret = vast_store_findAddress(BLOCK0_START_ADDRESS, &hstore.nextAddrVaild, &hstore.lastAddrVaild);
	}

	ret = vast_store_findAddress(validAddr, &hstore.nextAddrVaild, &hstore.lastAddrVaild);

	switch(block0Status)
	{
		case EMPTY_BLOCK:
		{
			ret = vast_store_verifyFullyErased(BLOCK0_START_ADDRESS);

			if(VAST_OK != ret)
			{
				ret = vast_store_erase(BLOCK0_START_ADDRESS);
			}

			if(block1Status == VALID_BLOCK)
			{
				// find nextAddrVaild
				vast_store_findAddress(BLOCK1_START_ADDRESS, &hstore.nextAddrVaild, &hstore.lastAddrVaild);
			}
			else
			{
				ret = vast_store_setBlockStatus(BLOCK1_START_ADDRESS, VALID_BLOCK);
			}
			break;
		}
		case VALID_BLOCK:
			break;
		case RECEIVE_BLOCK:
			break;
		default:
			break;
	}
#endif
    return ret;
}

static int16_t vast_store_swapBlock(Store_TypeTypeDef type, uint8_t *dat, uint32_t len)
{
	int16_t ret = VAST_OK;
	uint8_t storeTypeCnt = STORE_TYPE_LOG, tmpType = 0;
	uint32_t id, i = 0, nowAddr, tmpNowAddrValid, lastAddrBackup = 0, tmpLen, nextAddrBackUp, lastAddrBackUp;
	uint16_t crc16 = 0;
	uint32_t erase_times = 0, value = 0;
	uint32_t validAddr = 0xFFFFF, backupAddr = 0xFFFFF;

	vast_store_findValidBlock(&validAddr, &backupAddr);

	lastAddrBackUp = backupAddr;
	nextAddrBackUp = backupAddr + 4;
	vast_store_writeToBlock(backupAddr, &nextAddrBackUp, &lastAddrBackUp, type, dat, len);

	tmpNowAddrValid = hstore.lastAddrVaild;

	do {
		tmpType = __STORE_GET_TYPE(vast_store_readWord(tmpNowAddrValid));
		id = __STORE_GET_IDL(vast_store_readWord(tmpNowAddrValid)) | (__STORE_GET_IDH(vast_store_readWord((tmpNowAddrValid)+4)) << 8);

		if ( (type != tmpType) && (hstore.id[tmpType] == id))
		{
			tmpLen = __STORE_GET_LEN(vast_store_readWord(tmpNowAddrValid + 8));

			// 3. store type id crc len data to backup block
			lastAddrBackup = lastAddrBackUp;
			nowAddr = lastAddrBackUp = nextAddrBackUp;
			nextAddrBackUp = nextAddrBackUp + tmpLen + 12;

			if((nextAddrBackUp&0x03) != 0)
			{
				nextAddrBackUp = (nextAddrBackUp&0xfffffffc) + 4;
			}

			//store type & idL & nextAddrOffset
			vast_store_writeWord(nowAddr, __STORE_SET_TYPE(tmpType)
													| __STORE_SET_IDL(hstore.id[tmpType])
													|  __STORE_SET_NEXT_ADDR((nextAddrBackUp-backupAddr)));
			nowAddr = nowAddr + 4;

			//store idH & lastAddrOffset
			vast_store_writeWord(nowAddr, __STORE_SET_IDH(hstore.id[tmpType])
													|  __STORE_SET_LAST_ADDR((lastAddrBackup-backupAddr)));
			nowAddr = nowAddr + 4;

			//store crc & len
			crc16 = __STORE_GET_CRC(vast_store_readWord(tmpNowAddrValid + 8));
			vast_store_writeWord(nowAddr, __STORE_SET_CRC(crc16)
													|  __STORE_SET_LEN(tmpLen));

			if( (tmpLen <= 0) | (__STORE_SET_LEN(tmpLen) > (BLOCK_SIZE/STORE_TYPE_MAX)))
			{
				VAST_STORE_DBG_LOG(DBG_ERROR, "backupBlockAddr:%#lx, len:%ld\r\n", backupAddr, __STORE_SET_LEN(tmpLen));
			}

			nowAddr += 4;

			// 4. store data to backup block
			for (i=0; i<tmpLen; i+=4)
			{
				vast_store_writeWord(nowAddr+i, vast_store_readWord(tmpNowAddrValid+i+12));
			}

			storeTypeCnt++;
		}

		tmpNowAddrValid = __STORE_GET_LAST_ADDR(vast_store_readWord(tmpNowAddrValid + 4));

		if( tmpNowAddrValid < 4)
		{
			break;
		}
		else
		{
			tmpNowAddrValid = tmpNowAddrValid + validAddr;
		}

	}while ((storeTypeCnt != (STORE_TYPE_MAX-1)) & ((tmpNowAddrValid-validAddr) >= 4) );

	value = vast_store_readWord(backupAddr);
	erase_times = __STORE_GET_ERASE_TIMES(value);
	ret = vast_store_writeWord (backupAddr, __STORE_SET_STATUS(VALID_BLOCK) | __STORE_SET_ERASE_TIMES(erase_times));

	vast_store_setBlockStatus(validAddr, BACKUP_BLOCK);

	__VAST_SWAP(hstore.nextAddrVaild, nextAddrBackUp);
	__VAST_SWAP(hstore.lastAddrVaild, lastAddrBackUp);

	return ret;
}

/**
  * @brief  vast_ring_flash_store.
  * @param
  * @retval
  */
int32_t vast_store_write(Store_TypeTypeDef type, uint8_t *dat, uint32_t len)
{
	int16_t ret = VAST_OK;
	uint32_t validAddr = 0xFFFFF, backupAddr = 0xFFFFF;

	ASSERT(len < (BLOCK_SIZE/STORE_TYPE_MAX),
			VAST_STORE_DBG_LOG(DBG_INFO, "len=%ld, max=%ld\r\n", len, BLOCK_SIZE/STORE_TYPE_MAX);
			return VAST_ERROR);

	vast_store_findValidBlock(&validAddr, &backupAddr);

	//DBG_LOG(DBG_DEBUG, "1. nowAddr=%#lx, nextAddr=%#lx \r\n", nowAddr, nextAddr);

	DBG_LOG(DBG_DEBUG, "\r\n");

	if ((hstore.nextAddrVaild-validAddr) + len >= BLOCK_SIZE)
	{
		VAST_STORE_DBG_LOG(DBG_INFO, "validBlock:%#lX + %ld >= %#lX! store to backup block\r\n", hstore.nextAddrVaild, len, BLOCK_SIZE);

#if 0
		vast_store_writeToBackupBlock(type, dat, len);
#else
		vast_store_swapBlock(type, dat, len);
#endif
	}
	else
	{
		vast_store_writeToBlock(validAddr, &(hstore.nextAddrVaild), &(hstore.lastAddrVaild), type, dat, len);
	}

    return ret;
}

/**
  * @brief  vast_ring_flash_read.
  * @param
  * @retval
  */
int32_t vast_store_read(Store_TypeTypeDef type, uint8_t *dat, uint32_t size)
{
	uint32_t len = 0, readLen = 0, lastAddr = 0;
	uint8_t type_tmp = STORE_TYPE_MAX;
	uint32_t validAddr = 0xFFFFF, backupAddr = 0xFFFFF;

	//len = hstore.nextAddrVaild - hstore.lastAddrVaild - 12;

	ASSERT(((size > 0) & (type < STORE_TYPE_MAX)),
		DBG_LOG(DBG_ERROR, "nextAddr=%#lx, lastAddr=%#lx, type=%d, size=%d\r\n", hstore.nextAddrVaild, hstore.lastAddrVaild, type, size);
		return 0
			);

	if(hstore.id[type] <= 0)
	{
		return 0;
	}

	vast_store_findValidBlock(&validAddr, &backupAddr);

	// 1. read lastAddr & type
	if(hstore.lastAddrVaild != hstore.nextAddrVaild)
	{
		type_tmp = __STORE_GET_TYPE(vast_store_readWord(hstore.lastAddrVaild));
		lastAddr = hstore.lastAddrVaild;
	}
	else
	{
		type_tmp = __STORE_GET_TYPE(vast_store_readWord(hstore.lastAddrBackUp));
		lastAddr = hstore.lastAddrBackUp;
		validAddr = backupAddr;
	}

	while(type_tmp != type)
	{
		lastAddr = __STORE_GET_LAST_ADDR(vast_store_readWord(lastAddr + 4)) + validAddr;
		type_tmp = __STORE_GET_TYPE(vast_store_readWord(lastAddr));

		if((lastAddr == validAddr+4) & (type_tmp != type)) {
			validAddr = backupAddr;
			lastAddr = hstore.lastAddrBackUp;
			type_tmp = __STORE_GET_TYPE(vast_store_readWord(lastAddr));
		}
	}

	// 2. read len
	len = __STORE_GET_LEN(vast_store_readWord(lastAddr + 8));

	readLen = ((len) > size) ? size : (len);

	// 3. read buff
	vast_store_readBuff(lastAddr + 12, dat, readLen);

	return readLen;
}

/**
  * @brief  vast_store_print.
  * @param
  * @retval
  */
int16_t vast_store_print(Store_TypeTypeDef type, uint8_t isDetail, uint8_t isPrintHex)
{
	uint8_t type_tmp = 0;
	int16_t ret = VAST_OK;
	uint16_t crc16 = 0;
	uint32_t u32_nowAddr = 0, u32_lastAddr;
	uint32_t len = 0;
	uint32_t i = 0, id = 0;
	uint32_t addr[2] = {0xFFFFF, 0xFFFFF};

	vast_store_findValidBlock(&addr[0], &addr[1]);

	VAST_STORE_DBG_LOG(DBG_DEBUG, "validBlockAddr:%#lx, erase:%#lx\r\n", addr[0], __STORE_GET_ERASE_TIMES(vast_store_readWord(addr[0])));
	VAST_STORE_DBG_LOG(DBG_DEBUG, "backupBlockAddr:%#lx, erase:%#lx\r\n", addr[1], __STORE_GET_ERASE_TIMES(vast_store_readWord(addr[1])));

	for(uint8_t addrCnt=0; addrCnt<2; addrCnt++)
	{
		u32_nowAddr = u32_lastAddr = addr[addrCnt] + 4;

		u32_nowAddr = __STORE_GET_NEXT_ADDR(vast_store_readWord(u32_lastAddr));

		while ((u32_nowAddr != EMPTY_ADDR))
		{
			// 2. read len
			len = __STORE_GET_LEN(vast_store_readWord(u32_lastAddr + 8));
			type_tmp = __STORE_GET_TYPE(vast_store_readWord(u32_lastAddr));
			crc16 = __STORE_GET_CRC(vast_store_readWord(u32_lastAddr + 8));
			id = __STORE_GET_IDL(vast_store_readWord(u32_lastAddr)) | (__STORE_GET_IDH(vast_store_readWord(u32_lastAddr+4)) << 8);

			if((type == type_tmp) | (type >= STORE_TYPE_MAX))
			{
				DBG_INFO(DBG_DEBUG, "[%#lx](type:%d, id=%#05lx, len=%06d, crc16=%#x): ", u32_lastAddr, type_tmp, id, len, crc16);

				if(1 == isDetail)
				{
					for(i=0; i<len; i++)
					{
						DBG_INFO(DBG_DEBUG, "%c", vast_store_readByte(u32_lastAddr+i+12));
					}

					DBG_INFO(DBG_DEBUG, "->(");

					if (1 == isPrintHex)
					{
						for(i=0; i<len; i++)
						{
							DBG_INFO(DBG_DEBUG, "%02x ", vast_store_readByte(u32_lastAddr+i+12));
						}
					}
					DBG_INFO(DBG_DEBUG, ")");
				}
				DBG_INFO(DBG_DEBUG, "\r\n");
			}

			u32_lastAddr = u32_nowAddr + addr[addrCnt];
			u32_nowAddr = __STORE_GET_NEXT_ADDR(vast_store_readWord(u32_lastAddr));
		}
	}
	return ret;
}

/**
  * @brief  vast_ring_flash_get_next_addr.
  * @param
  * @retval
  */
uint32_t vast_store_get_next_addr(void)
{
	return hstore.nextAddrVaild;
}

/**
  * @brief  vast_ring_flash_get_last_addr.
  * @param
  * @retval
  */
uint32_t vast_store_get_last_addr(void)
{
	return hstore.lastAddrVaild;
}

/**
  * @brief  vast_ring_flash_get_last_addr.
  * @param
  * @retval
  */
int16_t vast_store_reset(void)
{
	int16_t ret = VAST_OK;

	ret = vast_store_setBlockStatus (BLOCK0_START_ADDRESS, VALID_BLOCK);
	ret = vast_store_setBlockStatus (BLOCK1_START_ADDRESS, BACKUP_BLOCK);

	vast_store_initialize();

	return ret;
}

/*
int main(int argc, char *argv[])
{
	int ret = vast_store_initialize();

	if(ret != VAST_OK){
		printf("%s,%d, ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	}

	uint8_t buff_array[32] = {0}, read_len = 0;
	read_len = vast_store_write(STORE_TYPE_LOG, buff_array, sizeof(buff_array));

	DBG_ARRAY(DBG_DEBUG, "buff_array: ", "\r\n", (char *)buff_array, read_len);

	uint8_t readBuff[10] = {0};
	ret = vast_store_read(STORE_TYPE_LOG, readBuff, sizeof(readBuff));

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



