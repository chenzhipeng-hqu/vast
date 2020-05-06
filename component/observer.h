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
#ifndef __OBSERVER_H__
#define __OBSERVER_H__


#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "vast_common.h"
#include "core/list.h"
#include "core/types.h"

/***********************************************
                    define
***********************************************/
#define     MAX_OBSERVER_NUMBER     128
#define     MAX_OBJECT_NUMBER       128

/***********************************************
                    typedef
***********************************************/
typedef struct _observer observer_t;

typedef struct _object
{
    error_t (*notify)(const struct _object* object);
    error_t (*attach)(observer_t *observer, struct _object *object);
    error_t (*detach)(observer_t *observer, struct _object *object);
    uint8_t name[16];

    observer_t *list[MAX_OBSERVER_NUMBER];
    int number;

    //list_t **list;
} object_t;

typedef struct _observer
{
    error_t (*update)(observer_t *observer, const object_t *object);
    uint8_t name[16];

    const object_t *list[MAX_OBJECT_NUMBER];
    int number;

    //list_t **list;

} observer_t;

/***********************************************
               function prototypes
***********************************************/
error_t object_register(object_t *object);
error_t bind_observer_to_object(observer_t *observer, object_t *object);
error_t unbind_observer_from_object(observer_t *observer, object_t *object);
error_t notify_observers(const object_t *object);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/


#ifdef __cplusplus
}
#endif

#endif /* __OBSERVER_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

