/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * File      : memheap.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-04-10     Bernard      first implementation
 * 2012-10-16     Bernard      add the mutex lock for heap object.
 * 2012-12-29     Bernard      memheap can be used as system heap.
 *                             change mutex lock to semaphore lock.
 * 2013-04-10     Bernard      add vast_memheap_realloc function.
 * 2013-05-24     Bernard      fix the vast_memheap_realloc issue.
 * 2013-07-11     Grissiom     fix the memory block splitting issue.
 * 2013-07-15     Grissiom     optimize vast_memheap_realloc
 */

/*#include <rthw.h>*/
/*#include <rtthread.h>*/
#include "vast_common.h"
#include <core/types.h>
#include "core/device.h"
#include "core/list.h"

#define LOG_TAG "heap"
#include "elog.h"

#undef log_d
#define log_d(...)

#ifdef VAST_USING_MEMHEAP
/**
 * memory item on the heap
 */
struct vast_memheap_item
{
    uint32_t             magic;                      /**< magic number for memheap */
    struct vast_memheap      *pool_ptr;                   /**< point of pool */

    struct vast_memheap_item *next;                       /**< next memheap item */
    struct vast_memheap_item *prev;                       /**< prev memheap item */

    struct vast_memheap_item *next_free;                  /**< next free memheap item */
    struct vast_memheap_item *prev_free;                  /**< prev free memheap item */
};

/**
 * Base structure of memory heap object
 */
struct vast_memheap
{
    struct object        parent;                     /**< inherit from vast_object */

    void                   *start_addr;                 /**< pool start address and size */

    uint32_t             pool_size;                  /**< pool size */
    uint32_t             available_size;             /**< available size */
    uint32_t             max_used_size;              /**< maximum allocated size */

    struct vast_memheap_item *block_list;                 /**< used block list */

    struct vast_memheap_item *free_list;                  /**< free block list */
    struct vast_memheap_item  free_header;                /**< free block list header */

    //struct vast_semaphore     lock;                       [>*< semaphore lock <]
    uint8_t         lock;
};
#endif

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define VAST_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define VAST_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def RT_NULL
 * Similar as the \c NULL in C library.
 */
#define VAST_NULL                         (0)

#define VAST_ALIGN_SIZE             4

#define     vast_sem_init(lock, name, val, flag)  
#define     vast_sem_take(lock, tick)             0
#define     vast_sem_release(lock)                
#define     vast_set_errno(ret)                

void vast_memheap_free(void *ptr);

#ifdef VAST_USING_MEMHEAP

/* dynamic pool magic and mask */
#define VAST_MEMHEAP_MAGIC        0x1ea01ea0
#define VAST_MEMHEAP_MASK         0xfffffffe
#define VAST_MEMHEAP_USED         0x01
#define VAST_MEMHEAP_FREED        0x00

#define VAST_MEMHEAP_IS_USED(i)   ((i)->magic & VAST_MEMHEAP_USED)
#define VAST_MEMHEAP_MINIALLOC    12

#define VAST_MEMHEAP_SIZE         VAST_ALIGN(sizeof(struct vast_memheap_item), VAST_ALIGN_SIZE)
#define MEMITEM_SIZE(item)      ((ubase_t)item->next - (ubase_t)item - VAST_MEMHEAP_SIZE)

/*
 * The initialized memory pool will be:
 * +-----------------------------------+--------------------------+
 * | whole freed memory block          | Used Memory Block Tailer |
 * +-----------------------------------+--------------------------+
 *
 * block_list --> whole freed memory block
 *
 * The length of Used Memory Block Tailer is 0,
 * which is prevents block merging across list
 */
