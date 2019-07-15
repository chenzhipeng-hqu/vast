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
#ifndef __VAST_COMMON_H__
#define __VAST_COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "vast_log.h"

/***********************************************
                    define
***********************************************/
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

#define VAST_IS_BIT_SET(REG, BIT)       (((REG) & (BIT)) == (BIT))
#define VAST_IS_BIT_CLR(REG, BIT)       (((REG) & (BIT)) == 0U)

#define SET_BIT(REG, BIT)     			((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   			((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)   			((REG) & (BIT))

#define CLEAR_REG(REG)        			((REG) = (0x0))

#define WRITE_REG(REG, VAL)   			((REG) = (VAL))

#define READ_REG(REG)         			((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     			(__CLZ(__RBIT(VAL)))

#define __VAST_LOCK(__HANDLE__)      	do{                                        \
										  if((__HANDLE__)->lock == VAST_LOCKED)   \
										  {                                      \
											 return (int)VAST_BUSY;                    \
										  }                                      \
										  else                                   \
										  {                                      \
											 (__HANDLE__)->lock = (int)VAST_LOCKED;    \
										  }                                      \
										}while (0U)

#define __VAST_UNLOCK(__HANDLE__)     	do{                                       \
											(__HANDLE__)->lock = (int)VAST_UNLOCKED;    \
										}while (0U)

/***********************************************
                    typedef
***********************************************/
 /**
   * @brief  VAST Lock structures definition
   */
 typedef enum
 {
 	VAST_UNLOCKED 	= 0x00U,
	VAST_LOCKED 	= 0x01U
 }Vast_LockTypedef;

 /**
   * @brief  HAL Status structures definition
   */
 typedef enum
 {
	VAST_OK   		= 0x00U,
	VAST_ERROR    	= 0x01U,
	VAST_BUSY     	= 0x02U,
	VAST_TIMEOUT  	= 0x03U
 } Vast_StatusTypeDef;

/***********************************************
               function prototypes
***********************************************/

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /*__VAST_COMMON_H__*/
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


