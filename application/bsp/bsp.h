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
//#include "vast_config.h"
//#include "stm32f407xx.h"

/***********************************************
                    define
***********************************************/
#define STM32_SRAM_SIZE        (128)
#define STM32_SRAM_END         (0x20000000 + STM32_SRAM_SIZE * 1024)

#if defined(__CC_ARM) || defined(__CLANG_ARM)
    extern int Image$$RW_IRAM1$$ZI$$Limit;
    #define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
    #pragma section="CSTACK"
    #define HEAP_BEGIN      (__segment_end("CSTACK"))
#else
    extern int _sstack;
    extern int _estack;
    extern int __bss_start__;
    extern int __bss_end__;
    extern int _Min_Heap_Size;
    extern int _Min_Stack_Size;
    #define HEAP_BEGIN      (((long)&__bss_end__))
#endif
#define HEAP_END        (((long)&_estack) - ((long)&_Min_Stack_Size))

//static uint8_t heap_buff[1024*5] = {0};

#ifdef configUSING_RTOS
	#include <cmsis_gcc.h>
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
extern int vast_bsp_init(void);
extern int vast_uart_bsp_init(void);


/***********************************************
	      		    inline
***********************************************/


/***********************************************
                   variable
***********************************************/
extern const char SW_VERSION[];
extern const char HW_VERSION[];
extern const char MANUFACTURER[];

extern const char buildDate[];
extern const char buildTime[];

#ifdef __cplusplus
}
#endif

#endif /* __VAST_BSP_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