error_t vast_memheap_init(struct vast_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         size_t         size)
{
    struct vast_memheap_item *item;

    assert(memheap != NULL);

    /* initialize pool object */
    //object_init(&(memheap->parent), VAST_Object_Class_MemHeap, name);
    object_attach(&(memheap->parent), name);

    memheap->start_addr     = start_addr;
    memheap->pool_size      = VAST_ALIGN_DOWN(size, VAST_ALIGN_SIZE);
    memheap->available_size = memheap->pool_size - (2 * VAST_MEMHEAP_SIZE);
    memheap->max_used_size  = memheap->pool_size - memheap->available_size;

    /* initialize the free list header */
    item            = &(memheap->free_header);
    item->magic     = VAST_MEMHEAP_MAGIC;
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    /* set the free list to free list header */
    memheap->free_list = item;

    /* initialize the first big memory block */
    item            = (struct vast_memheap_item *)start_addr;
    item->magic     = VAST_MEMHEAP_MAGIC;
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    item->next = (struct vast_memheap_item *)
                 ((uint8_t *)item + memheap->available_size + VAST_MEMHEAP_SIZE);
    item->prev = item->next;

    /* block list header */
    memheap->block_list = item;

    /* place the big memory block to free list */
    item->next_free = memheap->free_list->next_free;
    item->prev_free = memheap->free_list;
    memheap->free_list->next_free->prev_free = item;
    memheap->free_list->next_free            = item;

    /* move to the end of memory pool to build a small tailer block,
     * which prevents block merging
     */
    item = item->next;
    /* it's a used memory block */
    item->magic     = VAST_MEMHEAP_MAGIC | VAST_MEMHEAP_USED;
    item->pool_ptr  = memheap;
    item->next      = (struct vast_memheap_item *)start_addr;
    item->prev      = (struct vast_memheap_item *)start_addr;
    /* not in free list */
    item->next_free = item->prev_free = NULL;

    /* initialize semaphore lock */
    vast_sem_init(&(memheap->lock), name, 1, VAST_IPC_FLAG_FIFO);

    log_d("memory heap: start addr 0x%08x, size %d, free list header 0x%08x",
                  start_addr, size, &(memheap->free_header));

    return VAST_OK;
}
//VASTM_EXPOVAST(vast_memheap_init);

error_t vast_memheap_detach(struct vast_memheap *heap)
{
    assert(heap);
    //assert(vast_object_get_type(&heap->parent) == VAST_Object_Class_MemHeap);
    //assert(vast_object_is_systemobject(&heap->parent));

    //object_detach(&(heap->lock.parent.parent));
    object_detach(&(heap->parent));

    /* Return a successful completion. */
    return VAST_OK;
}
//VASTM_EXPOVAST(vast_memheap_detach);

void *vast_memheap_alloc(struct vast_memheap *heap, size_t size)
{
    error_t result = VAST_OK;
    uint32_t free_size;
    struct vast_memheap_item *header_ptr;

    assert(heap != NULL);
    //assert(vast_object_get_type(&heap->parent) == VAST_Object_Class_MemHeap);

    /* align allocated size */
    size = VAST_ALIGN(size, VAST_ALIGN_SIZE);
    if (size < VAST_MEMHEAP_MINIALLOC)
        size = VAST_MEMHEAP_MINIALLOC;

    log_d("allocate %d on heap:%s", size, heap->parent.name);

    if (size < heap->available_size)
    {
        /* search on free list */
        free_size = 0;

        /* lock memheap */
        result = vast_sem_take(&(heap->lock), VAST_WAITING_FOREVER);
        if (result != VAST_OK)
        {
            vast_set_errno(result);
            return NULL;
        }

        /* get the first free memory block */
        header_ptr = heap->free_list->next_free;
        while (header_ptr != heap->free_list && free_size < size)
        {
            /* get current freed memory block size */
            free_size = MEMITEM_SIZE(header_ptr);
            if (free_size < size)
            {
                /* move to next free memory block */
                header_ptr = header_ptr->next_free;
            }
        }

        /* determine if the memory is available. */
        if (free_size >= size)
        {
            /* a block that satisfies the request has been found. */

            /* determine if the block needs to be split. */
            if (free_size >= (size + VAST_MEMHEAP_SIZE + VAST_MEMHEAP_MINIALLOC))
            {
                struct vast_memheap_item *new_ptr;

                /* split the block. */
                new_ptr = (struct vast_memheap_item *)
                          (((uint8_t *)header_ptr) + size + VAST_MEMHEAP_SIZE);

                log_d("split: block[0x%08x] nextm[0x%08x] prevm[0x%08x] to new[0x%08x]",
                              header_ptr,
                              header_ptr->next,
                              header_ptr->prev,
                              new_ptr);

                /* mark the new block as a memory block and freed. */
                new_ptr->magic = VAST_MEMHEAP_MAGIC;

                /* put the pool pointer into the new block. */
                new_ptr->pool_ptr = heap;

                /* break down the block list */
                new_ptr->prev          = header_ptr;
                new_ptr->next          = header_ptr->next;
                header_ptr->next->prev = new_ptr;
                header_ptr->next       = new_ptr;

                /* remove header ptr from free list */
                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;

                /* insert new_ptr to free list */
                new_ptr->next_free = heap->free_list->next_free;
                new_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = new_ptr;
                heap->free_list->next_free            = new_ptr;
                log_d("new ptr: next_free 0x%08x, prev_free 0x%08x",
                                                new_ptr->next_free,
                                                new_ptr->prev_free);

                /* decrement the available byte count.  */
                heap->available_size = heap->available_size -
                                       size -
                                       VAST_MEMHEAP_SIZE;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;
            }
            else
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - free_size;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                /* remove header_ptr from free list */
                log_d("one block: block[0x%08x], next_free 0x%08x, prev_free 0x%08x",
                              header_ptr,
                              header_ptr->next_free,
                              header_ptr->prev_free);

                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;
            }

            /* Mark the allocated block as not available. */
            header_ptr->magic |= VAST_MEMHEAP_USED;

            /* release lock */
            vast_sem_release(&(heap->lock));

            /* Return a memory address to the caller.  */
            log_d("alloc mem: memory[0x%08x], heap[0x%08x], size: %d",
                          (void *)((uint8_t *)header_ptr + VAST_MEMHEAP_SIZE),
                          header_ptr,
                          size);

            return (void *)((uint8_t *)header_ptr + VAST_MEMHEAP_SIZE);
        }

        /* release lock */
        vast_sem_release(&(heap->lock));
    }

    log_d("allocate memory: failed");

    /* Return the completion status.  */
    return NULL;
}
//VASTM_EXPOVAST(vast_memheap_alloc);

