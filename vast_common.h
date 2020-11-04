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
#include <stdarg.h>
//#include "cmsis_gcc.h"
#include <assert.h>
#include "core/utils.h"
#include "core/init.h"
//#include "vast_config.h"
 //#include "vast_log.h"

/***********************************************
                    define
***********************************************/
#ifdef configUSING_RTOS
	#include <cmsis_gcc.h>
	#include <core/types.h>
	#define enable_irq()      __enable_irq()
	#define disable_irq()     __disable_irq()

	#define portBYTE_ALIGNMENT      (4)
	#define portBYTE_ALIGNMENT_MASK (0x03)

	#define OS_CPU_SR   uint32_t
	#define enter_critical()        \
		do { cpu_sr = __get_PRIMASK(); disable_irq();} while (0)
	#define exit_critical()         \
		do { __set_PRIMASK(cpu_sr);} while (0)

	#define portDISABLE_INTERRUPTS __disable_irq
#else
//	#include "os_dummy"
	#define OS_CPU_SR   		uint32_t
	#define enter_critical() 	(void)(cpu_sr)
	#define exit_critical()		(void)(cpu_sr)
#endif

/***********************************************
                    typedef
***********************************************/

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


