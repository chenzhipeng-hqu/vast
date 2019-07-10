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
#ifndef __VAST_SIMULATITE_EEPROM_H__
#define __VAST_SIMULATITE_EEPROM_H__

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
 /* EEPROM emulation firmware error codes */
#define EE_OK      (uint32_t)VAST_OK
#define EE_ERROR   (uint32_t)VAST_ERROR
#define EE_BUSY    (uint32_t)VAST_BUSY
#define EE_TIMEOUT (uint32_t)VAST_TIMEOUT

 /* Define the size of the sectors to be used */
 #define PAGE_SIZE               (uint32_t)0x4000  /* Page size = 16KByte */

 /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word  */
 #define VOLTAGE_RANGE           (uint8_t)VOLTAGE_RANGE_3

 /* EEPROM start address in Flash */
 #define EEPROM_START_ADDRESS  ((uint32_t)0x08008000) /* EEPROM emulation start address:
                                                   from sector2 : after 16KByte of used
                                                   Flash memory */

 /* Pages 0 and 1 base and end addresses */
 #define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
 #define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))
 #define PAGE0_ID               FLASH_SECTOR_2

 #define PAGE1_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x4000))
 #define PAGE1_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))
 #define PAGE1_ID               FLASH_SECTOR_3

 /* Used Flash pages for EEPROM emulation */
 #define PAGE0                 ((uint16_t)0x0000)
 #define PAGE1                 ((uint16_t)0x0001) /* Page nb between PAGE0_BASE_ADDRESS & PAGE1_BASE_ADDRESS*/

 /* No valid page define */
 #define NO_VALID_PAGE         ((uint16_t)0x00AB)

 /* Page status definitions */
 #define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
 #define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
 #define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */

 /* Valid pages in read and write defines */
 #define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
 #define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

 /* Page full define */
 #define PAGE_FULL             ((uint8_t)0x80)

 /* Variables' number */
 //#define NB_OF_VAR             sizeof(VirtTab)/sizeof(VirtTab[0])


/***********************************************
                    typedef
***********************************************/
 typedef struct
 {
 	const uint16_t addr;
 	const char  *help;
 }VirtTab_TypeDef;

 typedef enum
 {
 	BOARD_TYPE,
 	JUMP_ADDR,
 	NODE_ID,
 	BOX_ID_INC,
 	IWDG_RESET_TIMES,

 	NB_OF_VAR
 }NB_OF_VAR_e;

/***********************************************
               function prototypes
***********************************************/
 uint16_t EE_Init(void);
 uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
 uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/
//extern uint16_t VirtAddVarTab[NB_OF_VAR];
extern const VirtTab_TypeDef VirtTab[];


#ifdef __cplusplus
}
#endif

#endif /* __VAST_SIMULATITE_EEPROM_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