void *vast_memheap_realloc(struct vast_memheap *heap, void *ptr, size_t newsize)
{
    error_t result;
    size_t oldsize;
    struct vast_memheap_item *header_ptr;
    struct vast_memheap_item *new_ptr;

    assert(heap);
    //assert(vast_object_get_type(&heap->parent) == VAST_Object_Class_MemHeap);

    if (newsize == 0)
    {
        vast_memheap_free(ptr);

        return NULL;
    }
    /* align allocated size */
    newsize = VAST_ALIGN(newsize, VAST_ALIGN_SIZE);
    if (newsize < VAST_MEMHEAP_MINIALLOC)
        newsize = VAST_MEMHEAP_MINIALLOC;

    if (ptr == NULL)
    {
        return vast_memheap_alloc(heap, newsize);
    }

    /* get memory block header and get the size of memory block */
    header_ptr = (struct vast_memheap_item *)
                 ((uint8_t *)ptr - VAST_MEMHEAP_SIZE);
    oldsize = MEMITEM_SIZE(header_ptr);
    /* re-allocate memory */
    if (newsize > oldsize)
    {
        void *new_ptr;
        struct vast_memheap_item *next_ptr;

        /* lock memheap */
        result = vast_sem_take(&(heap->lock), VAST_WAITING_FOREVER);
        if (result != VAST_OK)
        {
            vast_set_errno(result);
            return NULL;
        }

        next_ptr = header_ptr->next;

        /* header_ptr should not be the tail */
        assert(next_ptr > header_ptr);

        /* check whether the following free space is enough to expand */
        if (!VAST_MEMHEAP_IS_USED(next_ptr))
        {
            int32_t nextsize;

            nextsize = MEMITEM_SIZE(next_ptr);
            assert(next_ptr > 0);

            /* Here is the ASCII art of the situation that we can make use of
             * the next free node without alloc/memcpy, |*| is the control
             * block:
             *
             *      oldsize           free node
             * |*|-----------|*|----------------------|*|
             *         newsize          >= minialloc
             * |*|----------------|*|-----------------|*|
             */
            if (nextsize + oldsize > newsize + VAST_MEMHEAP_MINIALLOC)
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - (newsize - oldsize);
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                /* remove next_ptr from free list */
                log_d("remove block: block[0x%08x], next_free 0x%08x, prev_free 0x%08x",
                              next_ptr,
                              next_ptr->next_free,
                              next_ptr->prev_free);

                next_ptr->next_free->prev_free = next_ptr->prev_free;
                next_ptr->prev_free->next_free = next_ptr->next_free;
                next_ptr->next->prev = next_ptr->prev;
                next_ptr->prev->next = next_ptr->next;

                /* build a new one on the right place */
                next_ptr = (struct vast_memheap_item *)((char *)ptr + newsize);

                log_d("new free block: block[0x%08x] nextm[0x%08x] prevm[0x%08x]",
                              next_ptr,
                              next_ptr->next,
                              next_ptr->prev);

                /* mark the new block as a memory block and freed. */
                next_ptr->magic = VAST_MEMHEAP_MAGIC;

                /* put the pool pointer into the new block. */
                next_ptr->pool_ptr = heap;

                next_ptr->prev          = header_ptr;
                next_ptr->next          = header_ptr->next;
                header_ptr->next->prev = next_ptr;
                header_ptr->next       = next_ptr;

                /* insert next_ptr to free list */
                next_ptr->next_free = heap->free_list->next_free;
                next_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = next_ptr;
                heap->free_list->next_free            = next_ptr;
                log_d("new ptr: next_free 0x%08x, prev_free 0x%08x",
                                                next_ptr->next_free,
                                                next_ptr->prev_free);

                /* release lock */
                vast_sem_release(&(heap->lock));

                return ptr;
            }
        }

        /* release lock */
        vast_sem_release(&(heap->lock));

        /* re-allocate a memory block */
        new_ptr = (void *)vast_memheap_alloc(heap, newsize);
        if (new_ptr != NULL)
        {
            memcpy(new_ptr, ptr, oldsize < newsize ? oldsize : newsize);
            vast_memheap_free(ptr);
        }

        return new_ptr;
    }

    /* don't split when there is less than one node space left */
    if (newsize + VAST_MEMHEAP_SIZE + VAST_MEMHEAP_MINIALLOC >= oldsize)
        return ptr;

    /* lock memheap */
    result = vast_sem_take(&(heap->lock), VAST_WAITING_FOREVER);
    if (result != VAST_OK)
    {
        vast_set_errno(result);

        return NULL;
    }

    /* split the block. */
    new_ptr = (struct vast_memheap_item *)
              (((uint8_t *)header_ptr) + newsize + VAST_MEMHEAP_SIZE);

    log_d("split: block[0x%08x] nextm[0x%08x] prevm[0x%08x] to new[0x%08x]",
                  header_ptr,
                  header_ptr->next,
                  header_ptr->prev,
                  new_ptr);

    /* mark the new block as a memory block and freed. */
    new_ptr->magic = VAST_MEMHEAP_MAGIC;
    /* put the pool pointer into the new block. */
    new_ptr->pool_ptr = heap;

    /* break down the block list */
    new_ptr->prev          = header_ptr;
    new_ptr->next          = header_ptr->next;
    header_ptr->next->prev = new_ptr;
    header_ptr->next       = new_ptr;

    /* determine if the block can be merged with the next neighbor. */
    if (!VAST_MEMHEAP_IS_USED(new_ptr->next))
    {
        struct vast_memheap_item *free_ptr;

        /* merge block with next neighbor. */
        free_ptr = new_ptr->next;
        heap->available_size = heap->available_size - MEMITEM_SIZE(free_ptr);

        log_d("merge: right node 0x%08x, next_free 0x%08x, prev_free 0x%08x\n",
                      header_ptr, header_ptr->next_free, header_ptr->prev_free);

        free_ptr->next->prev = new_ptr;
        new_ptr->next   = free_ptr->next;

        /* remove free ptr from free list */
        free_ptr->next_free->prev_free = free_ptr->prev_free;
        free_ptr->prev_free->next_free = free_ptr->next_free;
    }

    /* insert the split block to free list */
    new_ptr->next_free = heap->free_list->next_free;
    new_ptr->prev_free = heap->free_list;
    heap->free_list->next_free->prev_free = new_ptr;
    heap->free_list->next_free            = new_ptr;
    log_d("new free ptr: next_free 0x%08x, prev_free 0x%08x",
                                    new_ptr->next_free,
                                    new_ptr->prev_free);

    /* increment the available byte count.  */
    heap->available_size = heap->available_size + MEMITEM_SIZE(new_ptr);

    /* release lock */
    vast_sem_release(&(heap->lock));

    /* return the old memory block */
    return ptr;
}
//VASTM_EXPOVAST(vast_memheap_realloc);

