

#ifndef __RING_QUEUE_H__
#define __RING_QUEUE_H__


typedef enum {
	RING_QUEUE_UNLOCKED,
	RING_QUEUE_LOCKED,
}QueueLock;

typedef struct {
	int head;
	int tail;
	int length;
	QueueLock lock;
	char queue_type;
	char queue_name[15];
	void *data;
}RingQueueTypedef;


extern int ring_queue_init(RingQueueTypedef *rq, int length, unsigned char *pData);
extern void *ring_queue_pop(RingQueueTypedef *rq, char *pop_buf, int size);
extern void *ring_queue_push(RingQueueTypedef *rq, char *push_buf, int size);
extern void *ring_queue_add_size(RingQueueTypedef *rq, int size);
extern int ring_queue_refresh_tail(RingQueueTypedef *rq, int tail);
extern int ring_queue_find_block(RingQueueTypedef *rq, char *pop_buf, int start_index, int end_index);
extern int ring_queue_empty(RingQueueTypedef *rq);

#endif

