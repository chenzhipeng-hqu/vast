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
#include "core/core.h"
#include "core/utils.h"
#include "core/softimer.h"

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
static LIST_HEAD(timer_list);

/***********************************************
                   function
***********************************************/
/**
  * @brief
  * @param  
  * @retval 
  */
void softimer_add(struct softimer *st)
{
    OS_CPU_SR cpu_sr;

    ENTER_CRITICAL();

	//if ((st->cb) && (!(st->entry.next && st->entry.prev)))
	if (st->cb)
        object_attach(&timer_list, &st->obj, st->obj.name);

    EXIT_CRITICAL();
}

/**
  * @brief
  * @param  
  * @retval 
  */
void softimer_del(struct softimer *st)
{
    OS_CPU_SR cpu_sr;

    ENTER_CRITICAL();
    object_detach(NULL, &st->obj);
    EXIT_CRITICAL();
}

/**
  * @brief
  * @param  
  * @retval 
  */
void softimer_mod(struct softimer *st, time_t expires)
{
    OS_CPU_SR cpu_sr;

    ENTER_CRITICAL();
    st->expires = expires;
	if (st->cb)
        object_attach(&timer_list, &st->obj, st->obj.name);
    EXIT_CRITICAL();
}

/**
  * @brief
  * @param  
  * @retval 
  */
void softimer_schedule(void)
{
    struct softimer *iter = NULL;
    object_t *n = NULL, *cur_obj = NULL;

	list_for_each_entry_safe(cur_obj, n, &timer_list, entry)
	{
        iter = cur_obj;
		if (time_after_eq(GET_TIME_TICK(), iter->expires))
		{
			softimer_del(iter);

            uint32_t start = GET_TIME_TICK();

			iter->cb(iter);

            if (GET_TIME_TICK() - start > 10)
				printf("timer %s spent %ldms\r\n",
						iter->obj.name, (uint32_t)(GET_TIME_TICK()-start));
		}
	}
}

/**
  * @brief
  * @param  
  * @retval 
  */
list_t *timer_lists(void)
{
    return &timer_list;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

