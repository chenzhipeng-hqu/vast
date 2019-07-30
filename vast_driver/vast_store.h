/**
 ******************************************************************************
 * @file    xxx.h
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

/* Define to prevent recursive inclusion */
#ifndef __VAST_STORE_H__
#define __VAST_STORE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "stm32f4xx_hal.h"
#include "vast_common.h"

/***********************************************
                    define
***********************************************/
 /* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)0x4000  /* Page size = 16KByte */
#define PAGE_NUM               	(uint32_t)0x01  /* Page size = 16KByte */
#define BLOCK_SIZE              (uint32_t)PAGE_SIZE * PAGE_NUM /* block size = 16KByte */
#define	PAGE_NUM_ONE_BLOCK		(BLOCK_SIZE/PAGE_SIZE)

/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
be done by word  */
#define VOLTAGE_RANGE           (uint8_t)VOLTAGE_RANGE_3

 /* Pages 0 and 1 base and end addresses */
#define BLOCK0_START_ADDRESS    ((uint32_t)(0x08004000))
#define BLOCK0_END_ADDRESS     	((uint32_t)(BLOCK0_START_ADDRESS + (BLOCK_SIZE - 1)))
#define BLOCK0_START_ID         FLASH_SECTOR_1

#define BLOCK1_START_ADDRESS    ((uint32_t)(0x08008000))
#define BLOCK1_END_ADDRESS     	((uint32_t)(BLOCK1_START_ADDRESS + (BLOCK_SIZE - 1)))
#define BLOCK1_START_ID         FLASH_SECTOR_2

#define	NO_VALID_BLCOK			(0U)
#define	BLOCK0					(1U)
#define	BLOCK1					(2U)

#define BLOCK(x)              	(x)

#define 	VALID_BLOCK     	(0x00U)
#define 	BACKUP_BLOCK    	(0x01U)
#define 	EMPTY_BLOCK     	(0xFFU)

#define 	EMPTY_ADDR     		(0xFFFFFU)

 /*BLOCK*/
#define __STORE_GET_STATUS(x)                   (uint8_t)(((x) >> 28) & 0x0F)
#define __STORE_GET_ERASE_TIMES(x)              (uint32_t)((x) & 0xFFFFFF)

 /*data0*/
#define __STORE_GET_TYPE(x)                    	(uint8_t)(((x) >> 28) & 0x0F)
#define __STORE_GET_IDL(x)               		(uint32_t)(((x) >> 20) & 0xFF)
#define __STORE_GET_NEXT_ADDR(x)               	(uint32_t)(((x)) & 0x0FFFFF)
 /*data1*/
#define __STORE_GET_IDH(x)                    	(uint32_t)(((x) >> 20) & 0x0FFF)
#define __STORE_GET_LAST_ADDR(x)                (uint32_t)((x) & 0xFFFFF)
 /*data2*/
#define __STORE_GET_CRC(x)                		(uint16_t)(((x) >> 16) & 0xFFFF)
#define __STORE_GET_LEN(x)                		(uint16_t)((x) & 0xFFFF)

 /*BLOCK*/
#define __STORE_SET_STATUS(x)                   (uint8_t)(((x) & 0xF) << 28)     //
#define __STORE_SET_ERASE_TIMES(x)              (uint32_t)(((x) & 0xFFFFFF))   //

 /*data0*/
#define __STORE_SET_TYPE(x)                    	(uint32_t)(((x) & 0x0F) << 28)     //
#define __STORE_SET_IDL(x)               		(uint32_t)(((x) & 0xFF) << 20)     //
#define __STORE_SET_NEXT_ADDR(x)               	(uint32_t)(((x) & 0xFFFFF) << 0)   //
 /*data1*/
#define __STORE_SET_IDH(x)                    	(uint32_t)(((x) & 0xFFF00) << (20-8))     //
#define __STORE_SET_LAST_ADDR(x)                (uint32_t)(((x) & 0xFFFFF) << 0)   //
 /*data2*/
#define __STORE_SET_CRC(x)                		(uint32_t)(((x) & 0xFFFF) << 16)   //
#define __STORE_SET_LEN(x)                		(uint32_t)(((x) & 0xFFFF) << 0)   //

/***********************************************
                    typedef
***********************************************/
 typedef struct
 {
 	uint32_t id;
 	const char  *help;
 }Store_DataTypeDef;

 typedef enum
 {
	 STORE_TYPE_LOG,
	 STORE_TYPE_CONF,
	 STORE_TYPE_SCRIPT,

 	STORE_TYPE_MAX
 }Store_TypeTypeDef;

/***********************************************
               function prototypes
***********************************************/
int16_t vast_store_initialize	(void);
int32_t vast_store_write		(Store_TypeTypeDef type, uint8_t *dat, uint32_t len);
int32_t vast_store_read			(Store_TypeTypeDef type, uint8_t *dat, uint32_t len);
int16_t vast_store_print		(Store_TypeTypeDef type, uint8_t isDetail, uint8_t isPrintHex);
uint32_t vast_store_get_next_addr(void);
uint32_t vast_store_get_last_addr(void);
int16_t vast_store_reset		(void);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/
extern Store_DataTypeDef StoreDataTable[];

#ifdef __cplusplus
}
#endif

#endif /* __VAST_STORE_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


