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
#include <stdlib.h>
#include <string.h>
#include "vast_core/utils.h"
#include <vast_core/queue.h>

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
  * @brief  vast_ring_queue_init.
  * @param
  * @retval
  */
int vast_ring_queue_init(RingQueueTypedef *rq, int length, unsigned char *pData)
{
	rq->head = rq->tail = 0;

	if(length > 0)
	{	
		rq->length = length;
		if (pData != NULL)
		{
			rq->data = pData;
		}
		else
		{
			rq->data = (char *)malloc(sizeof(char)*(rq->length));
		}
		
		if(rq->data == NULL)
		{
			return -1;
		}
	}

	return 0;
}

/**
  * @brief  vast_ring_queue_pop.
  * @param
  * @retval
  */
//void *vast_ring_queue_pop(RingQueueTypedef *rq, char *pop_buf, int size)
int vast_ring_queue_pop(RingQueueTypedef *rq, char *pop_buf, int size)
{
	int size_cnt = 0;
	
	__VAST_LOCK(rq);

	if(rq->head == rq->tail)  // ring queue is not empty
	{
		__VAST_UNLOCK(rq);
		memset((void *)pop_buf, 0x00, size);
		return 0;
	}
	else
	{	
		if( size>0 )
		{
			for(size_cnt=0; size_cnt<size; size_cnt++)
			{				
				if(rq->head != rq->tail)
				{
					*(pop_buf+size_cnt) = *((char *)rq->data + rq->head);
					//*(rq->data + rq->head) = 0;
					rq->head = (rq->head+1)%rq->length;
				}	
				else
				{
					memset(pop_buf, 0, size);
					break;
				}					
			}		
		}
	}
	
	__VAST_UNLOCK(rq);
	
	return size_cnt;
}

/**
  * @brief  vast_ring_queue_find_block.
  * @param
  * @retval
  */
int vast_ring_queue_find_block(RingQueueTypedef *rq, char *pop_buf, int start_index, int end_index)
{
	int size_cnt = 0;

	__VAST_LOCK(rq);
	
	if( (start_index > rq->length) & (end_index > rq->length) & (end_index-start_index == 1))  // ring queue is not empty
	{
//		memset((void *)pop_buf, 0x00, end_addr-start_addr);
		__VAST_UNLOCK(rq);
		return 0;
	}
	else
	{	
			
		while(start_index != end_index)
		{
			*(pop_buf+(size_cnt++)) = *((char *)rq->data + start_index);
			start_index = (start_index+1)%rq->length;
		}	
								
	}
	
	__VAST_UNLOCK(rq);
	
	return size_cnt; 
}

/**
  * @brief  vast_ring_queue_push.
  * @param
  * @retval
  */
int vast_ring_queue_push(RingQueueTypedef *rq, char *push_buf, int size)
{
	int size_cnt = 0;

	__VAST_LOCK(rq);
	
	if(0)
	{
		__VAST_UNLOCK(rq);
		return -1;
	}
	else
	{
		if( (size>0) & (push_buf != NULL))
		{
			for(size_cnt=0; size_cnt<size; size_cnt++)
			{				
				*((char *)rq->data + rq->tail) = *(push_buf+size_cnt);
				rq->tail = (rq->tail+1)%rq->length;
			}
			
//			while(size != 0)
//			{				
//				size--;
//				rq->data[rq->tail+size] = (char )(push_buf[size]);
//			}	
//			rq->tail += size_bak;
		}
	}
	
	__VAST_UNLOCK(rq);
	
	return 0;
	
}

#if 0
/**
  * @brief  vast_ring_queue_add_size.
  * @param
  * @retval
  */
void *vast_ring_queue_add_size(RingQueueTypedef *rq, int size)
{
	__VAST_LOCK(rq);
	
	if(0)
	{
		return NULL;
	}
	else
	{
		rq->tail = (rq->tail+size)%rq->length;
	}
	
	__VAST_UNLOCK(rq);
	
	return 0;
	
}
#endif

/**
  * @brief  vast_ring_queue_refresh_tail.
  * @param
  * @retval
  */
int vast_ring_queue_refresh_tail(RingQueueTypedef *rq, int tail)
{
	__VAST_LOCK(rq);
	
	if(0)  //�ж�ͷβ�Ƿ��ص�
	{
		__VAST_UNLOCK(rq);
		return 1;
	}
	else
	{
		rq->tail = (tail)%rq->length;
	}
	
	__VAST_UNLOCK(rq);
	
	return 0;
}

/**
  * @brief  vast_ring_queue_empty.
  * @param
  * @retval
  */
int vast_ring_queue_empty(RingQueueTypedef *rq)
{
	if (rq->head == rq->tail)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/
