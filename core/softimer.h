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
#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "core/object.h"
#include <sys/time.h>

/***********************************************
                    define
***********************************************/
/*
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)     (((long)((b) - (a)) < 0))
#define time_before(a,b)    time_after(b,a)

#define time_after_eq(a,b)  (((long)((a) - (b)) >= 0))
#define time_before_eq(a,b) time_after_eq(b,a)

/*
 * Calculate whether a is in the range of [b, c].
 */
#define time_in_range(a,b,c) \
    (time_after_eq(a,b) && \
     time_before_eq(a,c))

/*
 * Have the 32 bit jiffies value wrap 5 minutes after boot
 * so jiffies wrap bugs show up earlier.
 */
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (0))


/***********************************************
                    typedef
***********************************************/
typedef struct softimer
{
    object_t            obj;
    time_t              expires;
    uint32_t  			data;
    void (*cb)(struct softimer *st);
}softimer_t;


/***********************************************
               function prototypes
***********************************************/
void softimer_add      ( struct softimer *st);
void softimer_del      ( struct softimer *st);
void softimer_mod      ( struct softimer *st, time_t expires);
void softimer_schedule ( void);
list_t *timer_lists    ( void);


/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __SOFTTIMER_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

