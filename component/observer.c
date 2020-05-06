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
#include "component/observer.h"

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
  * @brief  xxx.
  * @param
  * @retval
  */
static error_t attach_observer(observer_t *observer, object_t *object)
{
    error_t ret = 0;
    assert(NULL != observer && NULL != object);

    observer->list[observer->number++] = object;
    object->list[object->number++] = observer;

    return ret;
}

/**
  * @brief  xxx.
  * @param
  * @retval
  */
static error_t detach_observer(observer_t *observer, object_t *object)
{
    error_t ret = 0;
    uint8_t i=0;

    assert(NULL != observer && NULL != object);
    assert(observer->number > 0);
    assert(object->number > 0);

    //object->detach(observer);
    //memset(observer, 0, sizeof(observer_t));
    
    //observer
    observer->number--;
    for (i=0; i<observer->number; i++) {
        if (observer->list[i] == object) {
            break;
        }
    }
    //printf("name:%s, i=%d\r\n", object->name, i);
    for (; i<observer->number; i++) {
        observer->list[i] = observer->list[i+1];
    }

    //object
    object->number--;
    for (i=0; i<object->number; i++) {
        if (object->list[i] == observer) {
            break;
        }
    }
    for (; i<object->number; i++) {
        object->list[i] = object->list[i+1];
    }
    return ret;
}

/**
  * @brief  xxx.
  * @param
  * @retval
  */
error_t bind_observer_to_object(observer_t *observer, object_t *object)
{
    error_t ret = 0;
    assert(NULL != observer && NULL != object);

    observer->list[observer->number++] = object;
    object->list[object->number++] = observer;
    //object->attach(observer);
    
#if 0
    uint8_t i=0;
    printf("observer[%d]:%s: ", observer->number, observer->name);
    for (i=0; i<observer->number; i++) {
        printf("  %s", observer->list[i]->name);
    }
    printf("\r\n");

    printf("object[%d]:%s: ", object->number, object->name);
    for (uint8_t i=0; i<object->number; i++) {
        printf("  %s", object->list[i]->name);
    }
    printf("\r\n");
#endif

    return ret;
}

/**
  * @brief  xxx.
  * @param
  * @retval
  */
error_t unbind_observer_from_object(observer_t *observer, object_t *object)
{
    error_t ret = 0;
    uint8_t i=0;

    assert(NULL != observer && NULL != object);
    assert(observer->number > 0);
    assert(object->number > 0);

    //object->detach(observer);
    //memset(observer, 0, sizeof(observer_t));
    
    //observer
    observer->number--;
    for (i=0; i<observer->number; i++) {
        if (observer->list[i] == object) {
            break;
        }
    }
    //printf("name:%s, i=%d\r\n", object->name, i);
    for (; i<observer->number; i++) {
        observer->list[i] = observer->list[i+1];
    }

    //object
    object->number--;
    for (i=0; i<object->number; i++) {
        if (object->list[i] == observer) {
            break;
        }
    }
    for (; i<object->number; i++) {
        object->list[i] = object->list[i+1];
    }

#if 0
    printf("observer[%d]:%s: ", observer->number, observer->name);
    for (uint8_t i=0; i<observer->number; i++) {
        printf("  %s", observer->list[i]->name);
    }
    printf("\r\n");

    printf("object[%d]:%s: ", object->number, object->name);
    for (uint8_t i=0; i<object->number; i++) {
        printf("  %s", object->list[i]->name);
    }
    printf("\r\n");
#endif

    return ret;
}

/**
  * @brief  xxx.
  * @param
  * @retval
  */
error_t notify_observers(const object_t *object)
{
    error_t ret = 0;
    observer_t *observer;

    assert(NULL != object);

    for(int index = 0; index < object->number; index++)
    {
        observer = object->list[index];
        observer->update(observer, object);
    }

    return ret;
}

/**
  * @brief  xxx.
  * @param
  * @retval
  */
error_t object_register(object_t *object)
{
    error_t ret = 0;

    object->attach = attach_observer;
    object->detach = detach_observer;
    object->notify = notify_observers;

    return ret;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