void vast_memheap_free(void *ptr)
{
    error_t result = VAST_OK;
    struct vast_memheap *heap;
    struct vast_memheap_item *header_ptr, *new_ptr;
    uint32_t insevast_header;

    /* NULL check */
    if (ptr == NULL) return;

    /* set initial status as OK */
    insevast_header = 1;
    new_ptr       = NULL;
    header_ptr    = (struct vast_memheap_item *)
                    ((uint8_t *)ptr - VAST_MEMHEAP_SIZE);

    log_d("free memory: memory[0x%08x], block[0x%08x]",
                                    ptr, header_ptr);

    /* check magic */
    assert((header_ptr->magic & VAST_MEMHEAP_MASK) == VAST_MEMHEAP_MAGIC);
    assert(header_ptr->magic & VAST_MEMHEAP_USED);
    /* check whether this block of memory has been over-written. */
    assert((header_ptr->next->magic & VAST_MEMHEAP_MASK) == VAST_MEMHEAP_MAGIC);

    /* get pool ptr */
    heap = header_ptr->pool_ptr;

    assert(heap);
    //assert(vast_object_get_type(&heap->parent) == VAST_Object_Class_MemHeap);

    /* lock memheap */
    result = vast_sem_take(&(heap->lock), VAST_WAITING_FOREVER);
    if (result != VAST_OK)
    {
        vast_set_errno(result);

        return ;
    }

    /* Mark the memory as available. */
    header_ptr->magic &= ~VAST_MEMHEAP_USED;
    /* Adjust the available number of bytes. */
    heap->available_size = heap->available_size + MEMITEM_SIZE(header_ptr);

    /* Determine if the block can be merged with the previous neighbor. */
    if (!VAST_MEMHEAP_IS_USED(header_ptr->prev))
    {
        log_d("merge: left node 0x%08x",
                                        header_ptr->prev);

        /* adjust the available number of bytes. */
        heap->available_size = heap->available_size + VAST_MEMHEAP_SIZE;

        /* yes, merge block with previous neighbor. */
        (header_ptr->prev)->next = header_ptr->next;
        (header_ptr->next)->prev = header_ptr->prev;

        /* move header pointer to previous. */
        header_ptr = header_ptr->prev;
        /* don't insert header to free list */
        insevast_header = 0;
    }

    /* determine if the block can be merged with the next neighbor. */
    if (!VAST_MEMHEAP_IS_USED(header_ptr->next))
    {
        /* adjust the available number of bytes. */
        heap->available_size = heap->available_size + VAST_MEMHEAP_SIZE;

        /* merge block with next neighbor. */
        new_ptr = header_ptr->next;

        log_d("merge: right node 0x%08x, next_free 0x%08x, prev_free 0x%08x",
                      new_ptr, new_ptr->next_free, new_ptr->prev_free);

        new_ptr->next->prev = header_ptr;
        header_ptr->next    = new_ptr->next;

        /* remove new ptr from free list */
        new_ptr->next_free->prev_free = new_ptr->prev_free;
        new_ptr->prev_free->next_free = new_ptr->next_free;
    }

    if (insevast_header)
    {
        /* no left merge, insert to free list */
        header_ptr->next_free = heap->free_list->next_free;
        header_ptr->prev_free = heap->free_list;
        heap->free_list->next_free->prev_free = header_ptr;
        heap->free_list->next_free            = header_ptr;

        log_d("insert to free list: next_free 0x%08x, prev_free 0x%08x",
                      header_ptr->next_free, header_ptr->prev_free);
    }

    /* release lock */
    vast_sem_release(&(heap->lock));
}
//VASTM_EXPOVAST(vast_memheap_free);

