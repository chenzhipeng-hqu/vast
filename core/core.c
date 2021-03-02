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
#include "core.h"

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
static core_t core = {0};

/***********************************************
                   function
***********************************************/
/**
  * @brief
  * @param  
  * @retval 
  */
int16_t core_init(core_ops_t *ops) 
{
    core.ops = ops;
    core.ops->tick_init(&core);

    return 0;
}

/**
  * @brief
  * @param  
  * @retval 
  */
OS_CPU_SR core_enter_critical(void) 
{
    return core.ops->enter_critical();
}

/**
  * @brief
  * @param  
  * @retval 
  */
void core_exit_critical(OS_CPU_SR cpu_sr) 
{
    return core.ops->exit_critical(cpu_sr);
}

/**
  * @brief
  * @param  
  * @retval 
  */
uint32_t core_tick_get(void) 
{
    return core.jiffies;
}

/**
  * @brief
  * @param  
  * @retval 
  */
void core_tick_inc(void) 
{
    core.jiffies++;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



