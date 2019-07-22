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
#ifndef __VAST_RING_FLASH_H__
#define __VAST_RING_FLASH_H__

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

 /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word  */
 #define VOLTAGE_RANGE           (uint8_t)VOLTAGE_RANGE_3

 /* EEPROM start address in Flash */
 #define RING_FLASH_START_ADDRESS  ((uint32_t)0x08004000) /* EEPROM emulation start address:
                                                   from sector2 : after 16KByte of used
                                                   Flash memory */

 /* Pages 0 and 1 base and end addresses */
 #define PAGE_BASE_ADDRESS    ((uint32_t)(RING_FLASH_START_ADDRESS + 0x0000))
 #define PAGE_END_ADDRESS     ((uint32_t)(RING_FLASH_START_ADDRESS + (PAGE_SIZE - 1)))
 #define PAGE_ID               FLASH_SECTOR_1

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/
 int16_t vast_ring_flash_initialize(void);
 uint32_t vast_ring_flash_store(uint8_t *dat, uint32_t len);
 uint32_t vast_ring_flash_read(uint8_t *dat, uint32_t size);
 uint32_t vast_ring_flash_get_next_addr(void);
 uint32_t vast_ring_flash_get_last_addr(void);
 int16_t vast_ring_flash_print(void);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __VAST_RING_FLASH_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