#ifdef VAST_USING_MEMHEAP_AS_HEAP
static struct vast_memheap _heap;

void vast_system_heap_init(void *begin_addr, void *end_addr)
{
    /* initialize a default heap in the system */
    vast_memheap_init(&_heap,
                    "heap",
                    begin_addr,
                    (uint32_t)end_addr - (uint32_t)begin_addr);
}

/**
 *  The object type can be one of the follows with specific
 *  macros enabled:
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum rt_object_class_type
{
    RT_Object_Class_Null   = 0,                         /**< The object is not used. */
    RT_Object_Class_Thread,                             /**< The object is a thread. */
    RT_Object_Class_Semaphore,                          /**< The object is a semaphore. */
    RT_Object_Class_Mutex,                              /**< The object is a mutex. */
    RT_Object_Class_Event,                              /**< The object is a event. */
    RT_Object_Class_MailBox,                            /**< The object is a mail box. */
    RT_Object_Class_MessageQueue,                       /**< The object is a message queue. */
    RT_Object_Class_MemHeap,                            /**< The object is a memory heap */
    RT_Object_Class_MemPool,                            /**< The object is a memory pool. */
    RT_Object_Class_Device,                             /**< The object is a device */
    RT_Object_Class_Timer,                              /**< The object is a timer. */
    RT_Object_Class_Module,                             /**< The object is a module. */
    RT_Object_Class_Unknown,                            /**< The object is unknown. */
    RT_Object_Class_Static = 0x80                       /**< The object is a static object. */
};

