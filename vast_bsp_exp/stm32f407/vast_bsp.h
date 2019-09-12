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
#ifndef __VAST_BSP_H__
#define __VAST_BSP_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "stm32f407xx.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/
 typedef struct _gpio_list_t
 {
	GPIO_TypeDef 	*GPIOx;
	uint32_t 		PinMask;
	uint8_t 		_default;
 }gpio_list_t;

 typedef struct _dma_list_t
 {
    DMA_TypeDef  	*DMAx;
	uint8_t 		chx;
	IRQn_Type		irq;
 }dma_list_t;

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

#endif /* __VAST_BSP_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


