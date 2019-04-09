

/*
	假设一次传输的数据不会超过设置的buff

*/

#include "ring_queue.h"
#include <stdlib.h>
#include <string.h>

int ring_queue_init(RingQueueTypedef *rq, int length, unsigned char *pData)
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
			//rq->data = (char *)malloc(sizeof(char)*(rq->length));
		}
		
		if(rq->data == NULL)
		{
			return NULL;
		}
	}
	return 0;
}

void *ring_queue_pop(RingQueueTypedef *rq, char *pop_buf, int size)
{
	int size_cnt = 0;
	rq->lock = RING_QUEUE_LOCKED;
	
	if(rq->head == rq->tail)  // ring queue is not empty
	{
		memset((void *)pop_buf, 0x00, size);
		return NULL;
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
	
	rq->lock = RING_QUEUE_UNLOCKED;
	
	return pop_buf; 
}
int ring_queue_find_block(RingQueueTypedef *rq, char *pop_buf, int start_index, int end_index)
{
	int size_cnt = 0;
	rq->lock = RING_QUEUE_LOCKED;
	
	if( (start_index > rq->length) & (end_index > rq->length) & (end_index-start_index == 1))  // ring queue is not empty
	{
//		memset((void *)pop_buf, 0x00, end_addr-start_addr);
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
	
	rq->lock = RING_QUEUE_UNLOCKED;
	
	return size_cnt; 
}

void *ring_queue_push(RingQueueTypedef *rq, char *push_buf, int size)
{
	int size_cnt = 0;
	rq->lock = RING_QUEUE_LOCKED;
	
	if(0)
	{
		return NULL;
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
	
	rq->lock = RING_QUEUE_UNLOCKED;
	
	return 0;
	
}

void *ring_queue_add_size(RingQueueTypedef *rq, int size)
{
	rq->lock = RING_QUEUE_LOCKED;
	
	if(0)
	{
		return NULL;
	}
	else
	{
		rq->tail = (rq->tail+size)%rq->length;
	}
	
	rq->lock = RING_QUEUE_UNLOCKED;
	
	return 0;
	
}

int ring_queue_refresh_tail(RingQueueTypedef *rq, int tail)
{
	rq->lock = RING_QUEUE_LOCKED;
	
	if(0)  //判断头尾是否重叠
	{
		return 1;
	}
	else
	{
		rq->tail = (tail)%rq->length;
	}
	
	rq->lock = RING_QUEUE_UNLOCKED;
	
	return 0;
	
}
int ring_queue_empty(RingQueueTypedef *rq)
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