/**
 * The information of the kernel object
 */
struct vast_object_information
{
    enum rt_object_class_type type;                     /**< object class type */
    list_t                 object_list;              /**< object list */
    size_t                 object_size;              /**< object size */
};

void *vast_malloc(size_t size)
{
    void *ptr;

    /* try to allocate in system heap */
    ptr = vast_memheap_alloc(&_heap, size);
    if (ptr == NULL)
    {
    #if 0
        struct object *object;
        list_t *node;
        struct vast_memheap *heap;
        struct vast_object_information *information = NULL;

        /* try to allocate on other memory heap */
        //information = vast_object_get_information(VAST_Object_Class_MemHeap);
        assert(information != NULL);
        for (node  = information->object_list.next;
             node != &(information->object_list);
             node  = node->next)
        {
            object = list_entry(node, struct object, entry);
            heap   = (struct vast_memheap *)object;

            assert(heap);
//            assert(vast_object_get_type(&heap->parent) == VAST_Object_Class_MemHeap);

            /* not allocate in the default system heap */
            if (heap == &_heap)
                continue;

            ptr = vast_memheap_alloc(heap, size);
            if (ptr != NULL)
                break;
        }
    #endif
    }

    return ptr;
}
//VASTM_EXPOVAST(vast_malloc);

void vast_free(void *rmem)
{
    vast_memheap_free(rmem);
}
//VASTM_EXPOVAST(vast_free);

void *vast_realloc(void *rmem, size_t newsize)
{
    void *new_ptr;
    struct vast_memheap_item *header_ptr;

    if (rmem == NULL)
        return vast_malloc(newsize);

    if (newsize == 0)
    {
        vast_free(rmem);
        return NULL;
    }

    /* get old memory item */
    header_ptr = (struct vast_memheap_item *)
                 ((uint8_t *)rmem - VAST_MEMHEAP_SIZE);

    new_ptr = vast_memheap_realloc(header_ptr->pool_ptr, rmem, newsize);
    if (new_ptr == NULL && newsize != 0)
    {
        /* allocate memory block from other memheap */
        new_ptr = vast_malloc(newsize);
        if (new_ptr != NULL && rmem != NULL)
        {
            size_t oldsize;

            /* get the size of old memory block */
            oldsize = MEMITEM_SIZE(header_ptr);
            if (newsize > oldsize)
                memcpy(new_ptr, rmem, oldsize);
            else
                memcpy(new_ptr, rmem, newsize);

            vast_free(rmem);
        }
    }

    return new_ptr;
}
//VASTM_EXPOVAST(vast_realloc);

void *vast_calloc(size_t count, size_t size)
{
    void *ptr;
    size_t total_size;

    total_size = count * size;
    ptr = vast_malloc(total_size);
    if (ptr != NULL)
    {
        /* clean memory */
        memset(ptr, 0, total_size);
    }

    return ptr;
}
//VASTM_EXPOVAST(vast_calloc);

#endif

#endif
