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
#ifndef __VAST_RING_QUEUE_H__
#define __VAST_RING_QUEUE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "vast_core/types.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

 typedef struct {
	size_t 	head;
 	size_t 	tail;
 	size_t 	length;
 	lock_t 	lock;
 	char 	type;
 	char 	name[15];
 	void 	*data;
 }RingQueueTypedef;

/***********************************************
               function prototypes
***********************************************/

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/


extern int 	vast_ring_queue_init		(RingQueueTypedef *rq, int length, unsigned char *pData);
extern int 	vast_ring_queue_pop			(RingQueueTypedef *rq, char *pop_buf, int size);
extern int  vast_ring_queue_push		(RingQueueTypedef *rq, char *push_buf, int size);
extern void *vast_ring_queue_add_size	(RingQueueTypedef *rq, int size);
extern int 	vast_ring_queue_refresh_tail(RingQueueTypedef *rq, int tail);
extern int 	vast_ring_queue_find_block	(RingQueueTypedef *rq, char *pop_buf, int start_index, int end_index);
extern int	vast_ring_queue_empty		(RingQueueTypedef *rq);

#ifdef __cplusplus
}
#endif

#endif /*__VAST_RING_QUEUE_H__*/
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/
