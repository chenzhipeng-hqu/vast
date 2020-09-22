//#include <config.h>
#include <core/init.h>
#include <core/softtimer.h>
#include <core/utils.h>
#include <stdio.h>
#include <core/croutine.h>
//#include <third/croutine/port.h>
//#include <printk.h>
//#include <vast_core/jiffies.h>
//#include <board.h>


volatile time_t jiffies = INITIAL_JIFFIES;


static LIST_HEAD(timer_list);

void soft_timer_add(struct soft_timer *st)
{
    OS_CPU_SR cpu_sr;

    enter_critical();

	if ((st->cb) && (!(st->entry.next && st->entry.prev)))
		list_add_tail(&st->entry, &timer_list);
    exit_critical();
}

void soft_timer_del(struct soft_timer *st)
{
    OS_CPU_SR cpu_sr;

    enter_critical();
	if (st->entry.next && st->entry.prev)
		list_del(&st->entry);
    exit_critical();
}

void soft_timer_mod(struct soft_timer *st, time_t expires)
{
    OS_CPU_SR cpu_sr;

    enter_critical();
    list_del(&st->entry);
    st->expires = expires;
    list_add_tail(&st->entry, &timer_list);
    exit_critical();
}

static void soft_timer_task(void)
{
	struct soft_timer *iter, *n;

	list_for_each_entry_safe(iter, n, &timer_list, entry)
	{
		if (time_after_eq(jiffies, iter->expires))
		{
			soft_timer_del(iter);

			time_t start_jiffies = jiffies;

			//if (iter->cb)
			iter->cb(iter);

			if(jiffies - start_jiffies > 3)
				printf("timer %s spent %ldms\r\n",
						iter->name, (uint32_t)(jiffies-start_jiffies));
		}
	}
}

static void soft_timer_task_callback(struct task_ctrl_blk *tcb, ubase_t data)
{
    tSTART(tcb);

    for (;;)
    {
        soft_timer_task();   
        task_delay(tcb, 1);
    }

    tEND();
}

static tcb_t soft_timer_tcb;

int setup_soft_timer_service(void)
{
    task_create(&soft_timer_tcb, soft_timer_task_callback, "softtimer", 0);

    return 0;
}
late_initcall(setup_soft_timer_service);

list_t *timer_lists(void)
{
    return &timer_list;
}

