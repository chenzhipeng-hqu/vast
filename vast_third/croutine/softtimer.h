#ifndef _SOFTTIMER_H_
#define _SOFTTIMER_H_

#include <types.h>
#include <list.h>

typedef struct soft_timer
{
    struct list_head    entry;
    time_t              expires;
    volatile ubase_t    data;
    void (*cb)(struct soft_timer *st);
}soft_timer_t;

void soft_timer_add(struct soft_timer *st);
void soft_timer_del(struct soft_timer *st);
void soft_timer_mod(struct soft_timer *st, time_t expires);

int setup_soft_timer_service(void);
void soft_timer_task(void);

#endif
