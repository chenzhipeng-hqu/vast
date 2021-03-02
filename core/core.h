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
#ifndef __CORE_H__
#define __CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "core/types.h"

/***********************************************
                    define
***********************************************/
#define     OS_CPU_SR               uint32_t
#define     ENTER_CRITICAL()        \
                        do { cpu_sr = core_enter_critical();} while (0)
#define     EXIT_CRITICAL()         \
		                do { core_exit_critical(cpu_sr);} while (0)

#define     GET_TIME_TICK()          core_tick_get()

/***********************************************
                    typedef
***********************************************/
typedef struct core_ops {
    OS_CPU_SR             ( *enter_critical) ( void);
    void                  ( *exit_critical)  ( OS_CPU_SR);
    int16_t               ( *tick_init)      ( struct core*);
    //uint32_t            ( *get_tick)       ( void);
} core_ops_t;

typedef struct core {
    core_ops_t  *ops;
    uint32_t    jiffies;
} core_t;

/***********************************************
               function prototypes
***********************************************/
int16_t core_init             ( core_ops_t *ops);
OS_CPU_SR core_enter_critical ( void);
void core_exit_critical       ( OS_CPU_SR);
uint32_t core_tick_get        ( void);
void core_tick_inc            ( void);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __CORE_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


