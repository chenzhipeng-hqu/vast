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
#include <string.h>

#include "core/core.h"
#include "core/utils.h"

#include "core/object.h"

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

/***********************************************
                   function
***********************************************/
/**
  * @brief
  * @param  
  * @retval 
  */
object_t *object_find(list_t *list, const char *name)
{
    struct list_head *iter;

    list_for_each(iter, list)
    {
        struct object *o = container_of(iter, struct object, entry);
        if (!strcmp(o->name, name))
            return o;
    }
    return NULL;
}

/**
  * @brief
  * @param  
  * @retval 
  */
int16_t object_attach(list_t *list, object_t *o, const char *name)
{
    int16_t ret = -1;
    OS_CPU_SR cpu_sr = 0;

	if(NULL == object_find(list, name))
	{
	    o->name = name;
		ENTER_CRITICAL();
	    list_add_tail(&o->entry, list);
	    EXIT_CRITICAL();
	    ret = 0;
	}

	return ret;
}

/**
  * @brief
  * @param  
  * @retval 
  */
void object_detach(list_t *list, struct object *o)
{
    OS_CPU_SR cpu_sr = 0;

    ENTER_CRITICAL();
    list_del(&o->entry);
    EXIT_CRITICAL();
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



