#ifndef _SOFTTIMER_H_
#define _SOFTTIMER_H_

#include <core/list.h>
#include <core/types.h>
#include <sys/_timeval.h>

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

typedef struct soft_timer
{
    struct list_head    entry;
    time_t              expires;
    ubase_t    			data;
    void (*cb)(struct soft_timer *st);
}soft_timer_t;

extern volatile time_t jiffies;

void soft_timer_add(struct soft_timer *st);
void soft_timer_del(struct soft_timer *st);
void soft_timer_mod(struct soft_timer *st, time_t expires);

//int setup_soft_timer_service(void);
//void soft_timer_task(void);

#endif
